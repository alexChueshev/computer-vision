#ifndef COMPUTER_VISION_OPERATIONS_H
#define COMPUTER_VISION_OPERATIONS_H

#include <img.h>

#include <algorithm>

namespace pi::opts {
    Img grayscale(const Img& src);

    Img normalize(const Img& src);

    Img scale(const Img& src);
}

#endif //COMPUTER_VISION_OPERATIONS_H
