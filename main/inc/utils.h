#ifndef COMPUTER_VISION_UTILS_H
#define COMPUTER_VISION_UTILS_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <detectors.h>

#include <iomanip>
#include <ctime>
#include <chrono>

namespace utils {
    pi::Img load(const std::string& path);

    void render(const std::string& window, const pi::Img& img);

    void save(const std::string& path, const pi::Img& img,
              const std::string& ext = "png", bool addTime = true);

    pi::Img addPointsTo(const pi::Img& src, const std::vector<pi::detectors::Point>& points);

    float eulerDistance(int x1, int x2, int y1, int y2);

    float radius(const pi::Img& img);
}

#endif // COMPUTER_VISION_UTILS_H
