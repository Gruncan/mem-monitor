
#ifndef QMTCLOADER_H
#define QMTCLOADER_H

#include "decoder-monitor.h"
#include "decoder-worker.h"
#include "qplotcontrolsidebar.h"
#include <mtc-decoder.h>


#include <QProgressBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QPushButton>


class QMtcLoader : public QWidget {
    Q_OBJECT

public:

    explicit QMtcLoader(QWidget* parent, const char* name, QPlotControlSidebar* sidebar);
    ~QMtcLoader() override;

    std::shared_ptr<mtc::MtcObject> getMtcData();

public Q_SLOTS:
    void load();
    void loaded(const std::shared_ptr<mtc::MtcObject>& data, const std::string& filePath);
    void updateProgress(int progress);

Q_SIGNALS:
    void decode(const std::string& filename);
    void removeFile();
    void displayMetaInfo();
    void enableNonDefaultFields(std::map<mk_size_t, bool>* defaultFields);

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
