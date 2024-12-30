
#include "mainwindow.h"
#include "ui/ui_mainwindow.h"
#include "QPushButton"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->myButton, &QPushButton::clicked, this, [this]() {
        ui->myButton->setText("Button Clicked!");
    });
}


MainWindow::~MainWindow() {
    delete ui;
}


