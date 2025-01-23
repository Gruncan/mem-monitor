


#include "decoder-monitor.h"

#include <QThread>
#include <iostream>

DecodeMonitor::DecodeMonitor(QObject* parent, MtcObject* object) : QObject(parent), _object(object) {
}

void DecodeMonitor::monitorProgress() {
    int progress;
    do {
        progress = queryDecodeProgress(_object);
        emit progressQueried(progress);
        QThread::msleep(100);
    } while (progress < 98);
}
