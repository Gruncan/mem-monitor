
#ifndef QMTCLOADERS_H
#define QMTCLOADERS_H

#include "qmtcloader.h"


#include <qwidget.h>

class QMtcLoadersGroup : public QWidget {

    Q_OBJECT

public:

    explicit QMtcLoadersGroup(QWidget* parent = nullptr, std::vector<QMtcLoader*> widgets = {});
    ~QMtcLoadersGroup();


};






#endif //QMTCLOADERS_H
