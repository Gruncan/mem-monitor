use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::fs::File;
use std::io::{BufReader, Read};
use std::path::Path;
use std::sync::Mutex;
use tauri::{command, Builder};
use uuid::Uuid;

#[derive(Debug, Serialize, Deserialize, Clone)]
struct Metric {
    id: String,
    name: String,
    unit: String,
    description: Option<String>,
}

#[derive(Debug, Serialize, Deserialize, Clone)]
struct DataPoint {
    timestamp: u64,
    value: f64,
}

#[derive(Debug, Serialize, Deserialize, Clone)]
struct DataFile {
    id: String,
    name: String,
    metrics: HashMap<String, Vec<DataPoint>>,
}

struct AppState {
    data_files: Mutex<HashMap<String, DataFile>>,
    available_metrics: Vec<Metric>,
}

impl AppState {
    fn new() -> Self {
        Self {
            data_files: Mutex::new(HashMap::new()),
            available_metrics: vec![
                Metric {
                    id: "mem_used".to_string(),
                    name: "Memory Used".to_string(),
                    unit: "MB".to_string(),
                    description: Some("Total memory currently in use".to_string()),
                },
                Metric {
                    id: "mem_free".to_string(),
                    name: "Memory Free".to_string(),
                    unit: "MB".to_string(),
                    description: Some("Available memory not currently allocated".to_string()),
                },
                Metric {
                    id: "mem_cached".to_string(),
                    name: "Memory Cached".to_string(),
                    unit: "MB".to_string(),
                    description: Some("Memory used for file system cache".to_string()),
                },
                Metric {
                    id: "swap_used".to_string(),
                    name: "Swap Used".to_string(),
                    unit: "MB".to_string(),
                    description: Some("Amount of swap space currently in use".to_string()),
                },
                Metric {
                    id: "cpu_usage".to_string(),
                    name: "CPU Usage".to_string(),
                    unit: "%".to_string(),
                    description: Some("Percentage of CPU currently in use".to_string()),
                },
            ],
        }
    }
}

#[command]
async fn get_available_metrics(state: tauri::State<'_, AppState>) -> Result<Vec<Metric>, String> {
    Ok(state.available_metrics.clone())
}

#[command]
async fn parse_data_file(path: String, state: tauri::State<'_, AppState>) -> Result<DataFile, String> {
    let file_path = Path::new(&path);
    let file_name = file_path
        .file_name()
        .and_then(|name| name.to_str())
        .unwrap_or("Unknown File")
        .to_string();

    let file = File::open(file_path).map_err(|e| format!("Failed to open file: {}", e))?;
    let mut reader = BufReader::new(file);
    let mut content = String::new();
    reader
        .read_to_string(&mut content)
        .map_err(|e| format!("Failed to read file: {}", e))?;

    let parsed_data = if path.ends_with(".json") {
        parse_json_data(&content)?
    } else if path.ends_with(".csv") {
        parse_csv_data(&content, &state.available_metrics)?
    } else {
        return Err("Unsupported file format. Please use JSON or CSV.".to_string());
    };

    let file_id = Uuid::new_v4().to_string();
    let data_file = DataFile {
        id: file_id.clone(),
        name: file_name,
        metrics: parsed_data,
    };

    state.data_files
        .lock()
        .map_err(|_| "Failed to lock mutex".to_string())?
        .insert(file_id.clone(), data_file.clone());

    Ok(data_file)
}

#[command]
async fn get_metric_data(file_id: String, metric_id: String, state: tauri::State<'_, AppState>)
                                        -> Result<Vec<DataPoint>, String> {
    let data_files = state.data_files
        .lock()
        .map_err(|_| "Failed to lock mutex".to_string())?;

    let data_file = data_files
        .get(&file_id)
        .ok_or_else(|| "Data file not found".to_string())?;

    let data = data_file
        .metrics
        .get(&metric_id)
        .ok_or_else(|| "Metric not found in data file".to_string())?;

    Ok(data.clone())
}

fn parse_json_data(content: &str) -> Result<HashMap<String, Vec<DataPoint>>, String> {
    let parsed: HashMap<String, Vec<DataPoint>> =
        serde_json::from_str(content).map_err(|e| format!("Failed to parse JSON: {}", e))?;
    Ok(parsed)
}

fn parse_csv_data(content: &str, available_metrics: &[Metric])
                                        -> Result<HashMap<String, Vec<DataPoint>>, String> {
    let mut result = HashMap::new();
    let mut lines = content.lines();

    let header = lines
        .next()
        .ok_or_else(|| "CSV file is empty".to_string())?;
    let columns: Vec<&str> = header.split(',').map(|s| s.trim()).collect();

    let timestamp_idx = columns
        .iter()
        .position(|&col| col.to_lowercase() == "timestamp" || col.to_lowercase() == "time")
        .ok_or_else(|| "No timestamp column found in CSV".to_string())?;

    let mut metric_indices = HashMap::new();
    for (idx, col) in columns.iter().enumerate() {
        if idx != timestamp_idx {
            if let Some(metric) = available_metrics
                .iter()
                .find(|m| m.name.to_lowercase() == col.to_lowercase())
            {
                metric_indices.insert(idx, metric.id.clone());
                result.insert(metric.id.clone(), Vec::new());
            }
        }
    }

    for (row_idx, line) in lines.enumerate() {
        let row_number = row_idx + 2;
        let values: Vec<&str> = line.split(',').map(|s| s.trim()).collect();

        if values.len() != columns.len() {
            return Err(format!(
                "Row {} has incorrect number of columns",
                row_number
            ));
        }

        let timestamp = values[timestamp_idx]
            .parse::<u64>()
            .map_err(|_| format!("Invalid timestamp at row {}", row_number))?;

        for (&idx, metric_id) in &metric_indices {
            if let Some(value_str) = values.get(idx) {
                let value = value_str.parse::<f64>().map_err(|_| {
                    format!("Invalid value at row {}, column {}", row_number, idx + 1)
                })?;

                if let Some(data_points) = result.get_mut(metric_id) {
                    data_points.push(DataPoint { timestamp, value });
                }
            }
        }
    }

    Ok(result)
}

fn main() {
    let app_state = AppState::new();

    Builder::default()
        .manage(app_state)
        .invoke_handler(tauri::generate_handler![
            get_available_metrics,
            parse_data_file,
            get_metric_data
        ])
        .run(tauri::generate_context!())
        .expect("Error while running tauri application");
}