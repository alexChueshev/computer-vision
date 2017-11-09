#ifndef COMPUTER_VISION_UTILS_H
#define COMPUTER_VISION_UTILS_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <descriptors.h>

#include <iomanip>
#include <ctime>
#include <chrono>

namespace utils {
    pi::Img load(const std::string& path);

    void render(const std::string& window, const pi::Img& img);

    void render(const std::string& window, const cv::Mat& img);

    void save(const std::string& path, const pi::Img& img,
              const std::string& ext = "png", bool addTime = true);

    void save(const std::string& path, const cv::Mat& img,
              const std::string& ext = "png", bool addTime = true);

    cv::Mat convertToMat(const pi::Img& src);

    cv::Mat drawMatches(const pi::Img& src1, const pi::Img& src2, const std::vector<std::pair<
                        pi::descriptors::Descriptor, pi::descriptors::Descriptor>>& matches);

    pi::Img convertTo3Ch(const pi::Img& src);

    pi::Img addPointsTo(const pi::Img& src, const std::vector<pi::detectors::Point>& points);

    float euclidDistance(int x1, int x2, int y1, int y2);

    float radius(const pi::Img& img);
}

#endif // COMPUTER_VISION_UTILS_H
