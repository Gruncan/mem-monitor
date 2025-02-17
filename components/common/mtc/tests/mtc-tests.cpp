


#include "mem-monitor-config.h"
#include "mtccdecoder.h"
#include <chrono>
#include <fstream>
#include <iostream>

#include <cinttypes>
#include <tmtcdecoder.h>




int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return -1;
    }


    MtcObject object;

    createMtcObject(&object);

    auto start_time = std::chrono::high_resolution_clock::now();
    decode(argv[1], &object);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);


    printf("Size: %u\n", object.version);
    std::cout << duration.count() / 1'000'000.0  << " seconds\n";


    return 0;
}