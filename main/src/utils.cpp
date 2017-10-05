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

void utils::render(const std::string &window, const pi::Img &img) {
    assert(img.channels() == 1);

    cv::namedWindow(window, CV_WINDOW_AUTOSIZE);
    cv::imshow(window, cv::Mat(img.height(), img.width(),
                               CV_32FC1, const_cast<float*>(img.data())));
    cv::waitKey(0);
}

void utils::save(const std::string &path, const pi::Img &img) {
    assert(img.channels() == 1);

    pi::Img dst(img.height(), img.width(), img.channels());

    auto* dataSrc = img.data();
    auto* dataDst = dst.data();

    for(auto i = 0, size = img.dataSize(); i < size; i++) {
        dataDst[i] = 255 * dataSrc[i];
    }

    cv::imwrite(path, cv::Mat(dst.height(), dst.width(), CV_32FC1, dataDst));
}
