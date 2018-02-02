#ifndef COMPUTER_VISION_TRANSFORMS_H
#define COMPUTER_VISION_TRANSFORMS_H

#include <detectors.h>

namespace pi::transforms {
    constexpr int T_SIZE = 3;

    typedef std::array<float, T_SIZE> Transform1d;
    typedef std::array<Transform1d, T_SIZE> Transform2d;
}

#endif //COMPUTER_VISION_TRANSFORMS_H
