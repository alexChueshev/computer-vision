#include <opencv2/imgcodecs.hpp>
#include "image.h"

pi::Image::Image(const cv::Mat &image) {
    this->image = image;
}

void pi::Image::operation(const Operation &opt) {
    opt(this->image);
}

void pi::Image::operations(std::initializer_list<pi::Image::Operation> opts) {
    for(auto opt: opts) {
        opt(this->image);
    }
}

int pi::Image::width() const {
    return this->image.cols;
}

int pi::Image::height() const {
    return this->image.rows;
}

cv::Mat pi::Image::pureData() const {
    return this->image;
}

pi::Image pi::Image::clone() {
    Image image(this->image.clone());
    return image;
}
