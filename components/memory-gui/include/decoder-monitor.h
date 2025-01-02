#ifndef DECODE_MONITOR_H
#define DECODE_MONITOR_H

#include <mtc-decoder.h>
#include <mtc-object.h>
#include <qobject.h>


class DecodeMonitor : public QObject{
    Q_OBJECT

public:
    DecodeMonitor(QObject* parent, const std::shared_ptr<mtc::MtcDecoder>& decoder);

public Q_SLOTS:
    void monitorProgress();

Q_SIGNALS:
    void progressQueried(int progress);

private:
    static uint progressCount;
    std::shared_ptr<mtc::MtcDecoder> decoder;

};




#endif //DECODE_MONITOR_H
