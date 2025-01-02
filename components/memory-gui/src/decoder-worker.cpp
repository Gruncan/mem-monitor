


#include "decoder-worker.h"

#include <utility>
#include "mtc-decoder.h"

using namespace mtc;


DecoderWorker::DecoderWorker(QObject* parent, const std::shared_ptr<MtcDecoder>& decoder):
                                                    QObject(parent),
                                                    decoder(decoder)
{

}

void DecoderWorker::workerDecode(){
    printf("DecoderWorker::workerDecode()\n");
    decodedData = decoder->decode();
    emit workerDecodeFinished();
}

void DecoderWorker::setDecodeFilename(const std::string& filename) const {
    decoder->setFilename(filename);
}
