


#include <iostream>
#include <memory-decoder.h>
#include <mtc-decoder.h>

using namespace mtc;

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    MtcDecoder mtc = MtcDecoder("/home/duncan/Desktop/uwb_test1.mtc");
    std::shared_ptr<MtcObject> object = mtc.decode();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << duration.count() << " seconds" << std::endl;

    return 0;
}