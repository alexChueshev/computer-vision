#ifndef COMPUTER_VISION_IMAGE_H
#define COMPUTER_VISION_IMAGE_H

#include <opencv2/core.hpp>

namespace pi {
    class Image;
}

class pi::Image {

private:
    cv::Mat image;

public:
    explicit Image(const cv::Mat &image);

    int height() const;

    int width() const;

    cv::Mat pureData() const;

    Image clone();
};

#endif //COMPUTER_VISION_IMAGE_H