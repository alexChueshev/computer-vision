#include <iostream>

#include <image.h>
#include <utils.h>
#include <operations.h>
#include <filters.h>

using namespace pi;

int main() {
    auto oImage = utils::loadImage("/home/alexander/Lenna.png");
    auto pImage = oImage.clone();

    pImage.operations({pi::opts::grayscale,
                       pi::opts::normalize,
                       [](cv::Mat &src) {
                           filters::Gaussian(2).apply(src, borders::BORDER_REFLECT);
                           filters::Sobel().apply(src, borders::BORDER_REFLECT);
                       }
                      });

    utils::saveImage(pImage, "/home/alexander/test01.png");
    utils::renderImage(pImage, "original image");

    return 0;
}