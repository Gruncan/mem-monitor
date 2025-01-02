//
// Created by duncan on 12/31/24.
//

#ifndef MTC_POINT_H
#define MTC_POINT_H

#include <stdint.h>
#include <map>

namespace mtc {

struct MtcPoint {
    uint8_t time_offset;
    std::map<uint8_t, uint16_t> data;

};


}


#endif //MTC_POINT_H
