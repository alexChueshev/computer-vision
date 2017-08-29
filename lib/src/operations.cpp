#include "operations.h"

void pi::opts::grayscale(cv::Mat &src) {
    assert(src.type() == CV_8UC3);

    cv::Mat grayscale(src.rows, src.cols, CV_32FC1);

    auto srcIt = src.begin<cv::Vec3b>(),
            srcEnd = src.end<cv::Vec3b>();

    for (auto gIt = grayscale.begin<float>(),
                 gEnd = grayscale.end<float>(); gIt != gEnd; ++srcIt, ++gIt) {
        *gIt = (.299 * (*srcIt)[2] + .587 * (*srcIt)[1] + .114 * (*srcIt)[0]) / 255;
    }

    src = std::move(grayscale);
}

void pi::opts::normalize(cv::Mat &src) {
    assert(src.type() == CV_32FC1);

    auto min = .0, max = .0;

    cv::minMaxLoc(src, &min, &max);
    for (auto it = src.begin<uchar>(),
                 end = src.end<uchar>(); it != end; ++it) {
        *it = (*it - min) * (1 / (max - min));
    }
}