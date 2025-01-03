

#include "qplotcontrolsidebar.h"

#include <qboxlayout.h>


QPlotControlSidebar::QPlotControlSidebar(QWidget* parent) : QWidget(parent){
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderHidden(true);
    treeWidget->setMinimumWidth(200);
    layout->addWidget(treeWidget);

    connect(treeWidget, &QTreeWidget::itemChanged, this, [this](QTreeWidgetItem* item) {
        if (item->parent()) {
            emit plotCategoriesChanged(item->parent()->text(0), item->text(0),
                item->checkState(0) == Qt::Checked);

        }
    });
}

void QPlotControlSidebar::setPlotCategories(const std::vector<PlotCategory>& plotCategories) {
    treeWidget->clear();

    for (auto& plotCategory : plotCategories) {
        const auto item = createCategoryItem(plotCategory);
        treeWidget->addTopLevelItem(item);
        item->setExpanded(true);
    }
}


QTreeWidgetItem* QPlotControlSidebar::createCategoryItem(const PlotCategory& plotCategory) {
    auto item = new QTreeWidgetItem();
    item->setText(0, plotCategory.name);
    item->setFlags(item->flags() | Qt::ItemIsUserTristate | Qt::ItemIsUserCheckable);

    for (auto plot : plotCategory.data) {
        auto plotItem = new QTreeWidgetItem();
        plotItem->setText(0, plot.name);
        plotItem->setCheckState(0, plot.enabled ? Qt::Checked : Qt::Unchecked);
        item->addChild(plotItem);
    }

    return item;
}