#ifndef COMPUTER_VISION_IMAGE_H
#define COMPUTER_VISION_IMAGE_H

#include <opencv2/core.hpp>

namespace pi {
    class Image;
}

class pi::Image {

public:
    typedef std::function<void(cv::Mat &src)> Operation;

private:
    cv::Mat image;

public:
    explicit Image(const cv::Mat &image);

    void operation(const Operation &opt);

    void operations(std::initializer_list<Operation> opts);

    int height() const;

    int width() const;

    cv::Mat pureData() const;

    Image clone();
};

#endif //COMPUTER_VISION_IMAGE_H