//
// Created by duncan on 12/30/24.
//

#ifndef MTCDECODER_H
#define MTCDECODER_H

#include "memory-decoder.h"
#include "mtc-object.h"

namespace mtc {

    class MtcDecoder : public MemoryDecoder {
    private:
      std::shared_ptr<MtcObject> object;
      uint64_t offset;

    public:
          MtcDecoder(const std::string& filename);
          MtcDecoder();
          ~MtcDecoder() override;

          std::shared_ptr<MtcObject> decode() override;
          double getProgress() const;


    protected:
          void decode_header(uint64_t offset);
          uint64_t decode_mem_time_offset(uint64_t offset) const;
          uint64_t decode_data_length(uint64_t offset) const;
          void decode_mem_data(uint64_t data_length, uint64_t time_offset, uint64_t offset);

    };

}

#endif //MTCDECODER_H
