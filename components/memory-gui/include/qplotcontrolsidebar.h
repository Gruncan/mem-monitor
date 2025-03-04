
#ifndef QPLOTCONTROLSIDEBAR_H
#define QPLOTCONTROLSIDEBAR_H


#include "mtc-config.h"
#include <QTreeWidgetItem>

#include "plotdata.h"
#include "qtreememorywidgetitem.h"


class QPlotControlSidebar : public QWidget {
    Q_OBJECT

  public:
    explicit QPlotControlSidebar(QWidget* parent, std::map<mk_size_t, std::string>* nameMapping);

    void setCategories(const std::vector<mtc::MtcCategories>& categories);

    void setNameMapping(std::map<mk_size_t, std::string>* nameMapping);

  public Q_SLOTS:
    void enableNonDefaultFields(const std::map<mk_size_t, bool>* activeFields);

  Q_SIGNALS:
    void categoriesChanged(QTreeMemoryWidgetItem* item, bool enabled);
    void initialisePlot();

  private:
    QTreeWidget* treeWidget;
    bool initialised;
    std::map<mk_size_t, std::string>* _nameMapping;

    QTreeWidgetItem* createCategoryItem(const mtc::MtcCategories& categories);
};


#endif // QPLOTCONTROLSIDEBAR_H
