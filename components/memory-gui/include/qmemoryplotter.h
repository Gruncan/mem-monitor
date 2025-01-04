
#ifndef QMEMORYPLOTTER_H
#define QMEMORYPLOTTER_H
#include "qcustomplot.h"

#include <mtc-object.h>
#include <mtc-point.h>


class QMemoryPlotter : public QObject {
    Q_OBJECT

public:

    explicit QMemoryPlotter(QCustomPlot* plot);

    void togglePlot(const QString& category, const QString& plot, bool enabled, std::shared_ptr<mtc::MtcObject> object);

    void clearPlots();

private:

    QCustomPlot* _plot;
    QTimer _timer;
    QVector<double> times;
    QVector<double> values;
    double valueMax;

    std::map<uint8_t, QString> plotsEnabled;

    void processBatch(const std::vector<mtc::MtcPoint*>* points, size_t start, uint64_t skipFactor,
                            size_t end, double& timeSum);

    void plotData(const std::vector<mtc::MtcPoint*>* points, uint64_t length);

    void updatePlots(const std::shared_ptr<mtc::MtcObject>& object);

    static uint64_t getSkipFactor(size_t length);

    void requestUpdate();

    void updatePlot();

    void finalisePlot();


};


#endif //QMEMORYPLOTTER_H
