/***********************************
 *
 * @brief Plot control sidebar
 * @details Handles the UI interacts in the categories and propagates interactions into memory plotter
 *
 * @file qplotcontrolsidebar.h
 *
************************************/
#ifndef QPLOTCONTROLSIDEBAR_H
#define QPLOTCONTROLSIDEBAR_H


#include "mtc-config.h"
#include <QTreeWidgetItem>

#include "plotdata.h"


class QPlotControlSidebar : public QWidget {
    Q_OBJECT

  public:
    explicit QPlotControlSidebar(QWidget* parent);

    void setCategories(const std::vector<mtc::MtcCategories>& categories);

  public Q_SLOTS:
    void enableNonDefaultFields(const std::map<mk_size_t, bool>* activeFields);

  Q_SIGNALS:
    void categoriesChanged(const QString& category, const QString& plot, bool enabled);
    void initialisePlot();

  private:
    QTreeWidget* treeWidget;

    QTreeWidgetItem* createCategoryItem(const mtc::MtcCategories& categories);
};


#endif // QPLOTCONTROLSIDEBAR_H
