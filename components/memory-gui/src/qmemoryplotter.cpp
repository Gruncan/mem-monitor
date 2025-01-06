


#include "qmemoryplotter.h"
#include <random>
#include <mtc-config.h>


static constexpr int QUALITY = 1000;

QMemoryPlotter::QMemoryPlotter(QCustomPlot* plot, QMtcLoader* loader) : _plot(plot), _loader(loader), gen(1)  {

    _plotRender = new QPlotRender(_plot);

    _plot->setOpenGl(true);
    _plot->setNoAntialiasingOnDrag(true);
    _plot->setPlottingHints(QCP::phFastPolylines);
    _plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    renderThread = new QThread;
    _plotRender->moveToThread(renderThread);

    connect(this, &QMemoryPlotter::queueRendering, _plotRender, &QPlotRender::queueRendering);

    renderThread->start();
    plotsEnabled.clear();
}

void QMemoryPlotter::addPlot(mk_size_t key) {
    QCPGraph* graph = _plot->addGraph();
    plotsEnabled[key] = graph;
    if (_plot->graphCount() == 1) {
        _plot->legend->setVisible(true);
        _plot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
        _plot->legend->setBorderPen(QPen(Qt::black));
    }

    // _plot->xAxis->setRange(0, times.last());
    // _plot->yAxis->setRange(0, valueMax * 1.1);
    // Move to another thread
    std::shared_ptr<mtc::MtcObject> object = _loader->getMtcData();
    graph->setPen(QPen(generateRandomColor()));
    graph->setName(QString::fromStdString(mtc::MTC_INDEX_MAPPING.at(key)));

    emit queueRendering(object->get_points(key), object->get_length(), graph);
}

void QMemoryPlotter::removePlot(mk_size_t key) {
    if (_plot->graphCount() == 1) {
        _plot->legend->setVisible(false);
    }
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

    const mk_size_t key = value->second;

    if (enabled) {
        addPlot(key);
    }else {
        removePlot(key);
    }
}

QColor QMemoryPlotter::generateRandomColor() {
    std::uniform_int_distribution<> dis(30, 225);
    QColor color;
    do {
        color = QColor(dis(gen), dis(gen), dis(gen));
    } while (usedColors.end() != std::find(usedColors.begin(), usedColors.end(), color));
    usedColors.push_back(color);
    return color;
}
