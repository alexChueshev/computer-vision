#include <utils.h>

using namespace pi;

Img utils::load(const std::string& path) {
    cv::Mat src = cv::imread(path, cv::IMREAD_COLOR);
    if(src.empty()) return Img();

    assert(src.type() == CV_8UC3);
    Img img(src.rows, src.cols, src.channels());

    auto* data = img.data();
    auto step = img.step();

    for(auto i = 0, rows = src.rows; i < rows; i++) {
        auto* ptr = src.ptr<uchar>(i);
        auto row = i * step;

        for(auto j = 0; j < step; j++) {
            data[row + j] = ptr[j];
        }
    }

    return img;
}

std::vector<pi::Img> utils::load(const std::string& pattern, bool isRecursive) {
    std::vector<pi::Img> images;
    std::vector<cv::String> paths;

    cv::glob(pattern, paths, isRecursive);
    for(const auto &path : paths) {
        auto img = utils::load(path);
        if(!img.empty()) {
            images.push_back(std::move(img));
        }
    }

    return images;
}

void utils::render(const std::string& window, const Img& img) {
    assert(img.channels() == 1 || img.channels() == 3);

    utils::render(window, convertToMat(img));
}

void utils::render(const std::string& window, const cv::Mat& img) {
    cv::namedWindow(window, CV_WINDOW_AUTOSIZE);
    cv::imshow(window, img);
    cv::waitKey(0);
}

void utils::save(const std::string& path, const Img& img, const std::string& ext, bool addTime) {
    assert(img.channels() == 1 || img.channels() == 3);

    utils::save(path, convertToMat(img), ext, addTime);
}

void utils::save(const std::string& path, const cv::Mat& img, const std::string& ext, bool addTime) {
    assert(img.channels() == 1 || img.channels() == 3);

    std::stringstream ss;
    ss << path;
    if(addTime) {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%X");
    }
    ss << "." << ext;
    std::string s = ss.str();

    cv::Mat saved;
    img.convertTo(saved, CV_8U, 255);
    cv::imwrite(ss.str(), saved);
}

cv::Mat utils::convertToMat(const Img& src) {
    auto type = src.channels() == 1 ? CV_32FC1 : CV_32FC3;

    return cv::Mat(src.height(), src.width(), type, const_cast<float*>(src.data())).clone();
}

cv::Mat utils::convertToMat(const transforms::Transform2d& transform2d) {
    cv::Mat dst(transform2d.size(), transform2d[0].size(), CV_32F);

    for(auto i = 0, rows = dst.rows; i < rows; i++) {
        auto* ptr = dst.ptr<float>(i);

        for(auto j = 0, cols = dst.cols; j < cols; j++) {
            ptr[j] = transform2d[i][j];
        }
    }

    return dst;
}

cv::Mat utils::applyTransform(const Img& src, const transforms::Transform2d& transform2d, int width, int height) {
    cv::Mat warp;

    cv::warpPerspective(convertToMat(src), warp, convertToMat(transform2d), cv::Size(width, height));

    return warp;
}

std::vector<cv::Point2f> utils::applyTransform(const std::vector<cv::Point2f>& points,
                                               const transforms::Transform2d& transform2d) {
    std::vector<cv::Point2f> transformed;
    cv::perspectiveTransform(points, transformed, utils::convertToMat(transform2d));

    return transformed;
}

cv::Mat utils::simpleStitching(const std::vector<cv::Mat>& warps, const Img& src, int width, int height) {
    cv::Mat dst(height, width, CV_32FC1);

    cv::Mat roi(dst, cv::Rect(0, 0, src.width(), src.height()));
    for(const auto &warp : warps) {
        cv::Mat wroi(dst, cv::Rect(0, 0, warp.cols, warp.rows));
        warp.copyTo(wroi);
    }
    convertToMat(src).copyTo(roi);

    return dst;
}

Img utils::addPointsTo(const Img& src, const std::vector<detectors::Point>& points) {
    assert(src.channels() == 1);

    //convert to 3 ch
    Img dst = convertTo3Ch(src);

    //add points to image
    for(const auto &point : points) {
        auto* pixel = dst.at(point.row, point.col);
        *(pixel + 0) = .0f;
        *(pixel + 1) = 1.f;
        *(pixel + 2) = 1.f; //yellow color
    }

    return dst;
}

cv::Mat utils::addBlobsTo(const Img& src, const std::vector<detectors::SPoint>& points) {
    assert(src.channels() == 1);

    //convert to cv::Mat
    auto dst = convertToMat(convertTo3Ch(src));
    auto color = cv::Scalar(0, 0, 1);

    //add blobs to image
    for(const auto &point : points) {
        cv::circle(dst, cv::Point(point.col, point.row), point.sigmaGlobal, color);
    }

    return dst;
}

cv::Mat utils::addRectTo(const pi::Img& src, const std::vector<cv::Point2f>& points) {
    assert(src.channels() == 1);

    auto dst = convertToMat(convertTo3Ch(src));
    auto color = cv::Scalar(0, 0, 1);

    for(size_t i = 0, size = points.size(); i < size; i++) {
        cv::line(dst, points[i], points[(i + 1) % size], color);
    }

    return dst;
}

std::vector<cv::Point2f> utils::corners(const pi::Img& src, float shift) {
    auto x1 = 0.f, y1 = 0.f;
    auto x2 = src.width() + shift, y2 = y1;
    auto x3 = x2, y3 = src.height() + shift;
    auto x4 = x1, y4 = y3;

    return { cv::Point2f(x1, y1), cv::Point2f(x2, y2), cv::Point2f(x3, y3), cv::Point2f(x4, y4) };
}

Img utils::convertTo3Ch(const Img& src) {
    assert(src.channels() == 1);

    Img dst(src.height(), src.width(), 3);

    auto* dataDst = dst.data();
    auto* dataSrc = src.data();
    auto channels = dst.channels();

    for(auto i = 0, size = src.dataSize(); i < size; i++) {
        dataDst[i * channels] = dataSrc[i];
        dataDst[i * channels + 1] = dataSrc[i];
        dataDst[i * channels + 2] = dataSrc[i];
    }

    return dst;
}

float utils::euclidDistance(int x1, int x2, int y1, int y2) {
    return std::hypot((x1 - x2), (y1 - y2));
}

float utils::radius(const Img& img)  {
    auto dimension = std::min(img.height(), img.width());
    return std::hypot(dimension, dimension) / 2;
}
