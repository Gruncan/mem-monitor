
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

private:
    std::vector<QMtcLoader*> mtcLoaders;

};






#endif //QMTCLOADERS_H
