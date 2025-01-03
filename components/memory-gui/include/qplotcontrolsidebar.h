
#ifndef QPLOTCONTROLSIDEBAR_H
#define QPLOTCONTROLSIDEBAR_H
#include <QWidget>
#include <qtreewidget.h>
#include "plotdata.h"


class QPlotControlSidebar : public QWidget {
    Q_OBJECT

public:
    explicit QPlotControlSidebar(QWidget* parent = nullptr);

    void setPlotCategories(const std::vector<PlotCategory>& plotCategories);

signals:
     void plotCategoriesChanged(const QString& category, const QString& plot, bool enabled);

private:
    QTreeWidget* treeWidget;

    QTreeWidgetItem* createCategoryItem(const PlotCategory& plotCategory);

};


#endif //QPLOTCONTROLSIDEBAR_H
