

#include "qplotcontrolsidebar.h"

#include <qboxlayout.h>


QPlotControlSidebar::QPlotControlSidebar(QWidget* parent) : QWidget(parent){
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 10);

    treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderHidden(true);
    treeWidget->setMinimumWidth(200);
    treeWidget->setContentsMargins(0, 0, 0, 10);
    layout->addWidget(treeWidget);

    connect(treeWidget, &QTreeWidget::itemChanged, this, [this](QTreeWidgetItem* item) {
        if (item->parent()) {
            emit categoriesChanged(item->parent()->text(0), item->text(0),
                item->checkState(0) == Qt::Checked);

        }
    });
}

void QPlotControlSidebar::setCategories(const std::vector<mtc::MtcCategories>& categories) {
    treeWidget->clear();

    for (auto& category : categories) {
        const auto item = createCategoryItem(category);
        treeWidget->addTopLevelItem(item);
        item->setExpanded(false);
    }
}


QTreeWidgetItem* QPlotControlSidebar::createCategoryItem(const mtc::MtcCategories& categories) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, QString::fromStdString(categories.name));
    item->setFlags(item->flags() | Qt::ItemIsUserTristate | Qt::ItemIsUserCheckable);

    for (const uint32_t id : categories.ids) {
        auto plotItem = new QTreeWidgetItem();
        auto value = mtc::MTC_INDEX_MAPPING.find(id);
        if (value == mtc::MTC_INDEX_MAPPING.end()) {
            qDebug() << "id:" << id << "is not mapped";
            continue;
        }
        std::string name = value->second;
        plotItem->setText(0, QString::fromStdString(name));
        plotItem->setCheckState(0, Qt::Unchecked);
        item->addChild(plotItem);
    }

    return item;
}