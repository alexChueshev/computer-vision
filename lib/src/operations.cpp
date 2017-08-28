#include <opencv/cv.hpp>
#include "operations.h"

void pi::opts::grayscale(cv::Mat &src) {
    assert(src.depth() == CV_8U);
    assert(src.channels() == 3);

    cv::Mat grayscale(src.rows, src.cols, CV_8UC1);

    auto srcIt = src.begin<cv::Vec3b>(),
            srcEnd = src.end<cv::Vec3b>();

    for (auto gIt = grayscale.begin<uchar>(),
                 gEnd = grayscale.end<uchar>(); gIt != gEnd; ++srcIt, ++gIt) {
        *gIt = cv::saturate_cast<uchar>
                (.299 * (*srcIt)[2] + .587 * (*srcIt)[1] + .114 * (*srcIt)[0]);
    }

    src = std::move(grayscale);
}

void pi::opts::normalize(cv::Mat &src) {
    assert(src.depth() == CV_8U);
    assert(src.channels() == 1);

    auto min = .0, max = .0;

    cv::minMaxLoc(src, &min, &max);
    for (auto it = src.begin<uchar>(),
                 end = src.end<uchar>(); it != end; ++it) {
        *it = (*it - min) * (UCHAR_MAX / (max - min));
    }
}