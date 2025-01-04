
#ifndef QMEMORYPLOTTER_H
#define QMEMORYPLOTTER_H
#include "qcustomplot.h"
#include "qmtcloader.h"
#include "qplotrender.h"

#include <mtc-point.h>


class QMemoryPlotter : public QObject {
    Q_OBJECT

public:

    explicit QMemoryPlotter(QCustomPlot* plot, QMtcLoader* loader);

    void addPlot(uint8_t key);

    void removePlot(uint8_t key);

public Q_SLOTS:

    void plotToggleChange(const QString& category, const QString& plotString, bool enabled);

Q_SIGNALS:
    void queueRendering(const std::vector<mtc::MtcPoint*>* points, uint64_t length, QCPGraph* graph);


private:

    QCustomPlot* _plot;

    std::map<uint8_t, QCPGraph*> plotsEnabled;

    QMtcLoader* _loader;

    QPlotRender* _plotRender;

    QThread* renderThread;

};


#endif //QMEMORYPLOTTER_H
