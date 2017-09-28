#ifndef COMPUTER_VISION_BORDERS_UTILS_H
#define COMPUTER_VISION_BORDERS_UTILS_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <img.h>

namespace utils {
    pi::Img load(const std::string& path);

    void render(const std::string& window, const pi::Img& img);
}

#endif // COMPUTER_VISION_BORDERS_UTILS_H
