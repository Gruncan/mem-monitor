




#include "decoder-monitor.h"

#include <QThread>

DecodeMonitor::DecodeMonitor(QObject* parent, const std::shared_ptr<mtc::MtcDecoder>& decoder):
                                                    QObject(parent),
                                                    decoder(decoder)
{

}

void DecodeMonitor::monitorProgress() {
    double progress;
    do {
        progress = decoder->getProgress();
        emit progressQueried(static_cast<int>(progress));
        QThread::msleep(100);
    }while (progress <= 99);

}
