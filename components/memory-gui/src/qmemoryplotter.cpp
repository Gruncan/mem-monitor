


#include "qmemoryplotter.h"

#include <mtc-config.h>



QMemoryPlotter::QMemoryPlotter(QCustomPlot* plot, QMtcLoader* loader) : _plot(plot), _loader(loader)  {

    _plotRender = new QPlotRender(_plot);

    _plot->setOpenGl(true);
    _plot->setNoAntialiasingOnDrag(true);
    _plot->setPlottingHints(QCP::phFastPolylines);
    _plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);


    plotsEnabled.clear();
}

void QMemoryPlotter::addPlot(uint8_t key) {
    QCPGraph* graph = _plot->addGraph();
    plotsEnabled[key] = graph;

    // _plot->xAxis->setRange(0, times.last());
    // _plot->yAxis->setRange(0, valueMax * 1.1);
    // Move to another thread
    auto object = _loader->getMtcData();
    _plotRender->queueRendering(object->get_points(key), object->get_length(), graph);
}

void QMemoryPlotter::removePlot(uint8_t key) {
    _plot->removeGraph(plotsEnabled[key]);
    plotsEnabled.erase(key);
    // Maybe we should tell reender about this
    _plot->replot();

}


void QMemoryPlotter::plotToggleChange(const QString& category, const QString& plotString, const bool enabled) {
    const auto value = mtc::MTC_KEY_MAPPING.find(plotString.toStdString());
    if (value == mtc::MTC_KEY_MAPPING.end()) {
        qDebug() << "Unable to find plot name: " << plotString;
        return;
    }

    const uint8_t key = value->second;

    if (enabled) {
        addPlot(key);
    }else {
        removePlot(key);
    }
}
