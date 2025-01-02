
#ifndef DECODER_WORKER_H
#define DECODER_WORKER_H

#include <mtc-object.h>
#include <mtc-decoder.h>
#include <qobject.h>


class DecoderWorker : public QObject {
    Q_OBJECT

public:
    DecoderWorker(QObject* parent, const std::shared_ptr<mtc::MtcDecoder>& decoder);

    void setDecodeFilename(const std::string& filename) const;

public Q_SLOTS:

    void workerDecode();

Q_SIGNALS:
    void workerDecodeFinished();

private:
    std::shared_ptr<mtc::MtcDecoder> decoder;
    std::shared_ptr<mtc::MtcObject> decodedData;

};


#endif //DECODER_WORKER_H
