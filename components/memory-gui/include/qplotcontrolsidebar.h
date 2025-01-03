
#ifndef QPLOTCONTROLSIDEBAR_H
#define QPLOTCONTROLSIDEBAR_H
#include "mtc-config.h"


#include "plotdata.h"
#include <qtreewidget.h>


class QPlotControlSidebar : public QWidget {
    Q_OBJECT

public:
    explicit QPlotControlSidebar(QWidget* parent = nullptr);

    void setCategories(const std::vector<mtc::MtcCategories>& categories);

signals:
     void categoriesChanged(const QString& category, const QString& plot, bool enabled);

private:
    QTreeWidget* treeWidget;

    QTreeWidgetItem* createCategoryItem(const mtc::MtcCategories& categories);

};


#endif //QPLOTCONTROLSIDEBAR_H
