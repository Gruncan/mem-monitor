




#include "decoder-monitor.h"

#include <QThread>

DecodeMonitor::DecodeMonitor(QObject* parent, MtcObject* object): QObject(parent), _object(object)
{

}

void DecodeMonitor::monitorProgress() {
    uint8_t progress;
    do {
        progress = queryDecodeProgress(_object);
        emit progressQueried(progress);
        QThread::msleep(100);
    }while (progress <= 99);

}
