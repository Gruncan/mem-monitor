import { Component, EventEmitter, Output } from '@angular/core';
import { CommonModule } from '@angular/common';
import { TauriService } from '../../services/tauri.service';
import { DataFile } from '../../models/metric.model';

@Component({
    selector: 'app-file-uploader',
    standalone: true,
    imports: [CommonModule],
    template: `
        <div class="file-uploader">
            <button (click)="uploadFile()" class="upload-btn">
                <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                    <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
                    <polyline points="17 8 12 3 7 8"></polyline>
                    <line x1="12" y1="3" x2="12" y2="15"></line>
                </svg>
                Upload Data File
            </button>

            <div class="files-list">
                <div *ngFor="let file of uploadedFiles" class="file-item">
                    <span>{{ file.name }}</span>
                    <button (click)="removeFile(file.id)" class="remove-btn">
                        <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                            <line x1="18" y1="6" x2="6" y2="18"></line>
                            <line x1="6" y1="6" x2="18" y2="18"></line>
                        </svg>
                    </button>
                </div>
            </div>
        </div>
    `,
    styles: [`
        .file-uploader {
            padding: 1rem;
        }

        .upload-btn {
            display: flex;
            align-items: center;
            gap: 0.5rem;
            padding: 0.75rem 1.5rem;
            background: #007bff;
            color: white;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-weight: 500;
        }

        .upload-btn:hover {
            background: #0069d9;
        }

        .files-list {
            margin-top: 1rem;
            display: flex;
            flex-direction: column;
            gap: 0.5rem;
        }

        .file-item {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 0.5rem;
            background: #f8f9fa;
            border-radius: 4px;
        }

        .remove-btn {
            background: none;
            border: none;
            color: #dc3545;
            cursor: pointer;
            padding: 0.25rem;
            display: flex;
            align-items: center;
            justify-content: center;
        }

        .remove-btn:hover {
            color: #c82333;
        }
    `]
})
export class FileUploaderComponent {
    @Output() filesChange = new EventEmitter<DataFile[]>();
    uploadedFiles: DataFile[] = [];

    constructor(private tauriService: TauriService) {}

    async uploadFile(): Promise<void> {
        const file = await this.tauriService.uploadDataFile();

        if (file) {
            this.uploadedFiles.push(file);
            this.filesChange.emit(this.uploadedFiles);
        }
    }

    removeFile(fileId: string): void {
        this.uploadedFiles = this.uploadedFiles.filter(file => file.id !== fileId);
        this.filesChange.emit(this.uploadedFiles);
    }
}