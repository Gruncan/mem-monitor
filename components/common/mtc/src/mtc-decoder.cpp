

#include "mtc-decoder.h"


#define BIT_MASK_2 0b11
#define BIT_MASK_4 0b1111
#define BIT_MASK_5 0b11111
#define BIT_MASK_6 0b111111
#define BIT_MASK_7 0b1111111

namespace mtc {


    MtcDecoder::MtcDecoder(const std::string& filename) :
            MemoryDecoder(filename),
            object(nullptr)

    {

    }

    void MtcDecoder::decode_header(uint64_t offset) {
        const std::vector header_bytes(buffer->begin() + offset, buffer->begin() + offset + 5);

        int version = header_bytes[0];

        std::tm timeinfo = {};
        timeinfo.tm_year = (header_bytes[1] >> 2) - 100;
        timeinfo.tm_mon = (header_bytes[1] & BIT_MASK_2) << 6 | (header_bytes[2] >> 6);
        timeinfo.tm_mday = (header_bytes[2] >> 1) & BIT_MASK_5;
        timeinfo.tm_hour = (header_bytes[2] & BIT_MASK_7) >> 3 | header_bytes[3] >> 4;
        timeinfo.tm_min = (header_bytes[3] & BIT_MASK_4) << 2 | header_bytes[4] >> 6;
        timeinfo.tm_sec = header_bytes[4] & BIT_MASK_6;

        object = std::make_shared<MtcObject>(version, timeinfo);

    }

    uint64_t MtcDecoder::decode_mem_time_offset(const uint64_t offset) const {
        return buffer->at(offset) << 8 | buffer->at(offset + 1);
    }

    uint64_t MtcDecoder::decode_data_length(const uint64_t offset) const {
        return buffer->at(offset) << 8 | buffer->at(offset + 1);
    }

    void MtcDecoder::decode_mem_data(uint64_t data_length, uint64_t time_offset, const uint64_t offset) {
        MtcPoint point;
        for (uint64_t i = 0; i < data_length; i += 3) {
            uint8_t data_key = buffer->at(offset + i);
            const uint16_t data_value = buffer->at(offset + i + 1) << 8 | buffer->at(offset + i + 2);
            point.data[data_key] = data_value;
        }

        point.time_offset = time_offset;
        object->add_point(point);
    }



    std::shared_ptr<MtcObject> MtcDecoder::decode() override {
        if (buffer == nullptr) {
            load_file();
            if (buffer == nullptr || buffer->empty()) {
                return nullptr;
            }
        }

        uint64_t offset = 0;
        decode_header(offset);
        offset += 5;

        while (offset < length) {
            const uint64_t time_offset = decode_mem_time_offset(offset);
            offset += 2;

            const uint64_t data_length = decode_data_length(offset);
            offset += 2;

            decode_mem_data(data_length, time_offset, offset);

            offset += data_length;
        }


        return object;
    }



}