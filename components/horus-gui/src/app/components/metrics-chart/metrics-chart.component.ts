import { Component, Input, OnChanges, AfterViewInit, ElementRef, SimpleChanges, ViewChild, OnDestroy } from '@angular/core';
import { CommonModule } from '@angular/common';
import { DataPoint } from '../../models/metric.model';
import Chart from 'chart.js/auto';
import 'chartjs-adapter-date-fns';
import zoomPlugin from 'chartjs-plugin-zoom';

Chart.register(zoomPlugin);

@Component({
    selector: 'app-metrics-chart',
    standalone: true,
    imports: [CommonModule],
    template: `
        <div class="chart-container">
            <canvas #chartCanvas></canvas>

            <div class="chart-controls">
                <button (click)="resetZoom()" class="control-btn">Reset Zoom</button>
            </div>
        </div>
    `,
    styles: [`
        .chart-container {
            width: 100%;
            height: 100%;
            display: flex;
            flex-direction: column;
        }

        canvas {
            flex-grow: 1;
        }

        .chart-controls {
            display: flex;
            justify-content: center;
            padding: 1rem;
            gap: 1rem;
        }

        .control-btn {
            padding: 0.5rem 1rem;
            background: #f0f0f0;
            border: 1px solid #ddd;
            border-radius: 4px;
            cursor: pointer;
        }

        .control-btn:hover {
            background: #e0e0e0;
        }
    `]
})
export class MetricsChartComponent implements OnChanges, AfterViewInit, OnDestroy {
    @ViewChild('chartCanvas') chartCanvas!: ElementRef<HTMLCanvasElement>;

    @Input() datasets: {
        label: string;
        data: DataPoint[];
        color: string;
    }[] = [];

    private chart: Chart | null = null;

    constructor() {}

    ngAfterViewInit(): void {
        this.createChart();
    }

    ngOnChanges(changes: SimpleChanges): void {
        if (changes['datasets'] && this.chart) {
            this.updateChartData();
        }
    }

    ngOnDestroy(): void {
        if (this.chart) {
            this.chart.destroy();
            this.chart = null;
        }
    }

    createChart(): void {
        const ctx = this.chartCanvas.nativeElement.getContext('2d');
        if (!ctx) return;

        this.chart = new Chart(ctx, {
            type: 'line',
            data: {
                datasets: []
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    x: {
                        type: 'time',
                        time: {
                            unit: 'minute',
                            displayFormats: {
                                minute: 'HH:mm:ss'
                            }
                        },
                        title: {
                            display: true,
                            text: 'Time'
                        }
                    },
                    y: {
                        beginAtZero: false,
                        title: {
                            display: true,
                            text: 'Value'
                        }
                    }
                },
                plugins: {
                    legend: {
                        display: true,
                        position: 'top',
                    },
                    tooltip: {
                        enabled: true,
                        mode: 'index',
                        intersect: false,
                    },
                    zoom: {
                        pan: {
                            enabled: true,
                            mode: 'xy'
                        },
                        zoom: {
                            wheel: {
                                enabled: true,
                            },
                            pinch: {
                                enabled: true
                            },
                            mode: 'xy',
                        }
                    }
                },
                interaction: {
                    mode: 'nearest',
                    axis: 'x',
                    intersect: false
                },
            }
        });

        this.updateChartData();
    }

    updateChartData(): void {
        if (!this.chart) return;

        this.chart.data.datasets = this.datasets.map(ds => {
            return {
                label: ds.label,
                data: ds.data.map(d => ({
                    x: new Date(d.timestamp),
                    y: d.value
                })),
                backgroundColor: this.hexToRgba(ds.color, 0.2),
                borderColor: ds.color,
                borderWidth: 2,
                pointBackgroundColor: ds.color,
                pointBorderColor: '#fff',
                pointHoverBackgroundColor: '#fff',
                pointHoverBorderColor: ds.color,
                fill: false,
                tension: 0.1
            };
        }) as any;

        this.chart.update();
    }

    resetZoom(): void {
        if (this.chart) {
            this.chart.resetZoom();
        }
    }

    private hexToRgba(hex: string, alpha: number): string {
        const r = parseInt(hex.slice(1, 3), 16);
        const g = parseInt(hex.slice(3, 5), 16);
        const b = parseInt(hex.slice(5, 7), 16);
        return `rgba(${r}, ${g}, ${b}, ${alpha})`;
    }
}
