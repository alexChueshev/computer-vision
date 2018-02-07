#ifndef COMPUTER_VISION_UTILS_H
#define COMPUTER_VISION_UTILS_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <descriptors.h>
#include <homography.h>

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

template<typename T> utils::MatchResolvedType<pi::descriptors::BDescriptor, T>
utils::drawMatches(const cv::Mat& src1, const cv::Mat& src2, const std::vector<std::pair<T, T>>& matches) {
    assert(src1.type() == CV_32FC3);
    assert(src2.type() == CV_32FC3);

    cv::Mat dst(std::max(src1.rows, src2.rows), src1.cols + src2.cols, CV_32FC3);

    //concat images
    src1.copyTo(cv::Mat(dst, cv::Rect(0, 0, src1.cols, src1.rows)));
    src2.copyTo(cv::Mat(dst, cv::Rect(src1.cols, 0, src2.cols, src2.rows)));

    //draw lines
    for(const auto& match : matches) {
        cv::line(dst,
                 cv::Point(match.first.point.col, match.first.point.row),
                 cv::Point(match.second.point.col + src1.cols, match.second.point.row),
                 cv::Scalar(.0, 1., 1.)); //yellow color
    }

    return dst;
}

template<typename T> utils::MatchResolvedType<pi::detectors::Point, T>
utils::drawMatches(const cv::Mat& src1, const cv::Mat& src2, const std::vector<std::pair<T, T>>& matches) {
    assert(src1.type() == CV_32FC3);
    assert(src2.type() == CV_32FC3);

    cv::Mat dst(std::max(src1.rows, src2.rows), src1.cols + src2.cols, CV_32FC3);

    //concat images
    src1.copyTo(cv::Mat(dst, cv::Rect(0, 0, src1.cols, src1.rows)));
    src2.copyTo(cv::Mat(dst, cv::Rect(src1.cols, 0, src2.cols, src2.rows)));

    //draw lines
    for(const auto& match : matches) {
        cv::line(dst,
                 cv::Point(match.first.col, match.first.row),
                 cv::Point(match.second.col + src1.cols, match.second.row),
                 cv::Scalar(.0, 1., 1.)); //yellow color
    }

    return dst;
}

template<typename T>
cv::Mat utils::drawMatches(const pi::Img& src1, const pi::Img& src2, const std::vector<std::pair<T, T>>& matches) {
    assert(src1.channels() == 1);
    assert(src2.channels() == 1);

    return utils::drawMatches(utils::convertToMat(utils::convertTo3Ch(src1)),
                              utils::convertToMat(utils::convertTo3Ch(src2)), matches);
}

#endif // COMPUTER_VISION_UTILS_H
