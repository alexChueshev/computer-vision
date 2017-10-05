#include <imageprocessing.h>
#include <operations.h>
#include <pyramid.h>

#include <utils.h>

using namespace pi;
using namespace std::placeholders;

void l1();

void l2();

int main() {
    //l1();
    l2();

    return 0;
}

void l1() {
    ImageProcessing imageProcessing("/home/alexander/Lenna.png", utils::load);

    filters::Sobel sobel;
    auto sobelFunction = std::bind(&filters::Sobel::apply, &sobel, _1, _2);

    filters::Gaussian gaussian(2.5);
    auto gaussianFunction = std::bind(&filters::Gaussian::apply, &gaussian, _1, _2);

    imageProcessing.opts({opts::grayscale, opts::normalize})
                   .filters({std::make_pair(borders::BORDER_REPLICATE, gaussianFunction),
                             std::make_pair(borders::BORDER_REPLICATE, sobelFunction)})
                    .render("result", utils::render);
}

void l2() {
    pyramids::GaussianPyramid pyramid;
    ImageProcessing imageProcessing("/home/alexander/Lenna.png", utils::load);

    auto image = imageProcessing.opts({opts::grayscale, opts::normalize}).image();
    pyramid.apply(image, 2).whileLoop([](const pyramids::Layer &layer) {
        utils::save("/home/alexander/lr2/" + std::to_string(layer.sigmaEffective) + ".png",
                    layer.img);
    });
}
