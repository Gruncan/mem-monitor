

#include "mtc-decoder.h"

#include <iostream>


#define BIT_MASK_2 0b11
#define BIT_MASK_4 0b1111
#define BIT_MASK_5 0b11111
#define BIT_MASK_6 0b111111
#define BIT_MASK_7 0b1111111


#define CAST_UINT16(v1, v2) static_cast<uint16_t>(static_cast<uint8_t>(v1) << 8 | static_cast<uint8_t>(v2))

namespace mtc {


    MtcDecoder::MtcDecoder(const std::string& filename) :
            MemoryDecoder(filename),
            object(nullptr),
            offset(0)

    {

    }

    MtcDecoder::MtcDecoder() : object(nullptr), offset(0)
    {

    }

    MtcDecoder::~MtcDecoder() {

    }

    void MtcDecoder::decode_header(uint64_t offset) {
        const std::vector header_bytes(buffer->begin() + offset, buffer->begin() + offset + 5);

        int version = header_bytes[0];

        std::tm timeinfo = {};
        timeinfo.tm_year = static_cast<unsigned char>(header_bytes[1] >> 2) - 100;
        timeinfo.tm_mon = static_cast<unsigned char>((header_bytes[1] & BIT_MASK_2) << 6 | (header_bytes[2] >> 6));
        timeinfo.tm_mday = static_cast<unsigned char>((header_bytes[2] >> 1) & BIT_MASK_5);
        timeinfo.tm_hour = static_cast<unsigned char>((header_bytes[2] & BIT_MASK_7) >> 3 | header_bytes[3] >> 4);
        timeinfo.tm_min = static_cast<unsigned char>((header_bytes[3] & BIT_MASK_4) << 2 | header_bytes[4] >> 6);
        timeinfo.tm_sec = static_cast<unsigned char>(header_bytes[4] & BIT_MASK_6);

        object = std::make_shared<MtcObject>(version, timeinfo);

    }

    inline uint16_t* MtcDecoder::decode_mem_time_offset(const uint64_t offset) const {
        return new uint16_t(CAST_UINT16(buffer->at(offset), buffer->at(offset + 1)));
    }

    inline uint16_t MtcDecoder::decode_data_length(const uint64_t offset) const {
        return CAST_UINT16(buffer->at(offset), buffer->at(offset + 1));
    }

    void MtcDecoder::decode_mem_data(const uint16_t data_length, uint16_t* time_offset, const uint64_t offset) const {
        object->add_time(time_offset);
        for (uint64_t i = 0; i < data_length; i += 3) {
            const uint8_t data_key = static_cast<uint8_t>(buffer->at(offset + i));
            const uint16_t data_value = CAST_UINT16(buffer->at(offset + i + 1), buffer->at(offset + i + 2));
            MtcPoint* point = new MtcPoint{time_offset, data_value};
            object->add_point(data_key, point);
        }
    }



    std::shared_ptr<MtcObject> MtcDecoder::decode() {
        if (buffer == nullptr) {
            load_file();
            if (buffer == nullptr || buffer->empty()) {
                return nullptr;
            }
        }

        offset = 0;
        decode_header(offset);
        offset += 5;

        while (offset < buffer->size()) {
            try {
                uint16_t* time_offset = decode_mem_time_offset(offset);
                offset += 2;

                const uint64_t data_length = decode_data_length(offset);
                offset += 2;

                decode_mem_data(data_length, time_offset, offset);

                offset += data_length;
            } catch (...) {
                std::cout << "MtcDecoder::decode: Failed to decode data length" << std::endl;
                break;
            }
        }

        return object;
    }

    double MtcDecoder::getProgress() const {
        if (buffer == nullptr || buffer->empty()) {
            return 0.0;
        }
        return (static_cast<double>(offset) / static_cast<double>(buffer->size())) * 100;
    }




}