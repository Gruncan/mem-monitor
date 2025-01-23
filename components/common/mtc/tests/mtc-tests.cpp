


#include "mtccdecoder.h"
#include <chrono>
#include <fstream>
#include <iostream>

#include <tmtcdecoder.h>

#ifdef unix
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
#endif

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    TMtcObject object;
    initaliseTMtcObject(&object);

    decode_tmtc("/home/duncan/Development/Uni/Thesis/Data/single_uwb_tests_run.tmtc", &object);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << duration.count() / 1'000'000.0 << " seconds" << std::endl;
    std::cout << "Size: " << object.size << std::endl;

    return 0;
}