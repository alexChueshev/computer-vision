#include <iostream>

#include <image.h>
#include <utils.h>

int main() {

    std::cout << "Hello, World!" << std::endl;

    auto image = pi::utils::loadImage("/home/alexander/test.png");

    pi::utils::saveImage(image, "/home/alexander/test01.png");
    pi::utils::renderImage(image, "original image");

    return 0;
}