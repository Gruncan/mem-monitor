


#include <chrono>
#include <fstream>
#include <iostream>
#include "mtccdecoder.h"

#ifdef linux
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
    MtcObject object;
    initaliseMtcObject(&object);

    decode("C:\\Development\\University\\Thesis\\uwb_test3.mtc",  &object);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << duration.count() << " seconds" << std::endl;
    std::cout << object.size << std::endl;
#ifdef linux
    printMemoryUsage();
#endif

    uint64_t repeatCount = 0;
    for (int i=0; i < object.point_map[165].length; i++ ) {
        repeatCount += object.point_map[165].points[i].repeated + 1;
    }
    uint64_t  timesCount = 0;
    for (int i=0; i < object._times_length; i++ ) {
        timesCount += object.times[i].repeated + 1;
    }

    std::cout << "Total points " << repeatCount << std::endl;
    std::cout << "Total times " << timesCount << std::endl;
    std::cout << "Size: " << object.size << std::endl;
    return 0;
}