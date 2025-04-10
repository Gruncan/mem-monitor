export interface Metric {
    id: string;
    name: string;
    unit: string;
    description?: string;
}

export interface DataPoint {
    timestamp: number;
    value: number;
}

export interface DataFile {
    id: string;
    name: string;
    metrics: Map<string, DataPoint[]>;
}