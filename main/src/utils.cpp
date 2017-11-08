#include <utils.h>

pi::Img utils::load(const std::string &path) {
    cv::Mat src = cv::imread(path, cv::IMREAD_COLOR);
    assert(src.type() == CV_8UC3);

    pi::Img img(src.rows, src.cols, src.channels());

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

void utils::render(const std::string &window, const pi::Img &img) {
    assert(img.channels() == 1 || img.channels() == 3);

    auto type = img.channels() == 1 ? CV_32FC1 : CV_32FC3;

    cv::namedWindow(window, CV_WINDOW_AUTOSIZE);
    cv::imshow(window, cv::Mat(img.height(), img.width(),
                               type, const_cast<float*>(img.data())));
    cv::waitKey(0);
}

void utils::save(const std::string &path, const pi::Img &img, const std::string &ext, bool addTime) {
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

    pi::Img dst(img.height(), img.width(), img.channels());

    auto* dataSrc = img.data();
    auto* dataDst = dst.data();
    auto type = img.channels() == 1 ? CV_32FC1 : CV_32FC3;

    for(auto i = 0, size = img.dataSize(); i < size; i++) {
        dataDst[i] = 255 * dataSrc[i];
    }

    cv::imwrite(ss.str(), cv::Mat(dst.height(), dst.width(), type, dataDst));
}

pi::Img utils::addPointsTo(const pi::Img& src, const std::vector<pi::detectors::Point>& points) {
    assert(src.channels() == 1);

    //convert to 3 ch
    pi::Img dst(src.height(), src.width(), 3);

    auto* dataDst = dst.data();
    auto* dataSrc = src.data();
    auto channels = dst.channels();

    for(auto i = 0, size = src.dataSize(); i < size; i++) {
        dataDst[i * channels] = dataSrc[i];
        dataDst[i * channels + 1] = dataSrc[i];
        dataDst[i * channels + 2] = dataSrc[i];
    }

    //add points to image
    for(const auto &point : points) {
        auto* pixel = dst.at(point.row, point.col);
        *(pixel + 0) = .0f;
        *(pixel + 1) = 1.0f;
        *(pixel + 2) = 1.f; //yellow color
    }

    return dst;
}

float utils::euclidDistance(int x1, int x2, int y1, int y2) {
    return std::hypot((x1 - x2), (y1 - y2));
}

float utils::radius(const pi::Img& img)  {
    auto dimension = std::min(img.height(), img.width());
    return std::hypot(dimension, dimension) / 2;
}
