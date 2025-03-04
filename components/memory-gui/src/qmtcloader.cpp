

#include "qmtcloader.h"

#include <QFileDialog>
#include <QProgressBar>
#include <QPushButton>
#include <QThread>


#define IDS(size) \
{static_cast<unsigned char>(size + 1), static_cast<unsigned char>(0x2 + size), static_cast<unsigned char>(0x3 + size),\
                    static_cast<unsigned char>(0x4 + size), static_cast<unsigned char>(0x5 + size),\
                    static_cast<unsigned char>(0x6 + size), static_cast<unsigned char>(0x7 + size), static_cast<unsigned char>(0x8 + size),\
                    static_cast<unsigned char>(0x9 + size)}\

QMtcLoader::QMtcLoader(QWidget* parent, const char* name, QPlotControlSidebar* sidebar) : QWidget(parent), sidebar(sidebar) {
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
    createMtcObject(object);

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
    const QString filePath =
        QFileDialog::getOpenFileName(this, "Open File", "", "Memory Time Encoding (*.mtc);;All Files (*)");
    if (!filePath.isEmpty()) {
        qDebug() << "Selected file:" << filePath;
    } else {
        qDebug() << "No file selected";
        return;
    }
    std::string extension = std::filesystem::path(filePath.toStdString()).extension().string();
    if (extension == ".mtc") {
        emit decode(filePath.toStdString());
    } else {
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
    label->setText(QString("%1\nVersion: %2\nLength: %3\n")
                       .arg(QString::fromStdString(filePath))
                       .arg(object->version)
                       .arg(object->size));
    label->setStyleSheet("");

    std::map<mk_size_t, bool>* nonDefaultFields = new std::map<mk_size_t, bool>();

    if (object->version != 7) {
        for (mk_size_t i = 0; i < object->_key_size; i++) {
            (*nonDefaultFields)[i] = object->point_map[i].points[0].value == 0 && object->point_map[i].length == 1;
        }
        if (object->version % 2 != 0) {
            for (int arr[] = {0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0}; const auto v : arr) {
                (*nonDefaultFields)[v] = false;
            }
        }
    } else {
        auto nameMapping = new std::map<mk_size_t, std::string>();
        for (unsigned char size = 0; size < object->_proc_size * 10; size += 10) {
            unsigned int pid = object->point_map[size].points->value;
            for (auto [key, value] : mtc::MTC_BASE7_INDEX_MAPPING) {
                (*nameMapping)[key + size] = std::format("{} ({})", value, pid);
            }
        }
        sidebar->setNameMapping(nameMapping);

        std::vector<mtc::MtcCategories> MTC_PROC_CATEGORIES = std::vector<mtc::MtcCategories>();
        for (unsigned char size = 0; size < object->_proc_size * 10; size += 10) {
            MTC_PROC_CATEGORIES.push_back({std::format("Process {}", object->point_map[size].points->value),
               IDS(size)});
        }
        sidebar->setCategories(MTC_PROC_CATEGORIES);

    }

    emit enableNonDefaultFields(nonDefaultFields);
}

MtcObject* QMtcLoader::getMtcObject() {
    return object;
}