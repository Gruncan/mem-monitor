
#ifndef QMTCLOADERS_H
#define QMTCLOADERS_H

#include "qmtcloader.h"


#include <qwidget.h>

class QMtcLoadersGroup : public QWidget {
    Q_OBJECT

  public:
    explicit QMtcLoadersGroup(QWidget* parent = nullptr, std::vector<QMtcLoader*> widgets = {});
    ~QMtcLoadersGroup();

    QMtcLoader* getLoader(mk_size_t index) const;

    QMtcLoader* getSelectedLoader() const;

    void checkboxChanged(uint8_t index);

  private:
    std::vector<QMtcLoader*> mtcLoaders;
    uint8_t selectedLoader;
};


#endif // QMTCLOADERS_H
