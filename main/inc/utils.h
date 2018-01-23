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

    cv::Mat drawMatches(const cv::Mat& src1, const cv::Mat& src2, const std::vector<std::pair<
                        pi::descriptors::Descriptor, pi::descriptors::Descriptor>>& matches);

    cv::Mat drawMatches(const cv::Mat& src1, const cv::Mat& src2, const std::vector<std::pair<
                        pi::detectors::Point, pi::detectors::Point>>& matches);

    pi::Img convertTo3Ch(const pi::Img& src);

    pi::Img addPointsTo(const pi::Img& src, const std::vector<pi::detectors::Point>& points);

    cv::Mat addBlobsTo(const pi::Img& src, const std::vector<pi::detectors::SPoint>& points);

    float euclidDistance(int x1, int x2, int y1, int y2);

    float radius(const pi::Img& img);

    template<typename T>
    cv::Mat drawMatches(const pi::Img& src1, const pi::Img& src2, const std::vector<std::pair<T, T>>& matches) {
        assert(src1.channels() == 1);
        assert(src2.channels() == 1);

        return drawMatches(convertToMat(convertTo3Ch(src1)), convertToMat(convertTo3Ch(src2)), matches);
    }
}

#endif // COMPUTER_VISION_UTILS_H
