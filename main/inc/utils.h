#ifndef COMPUTER_VISION_UTILS_H
#define COMPUTER_VISION_UTILS_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <img.h>

#include <iomanip>
#include <ctime>
#include <chrono>

namespace utils {
    pi::Img load(const std::string& path);

    void render(const std::string& window, const pi::Img& img);

    void save(const std::string& path, const pi::Img& img,
              const std::string &ext = "png", bool addTime = true);
}

#endif // COMPUTER_VISION_UTILS_H
