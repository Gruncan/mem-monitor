


#include "qmemoryplotter.h"
#include "qplotrender.h"


static constexpr size_t BATCH_SIZE = 5000;
static constexpr size_t TARGET_CHUNK = 1000;

QPlotRender::QPlotRender(QCustomPlot* plot) : plot(plot), timeSum(0), valueMax(0)  {

}

QPlotRender::~QPlotRender() {

}

void QPlotRender::queueRendering(MtcPointMap* point_map, const QVector<double>& times, const uint64_t length,
                                 QCPGraph* graph) {

    // valueMax = 0;
    const uint64_t sampleRate = 50;

    // times.clear();
    values.clear();

    // times.reserve(length / sampleRate);
    values.reserve(length / sampleRate);

    // for (size_t start = 0; start < length; start += BATCH_SIZE) {
    //     const size_t end = std::min(start + BATCH_SIZE, length);std::vector<double> valuesStd;
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


inline uint64_t QPlotRender::getSampleRate(const size_t length) {
    return std::max(1, static_cast<int>(length / TARGET_CHUNK));
}