#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "decoder-monitor.h"
#include "decoder-worker.h"
#include "qmtcloadersgroup.h"


#include <QMainWindow>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private Q_SLOTS:
     void mtcCategoriesChanged(const QString& category, const QString& plot, bool enabled);

Q_SIGNALS:
    void startDecoding();

private:
    Ui::MainWindow* ui;
    QThread* decoderThread;
    QThread* monitorThread;
    DecoderWorker* decoderWorker;
    DecodeMonitor* decoderMonitor;
    std::shared_ptr<mtc::MtcDecoder> decoder;
    QMtcLoadersGroup* loadersGroup;

};

#endif //MAINWINDOW_H
