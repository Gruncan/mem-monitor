
#include "mainwindow.h"

#include "QPushButton"
#include "qmtcloadersgroup.h"
#include "qplotcontrolsidebar.h"
#include "ui/ui_mainwindow.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <iostream>

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

    loadersGroup = new QMtcLoadersGroup(this, loaders);
    loadersGroup->setGeometry(QRect(300, 720, 900, 150));

    QPlotControlSidebar* sidebar = new QPlotControlSidebar(this);
    sidebar->setGeometry(QRect(10, 10, 200, 900));
    sidebar->setCategories(mtc::MTC_CATEGORIES);


    connect(sidebar, &QPlotControlSidebar::categoriesChanged, this, &MainWindow::mtcCategoriesChanged);

}


void MainWindow::mtcCategoriesChanged(const QString& category, const QString& plot, bool enabled) {
    auto value = mtc::MTC_KEY_MAPPING.find(plot.toStdString());
    if (value == mtc::MTC_KEY_MAPPING.end()) {
        qDebug() << "Unable to find plot name: " << plot;
        return;
    }
    if (!enabled) {
        ui->plot->clearGraphs();
        return;
    }

    uint32_t key = value->second;
    std::shared_ptr<mtc::MtcObject> object = loadersGroup->getLoader(0)->getMtcData();

    std::vector<mtc::MtcPoint> mtcPoints = object->get_points();

    double time_sum = 0;
    double max_value = 0;
    QVector<double> time, values;
    for (int i = 0; i < mtcPoints.size(); i += 50) {
        mtc::MtcPoint p = mtcPoints.at(i);
        time_sum += p.time_offset;
        time.push_back(time_sum);
        if (p.data[key] > max_value) {
            max_value = p.data[key];
        }
        values.push_back(p.data[key]);
    }

    qDebug() << time.size();

    ui->plot->addGraph();
    ui->plot->graph(0)->setData(time, values);
    ui->plot->xAxis->setLabel("Time");
    ui->plot->yAxis->setLabel("Memory");

    ui->plot->xAxis->setRange(0, time_sum);
    ui->plot->yAxis->setRange(0, max_value);
    ui->plot->replot();
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


