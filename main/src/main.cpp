#include <imageprocessing.h>
#include <operations.h>
#include <pyramid.h>

#include <utils.h>

using namespace pi;

int main() {
    pyramids::GaussianPyramid pyramid;
    ImageProcessing imageProcessing("/home/alexander/Lenna.png", utils::load);

    auto image = imageProcessing.opts({opts::grayscale, opts::normalize}).image();
    pyramid.apply(image, 2).whileLoop([](const pyramids::Layer &layer) {
        utils::save("/home/alexander/lr2/" + std::to_string(layer.sigmaEffective) + ".png",
                    layer.img);
    });

    return 0;
}
