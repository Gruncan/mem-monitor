


#include "qmemoryplotter.h"

#include <mtc-config.h>


QMemoryPlotter::QMemoryPlotter(QCustomPlot* plot) : _plot(plot) {
    _plot->setOpenGl(true);
    _plot->setNoAntialiasingOnDrag(true);
    _plot->setPlottingHints(QCP::phFastPolylines);


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

    uint32_t key = value->second;
    if (enabled) {
        plotsEnabled[key] = plotString;
        plotData(object->get_points(), key);
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


void QMemoryPlotter::plotData(const std::vector<mtc::MtcPoint>& points, uint32_t key) {
    static constexpr size_t batchSize = 5000;
    double timeSum = 0;

    const uint64_t skipFactor = getSkipFactor(points.size());

    times.clear();
    values.clear();

    times.reserve(points.size() / skipFactor);
    values.reserve(points.size() / skipFactor);


    for (size_t i = 0; i < points.size(); i += batchSize) {
        const size_t end = std::min(i + batchSize, points.size());
        processBatch(points, key, i, skipFactor, end, timeSum);

        if (i + batchSize < points.size()) {
            QApplication::processEvents();
            updatePlot();
        }

    }
    finalisePlot();
}

void QMemoryPlotter::processBatch(const std::vector<mtc::MtcPoint>& points, const uint64_t key, const size_t start,
                            const uint64_t skipFactor, const size_t end, double& timeSum) {
    for (size_t i = start; i < end; i += skipFactor) {
        auto [time_offset, data] = points[i];
        timeSum += time_offset;
        times.push_back(timeSum);
        if (data.at(key) > valueMax) {
            valueMax = data.at(key);
        }
        values.push_back(data.at(key));
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



void QMemoryPlotter::updatePlots(std::shared_ptr<mtc::MtcObject> object) {
    _plot->clearGraphs();
    if (plotsEnabled.empty()) {
        _plot->replot();
        return;
    }

    const auto& points = object->get_points();
    for (auto& [key, _] : plotsEnabled) {
        plotData(points, key);
    }
}

uint64_t QMemoryPlotter::getSkipFactor(const size_t length) {
    const static size_t target = 1000;
    return std::max(1, static_cast<int>(length / target));
}
