#ifndef DECODE_MONITOR_H
#define DECODE_MONITOR_H

#include <mtccdecoder.h>
#include <qobject.h>


class DecodeMonitor : public QObject {
    Q_OBJECT

  public:
    DecodeMonitor(QObject* parent, MtcObject* object);

  public Q_SLOTS:
    void monitorProgress();

  Q_SIGNALS:
    void progressQueried(int progress);

  private:
    static uint progressCount;

    MtcObject* _object;
};


#endif // DECODE_MONITOR_H
