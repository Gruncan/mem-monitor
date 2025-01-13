
#ifndef QPLOTRENDER_H
#define QPLOTRENDER_H
#include "animation-data-container.h"
#include "qcustomplot.h"

#include "mtccdecoder.h"

class QPlotRender : public QObject {
    Q_OBJECT

public:
    explicit QPlotRender(QCustomPlot* plot);

    ~QPlotRender();

    void rewind();

    void forward();

    void setTimeSpacing(int spacing);


public Q_SLOTS:
    void queueRendering(MtcPointMap* point_map, const QVector<double>& times, uint64_t length, QCPGraph* graph);

    void queueAnimationRendering(MtcPointMap* point_map, MtcTime* times, uint64_t length, uint64_t timesLength, QCPGraph* graph, int timeSpacing);

    void updatePlot();

    void startAnimation();

    void stopAnimation();


private:

    void processBatch(MtcPointMap* points, size_t start_index, size_t end_index, uint64_t sample_rate);

    static uint64_t getSampleRate(size_t length);

private:
    QCustomPlot* plot;

    QVector<double> times;
    QVector<double> values;

    uint64_t timeSum;
    double valueMax;
    QTimer* animationTimer;

    int currentIndex;
    uint64_t segmentSize;
    QCPGraph* currentGraph;
    int timeSpacing;

    AnimationDataContainer<double>* animationData;
};




#endif //QPLOTRENDER_H
