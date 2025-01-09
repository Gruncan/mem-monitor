
#ifndef DECODER_WORKER_H
#define DECODER_WORKER_H

#include "mtccdecoder.h"
#include <qobject.h>


class DecoderWorker : public QObject {
    Q_OBJECT

public:
    DecoderWorker(QObject* parent, MtcObject* object);

public Q_SLOTS:
    void workerDecode(const std::string& filename);

Q_SIGNALS:
    void workerDecodeFinished(const std::string& filePath);

private:
    MtcObject* _object;

};


#endif //DECODER_WORKER_H
