
#ifndef QPLOTRENDER_H
#define QPLOTRENDER_H
#include "qcustomplot.h"

#include <mtc-point.h>


class QPlotRender : public QObject {
    Q_OBJECT

public:
    explicit QPlotRender(QCustomPlot* plot);

    ~QPlotRender();

public Q_SLOTS:
    void queueRendering(const std::vector<mtc::MtcPoint*>* points, uint64_t length, QCPGraph* graph);

private:

    void processBatch(const std::vector<mtc::MtcPoint*>* points, size_t start_index, size_t end_index, uint64_t sample_rate);

    static uint64_t getSampleRate(size_t length);

private:
    QCustomPlot* plot;

    QVector<double> times;
    QVector<double> values;

    uint64_t timeSum;
    double valueMax;
};




#endif //QPLOTRENDER_H
