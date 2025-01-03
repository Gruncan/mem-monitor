
#ifndef QMTCLOADER_H
#define QMTCLOADER_H

#include "decoder-monitor.h"
#include "decoder-worker.h"


#include <QProgressBar>
#include <QPushButton>
#include <mtc-decoder.h>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qwidget.h>


class QMtcLoader : public QWidget {

    Q_OBJECT

public:

    explicit QMtcLoader(QWidget* parent = nullptr, const char* name = nullptr);
    ~QMtcLoader();

    std::shared_ptr<mtc::MtcObject> getMtcData();

public Q_SLOTS:
    void load();
    void loaded(const std::shared_ptr<mtc::MtcObject>& data, const std::string& filePath);
    void updateProgress(int progress);

Q_SIGNALS:
    void decode(const std::string& filename);
    void removeFile();
    void displayMetaInfo();

private:
    QVBoxLayout* mainLayout;
    QFrame* frame;
    QVBoxLayout* frameLayout;
    QLabel* label;
    QProgressBar* progressBar;
    QPushButton* button;

    std::shared_ptr<mtc::MtcDecoder> decoder;
    DecoderWorker* worker;
    DecodeMonitor* monitor;

    QThread* workerThread;
    QThread* monitorThread;

    std::shared_ptr<mtc::MtcObject> data;


};


#endif //QMTCLOADER_H
