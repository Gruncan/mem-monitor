
#ifndef MEMORY_DECODER_H
#define MEMORY_DECODER_H

#include <vector>
#include <string>
#include "mtc-object.h"

namespace mtc {

    class MemoryDecoder {

    protected:
        std::shared_ptr<std::vector<char>> buffer;
        std::string filename;
        int length;

    public:

        MemoryDecoder(const std::string& filename);
        MemoryDecoder();

        virtual ~MemoryDecoder();

        void load_file();
        void setFilename(const std::string& filename);
        std::string getFilename();

        virtual std::shared_ptr<MtcObject> decode() = 0;

    };
}
#endif //MEMORY_DECODER_H
