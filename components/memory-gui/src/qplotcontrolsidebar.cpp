

#include "qplotcontrolsidebar.h"
#include "mainwindow.h"
#include "qtreememorywidgetitem.h"

#include <qboxlayout.h>


QPlotControlSidebar::QPlotControlSidebar(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 10);

    treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderHidden(true);
    treeWidget->setMinimumWidth(200);
    treeWidget->setContentsMargins(0, 0, 0, 10);
    layout->addWidget(treeWidget);

    connect(treeWidget, &QTreeWidget::itemChanged, this, [this](QTreeWidgetItem* item) {
        if (item->parent()) {
            emit categoriesChanged(item->parent()->text(0), item->text(0), item->checkState(0) == Qt::Checked);
        }
    });
    connect(this, &QPlotControlSidebar::initialisePlot, reinterpret_cast<MainWindow*>(parent),
            &MainWindow::initialisePlot);
}

void QPlotControlSidebar::setCategories(const std::vector<mtc::MtcCategories>& categories) {
    treeWidget->clear();
    for (auto& category : categories) {
        const auto item = createCategoryItem(category);
        treeWidget->addTopLevelItem(item);
        if (category.name == "General") {
            item->setExpanded(true);
        } else {
            item->setExpanded(false);
        }
        item->setDisabled(true);
    }
}


QTreeWidgetItem* QPlotControlSidebar::createCategoryItem(const mtc::MtcCategories& categories) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, QString::fromStdString(categories.name));
    item->setFlags(item->flags() | Qt::ItemIsUserTristate | Qt::ItemIsUserCheckable);

    for (const uint32_t id : categories.ids) {
        const auto plotItem = new QTreeMemoryWidgetItem(id);
        auto value = mtc::MTC_INDEX_MAPPING.find(id);
        if (value == mtc::MTC_INDEX_MAPPING.end()) {
            qDebug() << "id:" << id << "is not mapped";
            continue;
        }
        std::string name = value->second;
        plotItem->setText(0, QString::fromStdString(name));
        plotItem->setCheckState(0, Qt::Unchecked);
        plotItem->setDisabled(true);
        item->addChild(plotItem);
    }

    return item;
}

void QPlotControlSidebar::enableNonDefaultFields(const std::map<mk_size_t, bool>* defaultFields) {
    for (uint16_t i = 0; i < treeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem* item = treeWidget->topLevelItem(i);
        for (uint16_t j = 0; j < item->childCount(); j++) {
            QTreeMemoryWidgetItem* child = reinterpret_cast<QTreeMemoryWidgetItem*>(item->child(j));
            if (!defaultFields->at(child->get_key())) {
                item->setDisabled(false);
                child->setDisabled(false);
            }
        }
    }
    emit initialisePlot();
}