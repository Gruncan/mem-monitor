



#include "tmtcdecoder.h"
#include <argp.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <ostream>

enum SimulationSpeed {
    NO_DELAY,
    REAL,
};

enum SimulationSpeed parse_speed(const char* speedStr) {
    if (strcmp(speedStr, "nodelay") == 0)
        return NO_DELAY;
    else if (strcmp(speedStr, "real") == 0)
        return REAL;

}



int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Usage: ./simulator <filename>.tmtc <nodelay|real>\n");
    }

    char* filename = argv[1];
    char* speedStr = argv[2];
    enum SimulationSpeed speed = parse_speed(speedStr);

    void* mem = operator new(10);
    std::cout << mem << std::endl;

    return 0;
}