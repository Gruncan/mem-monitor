//
// Created by duncan on 12/31/24.
//

#ifndef MTC_POINT_H
#define MTC_POINT_H

#include <stdint.h>
#include <map>

namespace mtc {

struct MtcPoint {
    uint16_t* time_offset;
    uint16_t value;
};


}


#endif //MTC_POINT_H
