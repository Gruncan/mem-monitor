
#ifndef PLOTDATA_H
#define PLOTDATA_H
#include <QList>
#include <QString>


struct PlotData {
    QString name;
    bool enabled = false;
    QVector<double> x;
    QVector<double> y;
};

struct PlotCategory {
    QString name;
    std::vector<PlotData> data;
};

#endif // PLOTDATA_H
