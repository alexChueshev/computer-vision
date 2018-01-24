#include <utils.h>

using namespace pi;

Img utils::load(const std::string& path) {
    cv::Mat src = cv::imread(path, cv::IMREAD_COLOR);
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

cv::Mat utils::drawMatches(const cv::Mat& src1, const cv::Mat& src2, const std::vector<std::pair<
                           descriptors::Descriptor, descriptors::Descriptor>>& matches) {
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

cv::Mat utils::drawMatches(const cv::Mat& src1, const cv::Mat& src2, const std::vector<std::pair<
                           detectors::Point, detectors::Point>>& matches) {
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

cv::Mat utils::applyTransform(const Img& src, const transforms::Transform2d& transform2d, int width, int height) {
    cv::Mat warp;

    cv::warpPerspective(convertToMat(src), warp, convertToMat(transform2d), cv::Size(width, height));

    return warp;
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
