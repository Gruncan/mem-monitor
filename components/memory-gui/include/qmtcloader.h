
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
    explicit QMtcLoader(QWidget* parent, const char* name, QPlotControlSidebar* sidebar, uint8_t index, bool checked=false);
    ~QMtcLoader() override;

    MtcObject* getMtcObject();

    void setIsChecked(bool checked);

    void setCheckedPlots(const std::vector<mk_size_t>& keys);

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
    void enableNonDefaultFields(std::map<mk_size_t, bool>* defaultFields, uint8_t index);
    void setCheckedItems(std::vector<mk_size_t> keys);
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

    uint8_t index;

    std::map<mk_size_t, bool>* nonDefaultFields;

    bool isLoaded;
};


#endif // QMTCLOADER_H
