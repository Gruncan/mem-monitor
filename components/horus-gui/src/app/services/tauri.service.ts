import { Injectable } from '@angular/core';
import { invoke } from '@tauri-apps/api/core';
import { open } from '@tauri-apps/plugin-dialog';
import { DataFile, Metric } from '../models/metric.model';

@Injectable({
    providedIn: 'root'
})
export class TauriService {

    constructor() {}

    async uploadDataFile(): Promise<DataFile | null> {
        try {
            const selected = await open({
                multiple: false,
                filters: [{
                    name: 'Metrics Data',
                    extensions: ['json', 'csv']
                }]
            });

            if (!selected) return null;

            const result = await invoke('parse_data_file', { path: selected });
            return result as DataFile;
        } catch (error) {
            console.error('Error uploading file:', error);
            return null;
        }
    }

    async getAvailableMetrics(): Promise<Metric[]> {
        try {
            const metrics = await invoke('get_available_metrics');
            return metrics as Metric[];
        } catch (error) {
            console.error('Error fetching metrics:', error);
            return [];
        }
    }

    async getMetricData(fileId: string, metricId: string): Promise<any[]> {
        try {
            const data = await invoke('get_metric_data', { fileId, metricId });
            return data as any[];
        } catch (error) {
            console.error('Error fetching metric data:', error);
            return [];
        }
    }
}