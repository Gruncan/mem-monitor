


#include "memory-decoder.h"
#include <fstream>
#include <iostream>

namespace mtc {

    MemoryDecoder::MemoryDecoder(const std::string& filename) :
            buffer(nullptr),
            filename(filename),
            length(0)
    {

    }

    MemoryDecoder::MemoryDecoder() :
            buffer(nullptr),
            filename(""),
            length(0)
    {

    }

    MemoryDecoder::~MemoryDecoder() {
        buffer.reset();
    }

    void MemoryDecoder::load_file() {
        std::ifstream input(filename, std::ios::binary);

        if (!input) {
            std::cerr << "Error opening file " << filename << std::endl;
            return;
        }

        input.seekg(0, std::ios::end);
        const std::streampos length = input.tellg();

        input.seekg(0, std::ios::beg);

        buffer = std::make_shared<std::vector<char>>(length);
        buffer->reserve(length);

        input.read(buffer->data(), length);

        input.close();

    }

    void MemoryDecoder::setFilename(const std::string& filename) {
        this->filename = filename;
    }

    std::string MemoryDecoder::getFilename() {
        return this->filename;
    }


} // namespace mtc