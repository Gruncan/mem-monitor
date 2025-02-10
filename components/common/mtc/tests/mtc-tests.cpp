


#include <chrono>
#include <iostream>
#include <mtccdecoder.h>


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

    printf("Size: %lu\n", object.size);
    std::cout << duration.count() / 1'000'000.0  << " seconds\n";

    uint64_t timeLength = 0;
    for (uint32_t i = 0; i < object._times_length; i++) {
        timeLength += object.times[i].repeated + 1;
    }

    std::cout << timeLength << " size\n";

    return 0;
}