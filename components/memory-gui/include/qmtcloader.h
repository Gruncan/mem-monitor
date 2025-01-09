
#ifndef QMTCLOADER_H
#define QMTCLOADER_H

#include "decoder-monitor.h"
#include "decoder-worker.h"
#include "qplotcontrolsidebar.h"
#include "mtccdecoder.h"


#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>


class QMtcLoader : public QWidget {
    Q_OBJECT

public:

    explicit QMtcLoader(QWidget* parent, const char* name, QPlotControlSidebar* sidebar);
    ~QMtcLoader() override;

    MtcObject* getMtcObject();

public Q_SLOTS:
    void load();
    void loaded(const std::string& filePath);
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

    DecoderWorker* worker;
    DecodeMonitor* monitor;

    QThread* workerThread;
    QThread* monitorThread;

    MtcObject* object;


};


#endif //QMTCLOADER_H
