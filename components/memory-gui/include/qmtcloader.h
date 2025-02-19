
#ifndef QMTCLOADER_H
#define QMTCLOADER_H

#include "decoder-monitor.h"
#include "decoder-worker.h"
#include "mtccdecoder.h"
#include "qplotcontrolsidebar.h"


#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <qcheckbox.h>


class QMtcLoader : public QWidget {
    Q_OBJECT

  public:
    explicit QMtcLoader(QWidget* parent, const char* name, QPlotControlSidebar* sidebar, bool checked=false);
    ~QMtcLoader() override;

    MtcObject* getMtcObject();

    void setIsChecked(bool checked);

    bool isChecked;

  public Q_SLOTS:
    void load();
    void loaded(const std::string& filePath);
    void updateProgress(int progress);
    void checkBoxStateChanged(Qt::CheckState state);

  Q_SIGNALS:
    void decode(const std::string& filename);
    void removeFile();
    void displayMetaInfo();
    void enableNonDefaultFields(std::map<mk_size_t, bool>* defaultFields);
    void stateChanged();

  private:
    QVBoxLayout* mainLayout;
    QFrame* frame;
    QVBoxLayout* frameLayout;
    QLabel* label;
    QProgressBar* progressBar;
    QPushButton* button;

    QCheckBox* checkbox;

    DecoderWorker* worker;
    DecodeMonitor* monitor;

    QThread* workerThread;
    QThread* monitorThread;

    MtcObject* object;
};


#endif // QMTCLOADER_H
