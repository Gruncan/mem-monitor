
#include "mainwindow.h"
#include "ui/ui_mainwindow.h"
#include "QPushButton"
#include <QDesktopServices>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::loadButtonClick);
}


MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::loadButtonClick() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "Memory Time Encoding (*.mtc);;All Files (*)");
    if (!filePath.isEmpty()) {
        qDebug() << "Selected file:" << filePath;
    }else {
        qDebug() << "No file selected";
    }
}


