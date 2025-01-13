

#include "qmtcloadersgroup.h"

#include "qmtcloader.h"


#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>


QMtcLoadersGroup::QMtcLoadersGroup(QWidget* parent, std::vector<QMtcLoader*> loaders) : QWidget(parent),
                                                                                    mtcLoaders(loaders){
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFrame* frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);
    mainLayout->addWidget(frame);

    QHBoxLayout* layout = new QHBoxLayout(frame);
    for (const auto loader : loaders) {
        layout->addWidget(loader);
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



