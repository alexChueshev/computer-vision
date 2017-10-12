#include <imageprocessing.h>
#include <pyramid.h>
#include <detectors.h>

#include <utils.h>

using namespace pi;
using namespace std::placeholders;

void l1();

void l2();

void l3();

int main() {
    //l1();
    //l2();
    l3();

    return 0;
}

void l1() {
    ImageProcessing imageProcessing("/home/alexander/Lenna.png", utils::load);

    filters::Sobel sobel;
    auto sobelFunction = std::bind(&filters::Sobel::apply, &sobel, _1, _2);

    filters::Gaussian gaussian(2.2f);
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

void l3() {
    ImageProcessing imageProcessing("/home/alexander/Lenna.png", utils::load);
    imageProcessing.opts({opts::grayscale, opts::normalize});

    detectors::DetectorMoravec moravec;
    auto imageMoravec = imageProcessing.image().clone();
    moravec.apply(imageMoravec, borders::BORDER_REFLECT).addPointsTo(imageMoravec);
    utils::render("moravek", imageMoravec);

    auto gaussianSigma = .9f;
    auto gaussianSize = 2 * (int)(gaussianSigma * 3) + 1;
    const auto gaussianPatch = filters::Gaussian::data2d(gaussianSigma, gaussianSize);
    detectors::DetectorHarris harris(gaussianSize, [&gaussianPatch, &gaussianSize](int row, int col) {
        return gaussianPatch[row * gaussianSize + col];
    });
    auto imageHarris = imageProcessing.image().clone();
    harris.apply(imageHarris, borders::BORDER_REFLECT).addPointsTo(imageHarris);
    utils::render("harris", imageHarris);
}
