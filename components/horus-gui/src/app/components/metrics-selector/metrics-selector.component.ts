import { Component, EventEmitter, Input, Output } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';
import { Metric, DataFile } from '../../models/metric.model';

interface MetricSelection {
    fileId: string;
    metricId: string;
    color: string;
    visible: boolean;
}

@Component({
    selector: 'app-metrics-selector',
    standalone: true,
    imports: [CommonModule, FormsModule],
    template: `
    <div class="metrics-selector">
      <h3>Select Metrics</h3>
      
      <div class="data-files">
        <div *ngFor="let file of dataFiles" class="file-section">
          <div class="file-header">
            <h4>{{ file.name }}</h4>
          </div>
          
          <div class="metrics-list">
            <div *ngFor="let metric of availableMetrics" class="metric-item">
              <input 
                type="checkbox" 
                [id]="file.id + '-' + metric.id"
                (change)="toggleMetric(file.id, metric.id, $event)"
              />
              <label [for]="file.id + '-' + metric.id">
                {{ metric.name }} ({{ metric.unit }})
              </label>
              <input 
                type="color" 
                [value]="getMetricColor(file.id, metric.id)"
                (change)="changeMetricColor(file.id, metric.id, $event)"
                [disabled]="!isMetricSelected(file.id, metric.id)"
              />
            </div>
          </div>
        </div>
      </div>
    </div>
  `,
    styles: [`
    .metrics-selector {
      padding: 1rem;
      background: #f5f5f7;
      border-radius: 8px;
      box-shadow: 0 2px 4px rgba(0,0,0,0.05);
    }
    
    .file-section {
      margin-bottom: 1rem;
      padding: 0.5rem;
      background: white;
      border-radius: 6px;
    }
    
    .file-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 0.5rem;
    }
    
    .metrics-list {
      display: flex;
      flex-direction: column;
      gap: 0.5rem;
    }
    
    .metric-item {
      display: flex;
      align-items: center;
      gap: 0.5rem;
    }
  `]
})
export class MetricsSelectorComponent {
    @Input() dataFiles: DataFile[] = [];
    @Input() availableMetrics: Metric[] = [];
    @Output() selectionChange = new EventEmitter<MetricSelection[]>();

    selectedMetrics: MetricSelection[] = [];

    // Default colors for metrics
    private defaultColors = [
        '#FF6384', '#36A2EB', '#FFCE56', '#4BC0C0',
        '#9966FF', '#FF9F40', '#2ECC71', '#E74C3C'
    ];

    constructor() {}

    toggleMetric(fileId: string, metricId: string, event: Event): void {
        const isChecked = (event.target as HTMLInputElement).checked;

        if (isChecked) {
            // Add the metric to selection
            const colorIndex = this.selectedMetrics.length % this.defaultColors.length;
            this.selectedMetrics.push({
                fileId,
                metricId,
                color: this.defaultColors[colorIndex],
                visible: true
            });
        } else {
            // Remove the metric from selection
            this.selectedMetrics = this.selectedMetrics.filter(
                m => !(m.fileId === fileId && m.metricId === metricId)
            );
        }

        this.selectionChange.emit(this.selectedMetrics);
    }

    changeMetricColor(fileId: string, metricId: string, event: Event): void {
        const color = (event.target as HTMLInputElement).value;

        this.selectedMetrics = this.selectedMetrics.map(m => {
            if (m.fileId === fileId && m.metricId === metricId) {
                return { ...m, color };
            }
            return m;
        });

        this.selectionChange.emit(this.selectedMetrics);
    }

    getMetricColor(fileId: string, metricId: string): string {
        const metric = this.selectedMetrics.find(
            m => m.fileId === fileId && m.metricId === metricId
        );
        return metric?.color || '#000000';
    }

    isMetricSelected(fileId: string, metricId: string): boolean {
        return this.selectedMetrics.some(
            m => m.fileId === fileId && m.metricId === metricId
        );
    }
}