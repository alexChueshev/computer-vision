#ifndef COMPUTER_VISION_UTILS_H
#define COMPUTER_VISION_UTILS_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <descriptors.h>
#include <transforms.h>

#include <iomanip>
#include <ctime>
#include <chrono>
#include <type_traits>

namespace utils {
    pi::Img load(const std::string& path);

    std::vector<std::pair<std::string, pi::Img>> load(const std::string& pattern, bool isRecursive);

    void render(const std::string& window, const pi::Img& img);

    void render(const std::string& window, const cv::Mat& img);

    void save(const std::string& path, const pi::Img& img,
              const std::string& ext = "png", bool addTime = true);

    void save(const std::string& path, const cv::Mat& img,
              const std::string& ext = "png", bool addTime = true);

    void serialize(const std::string& filename, const std::vector<std::string>& paths,
                   const pi::transforms::Transform2d& transform2d, float probability);

    cv::Mat convertToMat(const pi::Img& src);

    cv::Mat convertToMat(const pi::transforms::Transform2d& transform2d);

    cv::Mat applyTransform(const pi::Img& src, const pi::transforms::Transform2d& transform2d,
                           int width, int height);

    std::vector<cv::Point2f> applyTransform(const std::vector<cv::Point2f>& points,
                                            const pi::transforms::Transform2d& transform2d);

    cv::Mat simpleStitching(const std::vector<cv::Mat>& warps, const pi::Img& src,
                            int width, int height);

    pi::Img convertTo3Ch(const pi::Img& src);

    pi::Img addPointsTo(const pi::Img& src, const std::vector<pi::detectors::Point>& points);

    cv::Mat addBlobsTo(const pi::Img& src, const std::vector<pi::detectors::SPoint>& points);

    cv::Mat addRectTo(const pi::Img& src, const std::vector<cv::Point2f>& points);

    std::vector<cv::Point2f> corners(const pi::Img& src, float shift);

    float euclidDistance(int x1, int x2, int y1, int y2);

    float radius(const pi::Img& img);

    template<typename Base, typename Derived>
    using MatchResolvedType = typename std::enable_if_t<std::is_base_of<Base, Derived>::value, cv::Mat>;

    template<typename T> MatchResolvedType<pi::descriptors::BDescriptor, T>
    drawMatches(const cv::Mat& src1, const cv::Mat& src2, const std::vector<std::pair<T, T>>& matches);

    template<typename T> MatchResolvedType<pi::detectors::Point, T>
    drawMatches(const cv::Mat& src1, const cv::Mat& src2, const std::vector<std::pair<T, T>>& matches);

    template<typename T>
    cv::Mat drawMatches(const pi::Img& src1, const pi::Img& src2, const std::vector<std::pair<T, T>>& matches);
}

#include <utils.hpp>

#endif // COMPUTER_VISION_UTILS_H
