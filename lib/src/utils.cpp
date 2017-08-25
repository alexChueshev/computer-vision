#include "utils.h"

pi::Image pi::utils::loadImage(const std::string &path) {
    auto data = cv::imread(path);
    pi::Image image(data);
    return image;
}

void pi::utils::saveImage(const Image& image, const std::string& path) {
    cv::imwrite(path, image.pureData());
}

void pi::utils::renderImage(const Image &image, const std::string &windowName) {
    cv::namedWindow(windowName, CV_WINDOW_AUTOSIZE);
    cv::imshow(windowName, image.pureData());
    cv::waitKey(0);
}
