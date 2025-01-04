


#include "qmemoryplotter.h"

#include <mtc-config.h>


static constexpr size_t BATCH_SIZE = 5000;
static constexpr size_t TARGET_CHUNK = 1000;

QMemoryPlotter::QMemoryPlotter(QCustomPlot* plot) : _plot(plot) {
    _plot->setOpenGl(true);
    _plot->setNoAntialiasingOnDrag(true);
    _plot->setPlottingHints(QCP::phFastPolylines);
    _plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);


    _timer.setSingleShot(true);
    _timer.setInterval(100);

    connect(&_timer, &QTimer::timeout, this, &QMemoryPlotter::updatePlot);

    plotsEnabled.clear();
}

void QMemoryPlotter::togglePlot(const QString& category, const QString& plotString, bool enabled,
                                                        std::shared_ptr<mtc::MtcObject> object) {

    auto value = mtc::MTC_KEY_MAPPING.find(plotString.toStdString());
    if (value == mtc::MTC_KEY_MAPPING.end()) {
        qDebug() << "Unable to find plot name: " << plotString;
        return;
    }

    uint8_t key = value->second;
    if (enabled) {
        plotsEnabled[key] = plotString;
        plotData(object->get_points(key), object->get_length());
    }else {
        plotsEnabled.erase(key);
        updatePlots(object);
    }
}

void QMemoryPlotter::clearPlots() {
    _plot->clearGraphs();
    _plot->replot();
    plotsEnabled.clear();
}


void QMemoryPlotter::plotData(const std::vector<mtc::MtcPoint*>* points, const uint64_t length) {
    double timeSum = 0;

    const uint64_t skipFactor = getSkipFactor(length);

    times.clear();
    values.clear();

    times.reserve(length / skipFactor);
    values.reserve(length / skipFactor);


    for (size_t i = 0; i < length; i += BATCH_SIZE) {
        const size_t end = std::min(i + BATCH_SIZE, length);
        processBatch(points, i, skipFactor, end, timeSum);

        if (i + BATCH_SIZE < length) {
            QApplication::processEvents();
            updatePlot();
        }

    }
    finalisePlot();
}

void QMemoryPlotter::processBatch(const std::vector<mtc::MtcPoint*>* points, const size_t start,
                            const uint64_t skipFactor, const size_t end, double& timeSum) {
    for (size_t i = start; i < end; i += skipFactor) {
        const mtc::MtcPoint* point = (*points)[i];

        timeSum += *point->time_offset;
        times.push_back(timeSum);
        if (point->value > valueMax) {
            valueMax = point->value;
        }
        values.push_back(point->value);
    }
}

void QMemoryPlotter::requestUpdate() {
    if (!_timer.isActive()) {
        _timer.start();
    }
}

void QMemoryPlotter::updatePlot() {
    try {
        if (!_plot->graph(0)) {
            _plot->addGraph();
        }
        _plot->graph(0)->setAdaptiveSampling(true);
        _plot->graph(0)->setScatterStyle(QCPScatterStyle::ssNone);

        _plot->graph(0)->setData(times, values);
        _plot->xAxis->setRange(0, times.last());
        _plot->yAxis->setRange(0, valueMax * 1.1);
        _plot->replot(QCustomPlot::rpQueuedReplot);
    } catch (const std::exception& e) {
        qCritical() << "QMemoryPlotter::updatePlot(): " << e.what();
    }
}

void QMemoryPlotter::finalisePlot() {
    updatePlot();
    _plot->replot();
}



void QMemoryPlotter::updatePlots(const std::shared_ptr<mtc::MtcObject>& object) {
    _plot->clearGraphs();
    if (plotsEnabled.empty()) {
        _plot->replot();
        return;
    }

    for (auto& [key, _] : plotsEnabled) {
        plotData(object->get_points(key), object->get_length());
    }
}

inline uint64_t QMemoryPlotter::getSkipFactor(const size_t length) {
    return std::max(1, static_cast<int>(length / TARGET_CHUNK));
}
