#include "operations.h"

pi::Image pi::opts::grayscale(const Image &src) {
    cv::Mat grayscale(src.height(), src.width(), CV_8UC1);
    pi::Image dst(grayscale);

    auto srcData = src.pureData();
    auto srcIt = srcData.begin<cv::Vec3b>(),
            srcEnd = srcData.end<cv::Vec3b>();

    for (auto gIt = grayscale.begin<uchar>(),
                 gEnd = grayscale.end<uchar>(); gIt != gEnd; ++srcIt, ++gIt) {
        *gIt = (uchar) (.299 * (*srcIt)[2] + .587 * (*srcIt)[1] + .114 * (*srcIt)[0]);
    }

    normalize(dst);
    return dst;
}

void pi::opts::normalize(Image &src) {
    auto min = .0, max = .0;
    auto srcData = src.pureData();

    cv::minMaxLoc(src.pureData(), &min, &max);
    for (auto it = srcData.begin<uchar>(),
                 end = srcData.end<uchar>(); it != end; ++it) {
        *it = (*it - min) * (UCHAR_MAX / (max - min));
    }
}