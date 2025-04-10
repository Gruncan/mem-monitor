import { Component, OnInit } from '@angular/core';
import { CommonModule } from '@angular/common';
import { MetricsSelectorComponent } from './components/metrics-selector/metrics-selector.component';
import { MetricsChartComponent } from './components/metrics-chart/metrics-chart.component';
import { FileUploaderComponent } from './components/file-uploader/file-uploader.component';
import { TauriService } from './services/tauri.service';
import { DataFile, DataPoint, Metric } from './models/metric.model';

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [
    CommonModule,
    MetricsSelectorComponent,
    MetricsChartComponent,
    FileUploaderComponent
  ],
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent implements OnInit {
  title = 'memory-metrics-visualizer';

  dataFiles: DataFile[] = [];
  availableMetrics: Metric[] = [];
  selectedMetricsData: {
    label: string;
    data: DataPoint[];
    color: string;
  }[] = [];

  constructor(private tauriService: TauriService) {}

  async ngOnInit(): Promise<void> {
    this.availableMetrics = await this.tauriService.getAvailableMetrics();
  }

  onFilesChange(files: DataFile[]): void {
    this.dataFiles = files;
  }

  async onSelectionChange(selection: any[]): Promise<void> {
    this.selectedMetricsData = [];

    for (const item of selection) {
      if (item.visible) {
        const metric = this.availableMetrics.find(m => m.id === item.metricId);
        if (!metric) continue;

        const file = this.dataFiles.find(f => f.id === item.fileId);
        if (!file) continue;

        const data = await this.tauriService.getMetricData(item.fileId, item.metricId);

        this.selectedMetricsData.push({
          label: `${file.name} - ${metric.name}`,
          data,
          color: item.color
        });
      }
    }
  }
}