#ifndef COMPUTER_VISION_OPERATIONS_H
#define COMPUTER_VISION_OPERATIONS_H

#include <img.h>

namespace pi {
namespace opts {
    void grayscale(Img& src);

    void normalize(Img& src);

    void scale(Img& src);
}}

#endif //COMPUTER_VISION_OPERATIONS_H
