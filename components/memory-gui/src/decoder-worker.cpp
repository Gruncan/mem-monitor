


#include "decoder-worker.h"

#include <filesystem>


DecoderWorker::DecoderWorker(QObject* parent, MtcObject* object) : QObject(parent), _object(object) {
}

void DecoderWorker::workerDecode(const std::string& filename) {
    printf("DecoderWorker::workerDecode()\n");
    decode(filename.c_str(), _object);
    emit workerDecodeFinished(std::filesystem::path(filename).filename().string());
}
