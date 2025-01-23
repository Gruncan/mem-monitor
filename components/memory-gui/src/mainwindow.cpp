
#include "mainwindow.h"

#include "qmtcloadersgroup.h"
#include "ui/ui_mainwindow.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), decoderThread(new QThread), monitorThread(new QThread) {
    ui->setupUi(this); // Move this first
    ui->plot->setDisabled(true);

    sidebar = new QPlotControlSidebar(this);
    sidebar->setGeometry(QRect(10, 10, 175, 710));
    sidebar->setCategories(mtc::MTC_CATEGORIES);

    std::vector<QMtcLoader*> loaders;
    for (int i = 1; i <= 3; i++) {
        loaders.push_back(new QMtcLoader(this, QString("Load %1").arg(i).toStdString().c_str(), sidebar));
    }

    loadersGroup = new QMtcLoadersGroup(this, loaders);
    loadersGroup->setGeometry(QRect(600, 520, 600, 150));

    animateControls = new QMemoryAnimateControls(this);
    animateControls->setGeometry(QRect(400, 520, 130, 120));

    // TODO update this index
    plotter = new QMemoryPlotter(this, ui->plot, loadersGroup->getLoader(0), animateControls);

    // connect(sidebar, &QPlotControlSidebar::categoriesChanged, plotter, &QMemoryPlotter::plotToggleChange);
}

void MainWindow::initialisePlot() {
    ui->plot->setDisabled(false);
    plotter->setIsLoaded(true);
    connect(sidebar, &QPlotControlSidebar::categoriesChanged, plotter, &QMemoryPlotter::plotToggleChange);
}

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
