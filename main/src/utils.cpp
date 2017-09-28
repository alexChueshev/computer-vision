#include "utils.h"

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

void utils::render(const std::__cxx11::string &window, const pi::Img &img) {
    cv::namedWindow(window, CV_WINDOW_AUTOSIZE);
    cv::imshow(window, cv::Mat(img.height(), img.width(),
                               CV_32FC1, const_cast<float*>(img.data())));
    cv::waitKey(0);
}
