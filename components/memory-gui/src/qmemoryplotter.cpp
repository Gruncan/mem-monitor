


#include "qmemoryplotter.h"

#include "qmemoryanimatecontrols.h"

#include <QDoubleSpinBox>
#include <QLabel>
#include <mtc-config.h>
#include <random>


static constexpr int QUALITY = 1000;

QMemoryPlotter::QMemoryPlotter(QWidget* parent, QCustomPlot* plot, QMtcLoader* loader,
                               QMemoryAnimateControls* animateControls)
    : QWidget(parent), _plot(plot), _loader(loader), gen(1), animateControls(animateControls),
      isAnimationRunning(false), isLoaded(false) {
    _plotRender = new QPlotRender(_plot);

    _plot->setOpenGl(true);
    _plot->setNoAntialiasingOnDrag(true);
    _plot->setPlottingHints(QCP::phFastPolylines);
    _plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    renderThread = new QThread;
    _plotRender->moveToThread(renderThread);

    connect(this, &QMemoryPlotter::queueRendering, _plotRender, &QPlotRender::queueRendering);
    connect(this, &QMemoryPlotter::queueAnimationRendering, _plotRender, &QPlotRender::queueAnimationRendering);
    connect(this, &QMemoryPlotter::startAnimation, _plotRender, &QPlotRender::startAnimation);
    connect(this, &QMemoryPlotter::stopAnimation, _plotRender, &QPlotRender::stopAnimation);

    renderThread->start();
    plotsEnabled.clear();

    QWidget* controlsWidget = new QWidget(parent);
    controlsWidget->setGeometry(QRect(200, 520, 100, 150));

    // Create layout for the container
    QVBoxLayout* controlsLayout = new QVBoxLayout(controlsWidget);

    // Add controls to the layout
    QLabel* xRangeLabel = new QLabel("X Range:", controlsWidget);
    xMinInput = new QDoubleSpinBox(controlsWidget);
    xMaxInput = new QDoubleSpinBox(controlsWidget);

    QLabel* yRangeLabel = new QLabel("Y Range:", controlsWidget);
    yMinInput = new QDoubleSpinBox(controlsWidget);
    yMaxInput = new QDoubleSpinBox(controlsWidget);

    controlsLayout->addWidget(xRangeLabel);
    controlsLayout->addWidget(xMinInput);
    controlsLayout->addWidget(xMaxInput);
    controlsLayout->addWidget(yRangeLabel);
    controlsLayout->addWidget(yMinInput);
    controlsLayout->addWidget(yMaxInput);

    setupSpinBox(xMaxInput);
    setupSpinBox(xMinInput);
    setupSpinBox(yMaxInput);
    setupSpinBox(yMinInput);

    connect(xMinInput, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &QMemoryPlotter::updateXRange);
    connect(xMaxInput, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &QMemoryPlotter::updateXRange);
    connect(yMinInput, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &QMemoryPlotter::updateYRange);
    connect(yMaxInput, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &QMemoryPlotter::updateYRange);


    connect(animateControls, &QMemoryAnimateControls::playClicked, this, &QMemoryPlotter::playClicked);
    connect(animateControls, &QMemoryAnimateControls::pauseClicked, this, &QMemoryPlotter::pauseClicked);
    connect(animateControls, &QMemoryAnimateControls::rewindClicked, this, &QMemoryPlotter::rewindClicked);
    connect(animateControls, &QMemoryAnimateControls::forwardClicked, this, &QMemoryPlotter::forwardClicked);
    connect(animateControls, &QMemoryAnimateControls::timeSpacingChange, this, &QMemoryPlotter::onTimeSpacingUpdate);
    connect(animateControls->exportButton, &QPushButton::clicked, this, &QMemoryPlotter::exportPlot);
    hasPlayed = false;
    timeSpacing = 1000;
    updateInputsFromPlot();
}

