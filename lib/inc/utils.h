#ifndef COMPUTER_VISION_UTILS_H
#define COMPUTER_VISION_UTILS_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <image.h>

namespace pi::utils {
    Image loadImage(const std::string& path);
    void saveImage(const Image& image, const std::string& path);
    void renderImage(const Image& image, const std::string &windowName);
}

#endif //COMPUTER_VISION_UTILS_H
