#include <iostream>

#include <image.h>
#include <utils.h>
#include <operations.h>

int main() {
    auto oImage = pi::utils::loadImage("/home/alexander/test.png");
    auto pImage = oImage.clone();

    //grayscale + normalization
    pImage.operations({pi::opts::grayscale, pi::opts::normalize});

    pi::utils::saveImage(pImage, "/home/alexander/test01.png");
    pi::utils::renderImage(pImage, "original image");

    return 0;
}