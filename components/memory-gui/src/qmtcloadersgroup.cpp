

#include "qmtcloadersgroup.h"

#include "qmtcloader.h"


#include <QProgressBar>
#include <QPushButton>
#include <qboxlayout.h>
#include <qlabel.h>


QMtcLoadersGroup::QMtcLoadersGroup(QWidget* parent, std::vector<QMtcLoader*> loaders)
    : QWidget(parent), mtcLoaders(loaders), selectedLoader(0) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFrame* frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);
    mainLayout->addWidget(frame);

    QHBoxLayout* layout = new QHBoxLayout(frame);
    uint8_t i = 0;
    for (const auto loader : loaders) {
        layout->addWidget(loader);
        connect(loader, &QMtcLoader::stateChanged, this, [this, i]() {
            selectedLoader = i;
            checkboxChanged(i);
        });
        i++;
    }

    setMinimumSize(700, 200);
}

QMtcLoadersGroup::~QMtcLoadersGroup() {
    for (const auto loader : mtcLoaders) {
        delete loader;
    }
}


QMtcLoader* QMtcLoadersGroup::getLoader(const mk_size_t index) const {
    return mtcLoaders.at(index);
}

uint8_t QMtcLoadersGroup::getSelectedLoaderIndex() const {
    return selectedLoader;
}

QMtcLoader* QMtcLoadersGroup::getSelectedLoader() const {
    return mtcLoaders.at(selectedLoader);
}

bool QMtcLoadersGroup::isSelectedLoader(uint8_t i) const {
    return i == selectedLoader;
}


void QMtcLoadersGroup::checkboxChanged(uint8_t index) {
    for (int i = 0; i < mtcLoaders.size(); ++i) {
        if (i != index) {
            mtcLoaders[i]->setIsChecked(false);
        }
    }

}