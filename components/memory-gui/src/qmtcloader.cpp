

#include "qmtcloader.h"

#include <QFileDialog>
#include <QProgressBar>
#include <QPushButton>
#include <QThread>



QMtcLoader::QMtcLoader(QWidget* parent, const char* name, QPlotControlSidebar* sidebar) : QWidget(parent){
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

    object = static_cast<MtcObject*>(malloc(sizeof(MtcObject)));
    initaliseMtcObject(object);

    worker = new DecoderWorker(nullptr, object);
    monitor = new DecodeMonitor(nullptr, object);

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

    connect(this, &QMtcLoader::enableNonDefaultFields, sidebar, &QPlotControlSidebar::enableNonDefaultFields);

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
    // TODO destroy threads
    free(object);
}


void QMtcLoader::load() {
    const QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "Memory Time Encoding (*.mtc);;All Files (*)");
    if (!filePath.isEmpty()) {
        qDebug() << "Selected file:" << filePath;
    }else {
        qDebug() << "No file selected";
        return;
    }
    std::string extension = std::filesystem::path(filePath.toStdString()).extension().string();
    if (extension == ".mtc") {
        emit decode(filePath.toStdString());
    }else {
        label->setText("Not a valid .mtc file");
        label->setStyleSheet("QLabel { color: rgb(255, 0, 0); }");
    }

}


void QMtcLoader::updateProgress(const int progress) {
    progressBar->setValue(progress);
}

void QMtcLoader::loaded(const std::string& filePath) {
    monitorThread->exit();
    progressBar->setValue(100);
    label->setText(QString("%1\nVersion: %2\nLength: %3\n").arg(QString::fromStdString(filePath)).arg(object->version).arg(object->size));
    label->setStyleSheet("");
    std::map<mk_size_t, bool>* nonDefaultFields = new std::map<mk_size_t, bool>();

    for (mk_size_t i = 0; i < KEY_SIZE; i++) {
        (*nonDefaultFields)[i] = object->point_map[i].points[0].value == 0 && object->point_map[i].length == 1;
    }
    emit enableNonDefaultFields(nonDefaultFields);
}

MtcObject* QMtcLoader::getMtcObject() {
    return object;
}