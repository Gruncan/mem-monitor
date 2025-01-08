


#include <fstream>
#include <iostream>
#include <memory-decoder.h>
#include <mtc-decoder.h>

using namespace mtc;

void printMemoryUsage() {
    std::ifstream statusFile("/proc/self/status");
    std::string line;

    if (!statusFile.is_open()) {
        std::cerr << "Error: Unable to open /proc/self/status." << std::endl;
        return;
    }

    while (std::getline(statusFile, line)) {
        if (line.find("VmRSS:") == 0 || line.find("VmSize:") == 0) {
            std::cout << line << std::endl;
        }
    }
    statusFile.close();
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    MtcDecoder mtc = MtcDecoder("/home/duncan/Desktop/uwb_test1.mtc");
    std::shared_ptr<MtcObject> object = mtc.decode();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << duration.count() << " seconds" << std::endl;
    std::cout << object->get_length() << std::endl;
    printMemoryUsage();
    return 0;
}