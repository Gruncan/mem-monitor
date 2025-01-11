


#include "animation-data-container.h"
#include "qmemoryplotter.h"
#include "qplotrender.h"


static constexpr size_t BATCH_SIZE = 5000;
static constexpr size_t TARGET_CHUNK = 1000;

QPlotRender::QPlotRender(QCustomPlot* plot) : plot(plot), timeSum(0), valueMax(0)  {
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &QPlotRender::updatePlot);
    animationData = new AnimationDataContainer<double>(1000, 100);

}

QPlotRender::~QPlotRender() {

}

void QPlotRender::queueRendering(MtcPointMap* point_map, const QVector<double>& times, const uint64_t length,
                                 QCPGraph* graph) {

    const uint64_t sampleRate = 50;

    values.clear();

    values.reserve(length / sampleRate);


    uint64_t c = 0;
    for (uint64_t i = 0; i < point_map->length; i++) {
        if (point_map->points[i].value > valueMax) {
            valueMax = point_map->points[i].value;
        }
        for (uint64_t j = 0; j < point_map->points[i].repeated + 1; j++) {
            if (c  == sampleRate) {
                values.push_back(point_map->points[i].value);
                c = 0;
                continue;
            }
            c++;
        }
    }


    graph->setData(times, values);
    plot->xAxis->setRange(0, times.last() / sampleRate, Qt::AlignLeft);
    plot->yAxis->setRange(0, valueMax * 1.1);
    plot->replot(QCustomPlot::rpQueuedReplot);

}

void QPlotRender::processBatch(MtcPointMap* points, size_t start_index, size_t end_index, uint64_t sample_rate) {
    for (size_t i = start_index; i < end_index; i += sample_rate) {
        MtcPoint* point = &points->points[i];

        timeSum += *point->time_offset;
        times.push_back(static_cast<double>(timeSum));
        if (point->value > valueMax) {
            valueMax = point->value;
        }
        values.push_back(point->value);
    }
}


void QPlotRender::queueAnimationRendering(MtcPointMap* point_map, MtcTime* times, uint64_t length, uint64_t timesLength, QCPGraph* graph, int timeSpacing) {
    int64_t sampleRate = 50;
    values.clear();
    this->times.clear();

    // Reserve space for the data
    values.reserve(length / sampleRate);
    this->times.reserve(length / sampleRate);
    // Process time points - these will remain fixed
    uint64_t timeSum = 0;
    uint64_t c = 0;
    for (uint64_t i = 0; i < timesLength; i++) {
        for (uint64_t j = 0; j < times[i].repeated+1; j++) {
            timeSum += *times[i].time_offset;
            if (c == sampleRate) {
                this->times.push_back(timeSum);
                c = 0;
                continue;
            }
            c++;
        }
    }

    // Process values
    c = 0;
    for (uint64_t i = 0; i < point_map->length; i++) {
        if (point_map->points[i].value > valueMax) {
            valueMax = point_map->points[i].value;
        }
        for (uint64_t j = 0; j < point_map->points[i].repeated + 1; j++) {
            if (c == sampleRate) {
                values.push_back(point_map->points[i].value);
                c = 0;
                continue;
            }
            c++;
        }
    }

    animationData->setData(&values);
    animationData->setTimes(&this->times);

    // int numPoints = 1000;
    // double xMin = 0, xMax = 100;
    // double step = (xMax - xMin) / (numPoints - 1);
    //
    // for (int i = 0; i < numPoints; ++i) {
    //     double x = xMin + (i % 500) * step;
    //     double y = std::sin(x);
    //     values.push_back(y);
    //     this->times.push_back(x);
    // }
    currentGraph = graph;
    // graph->setData(this->times, values);
    plot->setOpenGl(false);
    plot->xAxis->setRange(0, this->times[1000]);
    plot->yAxis->setRange(0, valueMax * 1.1);
    // plot->replot(QCustomPlot::rpQueuedReplot);
    animationTimer->start(250);
}


void QPlotRender::updatePlot() {

    auto [times, data] = animationData->getWindow();
    if (data->size() == 0) return;
    currentGraph->setData(*times, *data);
    plot->replot(QCustomPlot::rpImmediateRefresh);
}

inline uint64_t QPlotRender::getSampleRate(const size_t length) {
    return std::max(1, static_cast<int>(length / TARGET_CHUNK));
}