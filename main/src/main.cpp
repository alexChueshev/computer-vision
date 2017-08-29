#include <iostream>

#include <image.h>
#include <utils.h>
#include <operations.h>
#include <filters.h>

int main() {
    auto oImage = pi::utils::loadImage("/home/alexander/Lenna.png");
    auto pImage = oImage.clone();

    //grayscale + normalization
    pImage.operations({pi::opts::grayscale, pi::opts::normalize});

    auto img = pImage.pureData();

    pi::filters::Gaussian g(1);
    g.apply(img, pi::borders::BorderTypes::BORDER_REPLICATE);

    pi::filters::Sobel s;
    s.apply(img, pi::borders::BorderTypes::BORDER_REPLICATE);

    pImage.operation(pi::opts::normalize);


    pi::utils::saveImage(pImage, "/home/alexander/test01.png");
    pi::utils::renderImage(pImage, "original image");

    return 0;
}