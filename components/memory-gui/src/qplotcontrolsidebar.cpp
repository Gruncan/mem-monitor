

#include "qplotcontrolsidebar.h"
#include "mainwindow.h"
#include "qtreememorywidgetitem.h"

#include <qboxlayout.h>



#define INIT_CONNECTIONS \
    connect(treeWidget, &QTreeWidget::itemChanged, this, [this](QTreeWidgetItem* item) {\
        if (item->parent()) {\
            emit categoriesChanged(reinterpret_cast<QTreeMemoryWidgetItem*>(item), item->checkState(0) == Qt::Checked);\
        }\
    });\
    connect(this, &QPlotControlSidebar::initialisePlot, reinterpret_cast<MainWindow*>(parent), &MainWindow::initialisePlot);\

// #define REM_CONNECTIONS \
//     disco

QPlotControlSidebar::QPlotControlSidebar(QWidget* parent, std::map<mk_size_t, std::string>* nameMapping=nullptr) : QWidget(parent),
            initialised(false){
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 10);

    if (nameMapping == nullptr) {
        _nameMapping = &mtc::MTC_INDEX_MAPPING;
    }


    treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderHidden(true);
    treeWidget->setMinimumWidth(200);
    treeWidget->setContentsMargins(0, 0, 0, 10);
    layout->addWidget(treeWidget);

    INIT_CONNECTIONS
}

void QPlotControlSidebar::setCategories(const std::vector<mtc::MtcCategories>& categories) {
    treeWidget->clear();
    for (int i=0; i<categories.size(); i++) {
        const auto& category = categories[i];
        const auto item = createCategoryItem(category);
        treeWidget->addTopLevelItem(item);
        if (i == 0) {
            item->setExpanded(true);
        } else {
            item->setExpanded(false);
        }
        item->setDisabled(true);
    }
    initialised = true;
}

void QPlotControlSidebar::setNameMapping(std::map<mk_size_t, std::string>* nameMapping) {
    _nameMapping = nameMapping;
}


QTreeWidgetItem* QPlotControlSidebar::createCategoryItem(const mtc::MtcCategories& categories) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, QString::fromStdString(categories.name));
    item->setFlags(item->flags() | Qt::ItemIsUserTristate | Qt::ItemIsUserCheckable);

    for (const uint32_t id : categories.ids) {
        auto value = _nameMapping->find(id);
        if (value == _nameMapping->end()) {
            qDebug() << "id:" << id << "is not mapped";
            continue;
        }
        std::string name = value->second;
        const auto plotItem = new QTreeMemoryWidgetItem(id, name);
        plotItem->setText(0, QString::fromStdString(name));
        plotItem->setCheckState(0, Qt::Unchecked);
        plotItem->setDisabled(true);
        item->addChild(plotItem);
    }

    return item;
}

void QPlotControlSidebar::enableNonDefaultFields(const std::map<mk_size_t, bool>* defaultFields) {
    if (defaultFields->size() == 0) {
        for (uint16_t i = 0; i < treeWidget->topLevelItemCount(); i++) {
            QTreeWidgetItem* item = treeWidget->topLevelItem(i);
            for (uint16_t j = 0; j < item->childCount(); j++) {
                QTreeMemoryWidgetItem* child = reinterpret_cast<QTreeMemoryWidgetItem*>(item->child(j));
                item->setDisabled(false);
                child->setDisabled(false);
            }
        }
    }else {
        for (uint16_t i = 0; i < treeWidget->topLevelItemCount(); i++) {
            QTreeWidgetItem* item = treeWidget->topLevelItem(i);
            for (uint16_t j = 0; j < item->childCount(); j++) {
                QTreeMemoryWidgetItem* child = reinterpret_cast<QTreeMemoryWidgetItem*>(item->child(j));
                if (defaultFields->contains(child->get_key())) {
                    if (!defaultFields->at(child->get_key())) {
                        item->setDisabled(false);
                        child->setDisabled(false);
                    }
                }
            }
        }
    }

    emit initialisePlot();
}