

#include "qmtcloader.h"

#include <QFileDialog>
#include <QProgressBar>
#include <QPushButton>
#include <QThread>
#include <qboxlayout.h>
#include <qlabel.h>


QMtcLoader::QMtcLoader(QWidget* parent, const char* name) : QWidget(parent){
    setObjectName(name);

    mainLayout = new QVBoxLayout(this);
    frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);
    mainLayout->addWidget(frame);

    frameLayout = new QVBoxLayout(frame);
    frameLayout->setContentsMargins(10, 10, 10, 10);

    label = new QLabel("", this);
    label->setAlignment(Qt::AlignCenter);
    label->setMinimumSize(125, 40);
    frameLayout->addWidget(label);

    progressBar = new QProgressBar(this);
    progressBar->setAlignment(Qt::AlignCenter);
    progressBar->setMinimumSize(125, 20);
    frameLayout->addWidget(progressBar);

    button = new QPushButton(name, this);
    frameLayout->addWidget(button);

    connect(button, &QPushButton::clicked, this, &QMtcLoader::load);


    workerThread = new QThread;
    monitorThread = new QThread;

    decoder = std::make_shared<mtc::MtcDecoder>();
    worker = new DecoderWorker(nullptr, decoder);
    monitor = new DecodeMonitor(nullptr, decoder);

    worker->moveToThread(workerThread);
    monitor->moveToThread(monitorThread);

    // connect(workerThread, &QThread::started, worker, &DecoderWorker::workerDecode);
    connect(worker, &DecoderWorker::workerDecodeFinished, this, &QMtcLoader::loaded);
    // connect(monitorThread, &QThread::started, monitor, &DecodeMonitor::monitorProgress);

    // connect(worker, &DecoderWorker::workerDecodeFinished, workerThread, &QThread::quit);
    // connect(workerThread, &QThread::finished, decoderWorker, &QObject::deleteLater);

    connect(this, &QMtcLoader::decode, worker, &DecoderWorker::workerDecode);
    connect(this, &QMtcLoader::decode, monitor, &DecodeMonitor::monitorProgress);
    connect(monitor, &DecodeMonitor::progressQueried, this, &QMtcLoader::updateProgress);

    workerThread->start();
    monitorThread->start();
}

QMtcLoader::~QMtcLoader() {
    delete button;
    delete progressBar;
    delete label;
    delete frameLayout;
    delete frame;
    delete mainLayout;
}


void QMtcLoader::load() {
    const QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "Memory Time Encoding (*.mtc);;All Files (*)");
    if (!filePath.isEmpty()) {
        qDebug() << "Selected file:" << filePath;
    }else {
        qDebug() << "No file selected";
        return;
    }

    emit decode(filePath.toStdString());

}


void QMtcLoader::updateProgress(int progress) {
    progressBar->setValue(progress);
}

void QMtcLoader::loaded(const std::shared_ptr<mtc::MtcObject>& data) {
    label->setText(QString("Version %1\n: Length: %2").arg(data->get_version()).arg(data->get_length()));
}