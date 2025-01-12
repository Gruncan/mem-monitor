
#ifndef QMEMORYPLOTTER_H
#define QMEMORYPLOTTER_H

#include "mtccdecoder.h"
#include "qcustomplot.h"
#include "qmemoryanimatecontrols.h"
#include "qmtcloader.h"
#include "qplotrender.h"


class QMemoryPlotter : public QWidget {
    Q_OBJECT

public:

    explicit QMemoryPlotter(QWidget* parent, QCustomPlot* plot, QMtcLoader* loader, QMemoryAnimateControls* animateControls);

    void addPlot(mk_size_t key);

    void removePlot(mk_size_t key);

    void setupSpinBox(QDoubleSpinBox* spinBox);

    void updateInputsFromPlot();

    void setSpinBoxRanges(double xMin, double xMax, double yMin, double yMax);

    void setIsLoaded(bool isLoaded);

public Q_SLOTS:

    void plotToggleChange(const QString& category, const QString& plotString, bool enabled);

    void updateXRange();

    void updateYRange();

    void playClicked();
    void pauseClicked();
    void rewindClicked();
    void forwardClicked();
    void onTimeSpacingUpdate(int timeSpacing);


Q_SIGNALS:
    void queueRendering(MtcPointMap* points, const QVector<double>& times, uint64_t length, QCPGraph* graph);

    void queueAnimationRendering(MtcPointMap* point_map, MtcTime* times, uint64_t length, uint64_t timesLength, QCPGraph* graph, int timeSpacing);

    void startAnimation();
    void stopAnimation();

private:

    QCustomPlot* _plot;

    std::map<mk_size_t, QCPGraph*> plotsEnabled;

    QMtcLoader* _loader;

    QPlotRender* _plotRender;

    QThread* renderThread;
    std::mt19937 gen;
    std::vector<QColor> usedColors;

    QColor generateRandomColor();

    QDoubleSpinBox* xMinInput;
    QDoubleSpinBox* xMaxInput;
    QDoubleSpinBox* yMinInput;
    QDoubleSpinBox* yMaxInput;

    QMemoryAnimateControls* animateControls;

    bool isAnimationRunning;
    bool isLoaded;

    int timeSpacing;
    bool hasPlayed;

};


#endif //QMEMORYPLOTTER_H
