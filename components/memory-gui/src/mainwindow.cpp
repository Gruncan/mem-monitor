
#include "mainwindow.h"

#include "QPushButton"
#include "qmtcloadersgroup.h"
#include "ui/ui_mainwindow.h"
#include <QDesktopServices>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
                        QMainWindow(parent),
                        ui(new Ui::MainWindow),
                        decoderThread(new QThread),
                        monitorThread(new QThread)
{
    ui->setupUi(this);  // Move this first

    decoder = std::make_shared<mtc::MtcDecoder>();
    decoderWorker = new DecoderWorker(nullptr, decoder);
    decoderMonitor = new DecodeMonitor(nullptr, decoder);

    std::vector<QMtcLoader*> loaders;
    for (int i = 1; i <= 5; i++) {
        loaders.push_back(new QMtcLoader(this, QString("Load %1").arg(i).toStdString().c_str()));
    }

    QMtcLoadersGroup* loaderGroup = new QMtcLoadersGroup(this, loaders);
    loaderGroup->setGeometry(QRect(300, 720, 900, 150));


    // decoderWorker->moveToThread(decoderThread);
    // decoderMonitor->moveToThread(monitorThread);
    //
    // connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::loadButtonClick);
    //
    // // connect(decoderThread, &QThread::started, decoderWorker, &DecoderWorker::workerDecode);
    // connect(decoderWorker, &DecoderWorker::workerDecodeFinished, this, &MainWindow::loaded);
    // connect(decoderWorker, &DecoderWorker::workerDecodeFinished, decoderThread, &QThread::quit);
    // connect(decoderThread, &QThread::finished, decoderWorker, &QObject::deleteLater);
    //
    // // connect(monitorThread, &QThread::started, decoderMonitor, &DecodeMonitor::monitorProgress);
    // connect(decoderMonitor, &DecodeMonitor::progressQueried, this, &MainWindow::updateProgress);
    // connect(monitorThread, &QThread::finished, decoderMonitor, &QObject::deleteLater);
    //
    // connect(this, &MainWindow::startDecoding, decoderMonitor, &DecodeMonitor::monitorProgress);
    // connect(this, &MainWindow::startDecoding, decoderWorker, &DecoderWorker::workerDecode);
    //
    // decoderThread->start();
    // monitorThread->start();



}


// void MainWindow::updateProgress(const int progress) const {
//     ui->progressBar->setValue(progress);
// }

MainWindow::~MainWindow() {
    // delete ui;
    // decoderThread->wait();
    // decoderThread->quit();
    // decoderThread->wait();
    // decoderThread->quit();
    // delete decoderThread;
    // delete decoderWorker;
    // delete monitorThread;
    // delete decoderMonitor;
}


// void MainWindow::loadButtonClick() {
//     const QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "Memory Time Encoding (*.mtc);;All Files (*)");
//     if (!filePath.isEmpty()) {
//         qDebug() << "Selected file:" << filePath;
//     }else {
//         qDebug() << "No file selected";
//     }
//     decoderWorker->setDecodeFilename(filePath.toStdString());
//     ui->progressBar->setVisible(true);
//
//     emit startDecoding();
// }


// void MainWindow::loaded() {
//     qDebug() << "Loaded";
//     ui->progressBar->setValue(100);
//     QVector<double> x(101), y(101); // initialize with entries 0..100
//     for (int i=0; i<101; ++i)
//     {
//         x[i] = i/50.0 - 1; // x goes from -1 to 1
//         y[i] = x[i]*x[i]; // let's plot a quadratic function
//     }
//     // create graph and assign data to it:
//     ui->plot->addGraph();
//     ui->plot->graph(0)->setData(x, y);
//     // give the axes some labels:
//     ui->plot->xAxis->setLabel("x");
//     ui->plot->yAxis->setLabel("y");
//     // set axes ranges, so we see all data:
//     ui->plot->xAxis->setRange(-1, 1);
//     ui->plot->yAxis->setRange(0, 1);
//     ui->plot->replot();
//     // monitorThread->terminate();
//     // decoderThread->terminate();
// };


