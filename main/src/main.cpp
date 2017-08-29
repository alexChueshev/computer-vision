#include <iostream>

#include <image.h>
#include <utils.h>
#include <operations.h>
#include <filters.h>

int main() {
    auto oImage = pi::utils::loadImage("/home/alexander/test.png");
    auto pImage = oImage.clone();

    //grayscale + normalization
    pImage.operations({pi::opts::grayscale, pi::opts::normalize});

    auto f = pi::borders::Factory::get();
    auto s = f(2,2,cv::Mat(5,5, CV_32FC1));

    pi::utils::saveImage(pImage, "/home/alexander/test01.png");
    pi::utils::renderImage(pImage, "original image");

    return 0;
}