
#ifndef QMEMORYPLOTTER_H
#define QMEMORYPLOTTER_H

#include "qcustomplot.h"
#include "qmtcloader.h"
#include "qplotrender.h"
#include "mtccdecoder.h"


class QMemoryPlotter : public QWidget {
    Q_OBJECT

public:

    explicit QMemoryPlotter(QWidget* parent, QCustomPlot* plot, QMtcLoader* loader);

    void addPlot(mk_size_t key);

    void removePlot(mk_size_t key);

    void setupSpinBox(QDoubleSpinBox* spinBox);

    void updateInputsFromPlot();

    void setSpinBoxRanges(double xMin, double xMax, double yMin, double yMax);

public Q_SLOTS:

    void plotToggleChange(const QString& category, const QString& plotString, bool enabled);

    void updateXRange();

    void updateYRange();


Q_SIGNALS:
    void queueRendering(MtcPointMap* points, const QVector<double>& times, uint64_t length, QCPGraph* graph);


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

};


#endif //QMEMORYPLOTTER_H
