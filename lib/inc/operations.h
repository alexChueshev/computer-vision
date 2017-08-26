#ifndef COMPUTER_VISION_OPERATIONS_H
#define COMPUTER_VISION_OPERATIONS_H

#include <image.h>

namespace pi::opts {
    Image grayscale(const Image &src);

    void normalize(Image &src);
}

#endif //COMPUTER_VISION_OPERATIONS_H
