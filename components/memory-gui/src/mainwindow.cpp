
#include "mainwindow.h"
#include "ui/ui_mainwindow.h"
#include "QPushButton"
#include <QDesktopServices>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
                        QMainWindow(parent),
                        ui(new Ui::MainWindow),
                        decoderThread(new QThread),
                        monitorThread(new QThread)
{
    decoder = std::make_shared<mtc::MtcDecoder>();
    decoderWorker = new DecoderWorker(nullptr, decoder);
    decoderMonitor = new DecodeMonitor(nullptr, decoder);
    ui->setupUi(this);
    decoderWorker->moveToThread(decoderThread);
    decoderMonitor->moveToThread(monitorThread);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::loadButtonClick);

    // connect(decoderThread, &QThread::started, decoderWorker, &DecoderWorker::workerDecode);
    connect(decoderWorker, &DecoderWorker::workerDecodeFinished, this, &MainWindow::loaded);
    connect(decoderWorker, &DecoderWorker::workerDecodeFinished, decoderThread, &QThread::quit);
    connect(decoderThread, &QThread::finished, decoderWorker, &QObject::deleteLater);

    // connect(monitorThread, &QThread::started, decoderMonitor, &DecodeMonitor::monitorProgress);
    connect(decoderMonitor, &DecodeMonitor::progressQueried, this, &MainWindow::updateProgress);
    connect(monitorThread, &QThread::finished, decoderMonitor, &QObject::deleteLater);

    connect(this, &MainWindow::startDecoding, decoderMonitor, &DecodeMonitor::monitorProgress);
    connect(this, &MainWindow::startDecoding, decoderWorker, &DecoderWorker::workerDecode);

    decoderThread->start();
    monitorThread->start();

}


void MainWindow::updateProgress(const int progress) const {
    ui->progressBar->setValue(progress);
}

MainWindow::~MainWindow() {
    delete ui;
    decoderThread->wait();
    decoderThread->quit();
    decoderThread->wait();
    decoderThread->quit();
    delete decoderThread;
    delete decoderWorker;
    delete monitorThread;
    delete decoderMonitor;
}


void MainWindow::loadButtonClick() {
    const QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "Memory Time Encoding (*.mtc);;All Files (*)");
    if (!filePath.isEmpty()) {
        qDebug() << "Selected file:" << filePath;
    }else {
        qDebug() << "No file selected";
    }
    decoderWorker->setDecodeFilename(filePath.toStdString());
    ui->progressBar->setVisible(true);

    emit startDecoding();
}


void MainWindow::loaded() {
    qDebug() << "Loaded";
    ui->progressBar->setValue(100);
    // monitorThread->terminate();
    // decoderThread->terminate();
};


