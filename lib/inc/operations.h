#ifndef COMPUTER_VISION_OPERATIONS_H
#define COMPUTER_VISION_OPERATIONS_H

#include <image.h>

namespace pi::opts {
    void grayscale(cv::Mat &src);

    void normalize(cv::Mat &src);
}

#endif //COMPUTER_VISION_OPERATIONS_H
