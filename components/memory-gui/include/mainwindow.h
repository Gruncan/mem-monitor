/***********************************
 *
 * @brief The main window of the graphical interface
 *
 * @file mainwindow.h
 *
************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "decoder-monitor.h"
#include "decoder-worker.h"
#include "qmemoryanimatecontrols.h"
#include "qmemoryplotter.h"
#include "qmtcloadersgroup.h"
#include "qplotcontrolsidebar.h"

#include <QMainWindow>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

  public Q_SLOTS:

    void initialisePlot();

  Q_SIGNALS:
    void startDecoding();

  private:
    Ui::MainWindow* ui;
    QThread* decoderThread;
    QThread* monitorThread;
    DecoderWorker* decoderWorker;
    DecodeMonitor* decoderMonitor;
    // std::shared_ptr<mtc::MtcDecoder> decoder;
    QMtcLoadersGroup* loadersGroup;
    QPlotControlSidebar* sidebar;

    QMemoryPlotter* plotter;

    QMemoryAnimateControls* animateControls;
};

#endif // MAINWINDOW_H
