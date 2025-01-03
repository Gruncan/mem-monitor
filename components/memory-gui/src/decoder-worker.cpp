


#include "decoder-worker.h"

#include <utility>
#include "mtc-decoder.h"

using namespace mtc;


DecoderWorker::DecoderWorker(QObject* parent, const std::shared_ptr<MtcDecoder>& decoder):
                                                    QObject(parent),
                                                    decoder(decoder)
{

}

void DecoderWorker::workerDecode(const std::string& filename){
    printf("DecoderWorker::workerDecode()\n");
    decoder->setFilename(filename);
    decodedData = decoder->decode();
    emit workerDecodeFinished(decodedData);
}
