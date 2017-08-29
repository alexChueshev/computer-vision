#include <iostream>

#include <image.h>
#include <utils.h>
#include <operations.h>
#include <filters.h>

using namespace pi;
using namespace std;
struct A {
    A() {
        cout << "def c" << endl;
    }

    A(const A &a) {
        cout << "copy c" << endl;
    }

    A(A &&a) {
        cout << "move c" << endl;
    }

    A& operator=(A &&a) {
        cout << "move =" << endl;
    }

    A& operator=(const A &a) {
        cout << "copy =" << endl;
    }
};

int test(A a) {
    A b = std::move(a);
    int c = 2;
}

int main() {

    A a;
    test(a);

    auto oImage = utils::loadImage("/home/alexander/Lenna.png");
    auto pImage = oImage.clone();

    pImage.operations({pi::opts::grayscale,
                       pi::opts::normalize,
                       [](cv::Mat &src) {
                           filters::Gaussian(1).apply(src, borders::BORDER_REFLECT);
                           filters::Sobel().apply(src, borders::BORDER_REFLECT);
                       }
                      });

    utils::saveImage(pImage, "/home/alexander/test01.png");
    utils::renderImage(pImage, "original image");

    return 0;
}