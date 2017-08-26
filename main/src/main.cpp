#include <iostream>

#include <image.h>
#include <utils.h>
#include <operations.h>

int main() {
    auto image = pi::utils::loadImage("/home/alexander/test.png");
    auto grayscale = pi::opts::grayscale(image);

    pi::utils::saveImage(grayscale, "/home/alexander/test01.png");
    pi::utils::renderImage(grayscale, "original image");


    return 0;
}