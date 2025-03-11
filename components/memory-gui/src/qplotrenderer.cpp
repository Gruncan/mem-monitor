


#include "animation-data-container.h"
#include "qmemoryplotter.h"
#include "qplotrender.h"


static constexpr size_t BATCH_SIZE = 5000;
static constexpr size_t TARGET_CHUNK = 1000;

QPlotRender::QPlotRender(QCustomPlot* plot) : plot(plot), timeSum(0), valueMax(0) {
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &QPlotRender::updatePlot);
    timeSpacing = 1000;
    animationData = new AnimationDataContainer<double>(timeSpacing, timeSpacing / 10);
}

QPlotRender::~QPlotRender() {
}

void QPlotRender::queueRendering(MtcPointMap* point_map, const QVector<double>& times, const uint64_t length,
                                 QCPGraph* graph) {

    values.clear();
    valueMax = 0;


#ifndef SAMPLE_RATE
    values.reserve(length);

    for (uint64_t i = 0; i < point_map->length; i++) {
        if (point_map->points[i].value > valueMax) {
            valueMax = point_map->points[i].value;
        }
        for (uint64_t j = 0; j < point_map->points[i].repeated + 1; j++) {
            values.push_back(point_map->points[i].value);
        }
    }
    // values.resize(values.size() - 400);
#else
    static constexpr uint64_t sampleRate = SAMPLE_RATE;
    values.reserve(length / sampleRate);

    uint64_t c = 0;
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
#endif

    graph->setData(times, values);
    plot->xAxis->setRange(0, times.last() / 8, Qt::AlignLeft);
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


void QPlotRender::queueAnimationRendering(MtcPointMap* point_map, MtcTime* times, uint64_t length, uint64_t timesLength,
                                          QCPGraph* graph, int timeSpacing) {
    int64_t sampleRate = 50;
    values.clear();
    this->times.clear();

    values.reserve(length / sampleRate);
    this->times.reserve(length / sampleRate);
    uint64_t timeSum = 0;
    uint64_t c = 0;
    for (uint64_t i = 0; i < timesLength; i++) {
        for (uint64_t j = 0; j < times[i].repeated + 1; j++) {
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
    this->timeSpacing = timeSpacing;

    currentGraph = graph;
    plot->setOpenGl(false);
    plot->xAxis->setRange(0, this->times[this->timeSpacing]);
    plot->yAxis->setRange(0, valueMax * 1.1);
    // plot->replot(QCustomPlot::rpQueuedReplot);
    animationTimer->start(250);
}


void QPlotRender::startAnimation() {
    plot->xAxis->setRange(0, this->times[this->timeSpacing]);
    plot->yAxis->setRange(0, valueMax * 1.1);
    animationTimer->start(250);
}

void QPlotRender::stopAnimation() {
    animationTimer->stop();
}

void QPlotRender::rewind() {
    animationData->rewind();
    auto [times, data] = animationData->getWindow();
    currentGraph->setData(*times, *data);
    plot->replot(QCustomPlot::rpImmediateRefresh);
}

void QPlotRender::forward() {
    animationData->forward();
    auto [times, data] = animationData->getWindow();
    currentGraph->setData(*times, *data);
    plot->replot(QCustomPlot::rpImmediateRefresh);
}

void QPlotRender::setTimeSpacing(int spacing) {
    animationTimer->stop();
    this->timeSpacing = spacing;
    animationData->restart(spacing);
}

void QPlotRender::updatePlot() {
    auto [times, data] = animationData->getWindow();
    if (data->size() == 0)
        return;
    currentGraph->setData(*times, *data);
    // auto [lower, upper] = animationData->getTimeRange();
    // QCPRange range = QCPRange(lower, upper);
    // qDebug() << range.lower << " " << range.upper;
    // plot->xAxis->mTicker->generate(range, plot->xAxis->mParentPlot->locale(), plot->xAxis->mNumberFormatChar,
    // plot->xAxis->mNumberPrecision, plot->xAxis->mTickVector, plot->xAxis->mSubTicks ? &plot->xAxis->mSubTickVector :
    // nullptr, plot->xAxis->mTickLabels ? &plot->xAxis->mTickVectorLabels : nullptr); plot->xAxis->setRange()
    plot->replot(QCustomPlot::rpImmediateRefresh);
}

inline uint64_t QPlotRender::getSampleRate(const size_t length) {
    return std::max(1, static_cast<int>(length / TARGET_CHUNK));
}