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

   // pi::filters::Gaussian g(1);
   // auto img = pImage.pureData();
   // g.apply(img, pi::borders::BorderTypes::BORDER_CONSTANT);

    pi::utils::saveImage(pImage, "/home/alexander/test01.png");
    pi::utils::renderImage(pImage, "original image");

    return 0;
}