void QMemoryPlotter::addPlot(mk_size_t key) {
    QCPGraph* graph = _plot->addGraph();
    plotsEnabled[key] = graph;
    if (_plot->graphCount() == 1) {
        _plot->legend->setVisible(true);
        _plot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
        _plot->legend->setBorderPen(QPen(Qt::black));
    }

    const MtcObject* object = _loader->getMtcObject();
    uint64_t sampleRate = 50;

    QVector<double> times;
    uint64_t timeSum = 0;
    times.reserve(object->size / sampleRate);
    uint64_t c = 0;
    for (uint64_t i = 0; i < object->_times_length; i++) {
        for (uint64_t j = 0; j < object->times[i].repeated; j++) {
            timeSum += *object->times[i].time_offset;
            if (c == sampleRate) {
                times.push_back(timeSum);
                c = 0;
                continue;
            }
            c++;
        }
    }
    // _plot->xAxis->setRange(0, times.last());
    // _plot->yAxis->setRange(0, valueMax * 1.1);
    // Move to another thread
    // std::shared_ptr<mtc::MtcObject> object = _loader->getMtcData();
    graph->setPen(QPen(generateRandomColor()));
    graph->setName(QString::fromStdString(mtc::MTC_INDEX_MAPPING.at(key)));

    emit queueRendering(&object->point_map[key], times, object->size, graph);
}

void QMemoryPlotter::removePlot(mk_size_t key) {
    if (_plot->graphCount() == 1) {
        _plot->legend->setVisible(false);
    }
    _plot->removeGraph(plotsEnabled[key]);
    plotsEnabled.erase(key);
    if (plotsEnabled.empty()) {
        hasPlayed = false;
    }
    // Maybe we should tell reender about this
    _plot->replot();
}

void QMemoryPlotter::clearAllPlots() {

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
    } else {
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

void QMemoryPlotter::setSpinBoxRanges(double xMin, double xMax, double yMin, double yMax) {
    xMinInput->setRange(xMin, xMax);
    xMaxInput->setRange(xMin, xMax);
    yMinInput->setRange(yMin, yMax);
    yMaxInput->setRange(yMin, yMax);
}


void QMemoryPlotter::setupSpinBox(QDoubleSpinBox* spinBox) {
    spinBox->setDecimals(2);
    spinBox->setRange(0, 90000000000);
    spinBox->setSingleStep(1.0);
}

void QMemoryPlotter::updateInputsFromPlot() {
    QCPRange xRange = _plot->xAxis->range();
    QCPRange yRange = _plot->yAxis->range();

    xMinInput->setValue(xRange.lower);
    xMaxInput->setValue(xRange.upper);
    yMinInput->setValue(yRange.lower);
    yMaxInput->setValue(yRange.upper);
}

void QMemoryPlotter::updateXRange() {
    double min = xMinInput->value();
    double max = xMaxInput->value();
    if (min < max) {
        _plot->xAxis->setRange(min, max);
        _plot->replot();
    }
}

void QMemoryPlotter::updateYRange() {
    double min = yMinInput->value();
    double max = yMaxInput->value();
    if (min < max) {
        _plot->yAxis->setRange(min, max);
        _plot->replot();
    }
}

void QMemoryPlotter::playClicked() {
    if (!isLoaded || isAnimationRunning || plotsEnabled.empty())
        return;

    if (_plot->graphCount() != 1) {
        return;
    }

    _plot->xAxis->setRange(0, timeSpacing);
    const mk_size_t key = plotsEnabled.begin()->first;
    const MtcObject* object = _loader->getMtcObject();
    if (!hasPlayed) {
        hasPlayed = true;
        emit queueAnimationRendering(&object->point_map[key], object->times, object->size, object->_times_length,
                                     plotsEnabled[key], this->timeSpacing);
    } else {
        emit startAnimation();
    }
}

void QMemoryPlotter::pauseClicked() {
    emit stopAnimation();
}

void QMemoryPlotter::rewindClicked() {
    _plotRender->rewind();
}

void QMemoryPlotter::forwardClicked() {
    _plotRender->forward();
}

void QMemoryPlotter::onTimeSpacingUpdate(int timeSpacing) {
    this->timeSpacing = timeSpacing;
    _plotRender->setTimeSpacing(timeSpacing);
    hasPlayed = false;
    animateControls->onPlayPauseClicked();
}

void QMemoryPlotter::setIsLoaded(const bool isLoaded) {
    this->isLoaded = isLoaded;
}

void QMemoryPlotter::exportPlot() {
    QString filename = "plot.png";
    if (_plot->savePng(filename, 1061, 511)) {
        qDebug() << "Plot saved to" << filename;
    } else {
        qDebug() << "Failed to save plot.";
    }
}