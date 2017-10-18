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

    filters::Gaussian gaussian(1.8f);
    auto gaussianFunction = std::bind(&filters::Gaussian::apply, &gaussian, _1, _2);

    imageProcessing.opts({opts::grayscale, opts::normalize})
                   .filters({std::make_pair(borders::BORDER_REPLICATE, gaussianFunction),
                             std::make_pair(borders::BORDER_REPLICATE, sobelFunction)})
                    .render("result", utils::render)
                    .save("../examples/lr1/sobel", utils::save);
}

void l2() {
    pyramids::GaussianPyramid pyramid;
    ImageProcessing imageProcessing("/home/alexander/Lenna.png", utils::load);

    auto image = imageProcessing.opts({opts::grayscale, opts::normalize}).image();
    pyramid.apply(image, 2).whileLoop([](const pyramids::Layer &layer) {
        utils::save("../examples/lr2/" + std::to_string(layer.sigmaEffective), layer.img);
    });
}

void l3() {
    ImageProcessing imageProcessing("/home/alexander/Lenna.png", utils::load);
    imageProcessing.opts({opts::grayscale, opts::normalize});

    /**
     * @brief moravec
     */
    detectors::DetectorMoravec moravec(imageProcessing.image());
    auto imageMoravec = moravec.apply(borders::BORDER_REFLECT)
            .adaptNonMaximumSuppr(300, [](int x1, int x2, int y1, int y2) {
        return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    }).addPointsToImage();
    utils::render("moravec", imageMoravec);
    utils::save("../examples/lr3/moravec300points", imageMoravec);

    /**
     * @brief harris
     */
    auto gaussianSigma = 1.2f;
    auto gaussianSize = 2 * (int)(gaussianSigma * 3) + 1;
    const auto gaussianPatch = filters::Gaussian::data2d(gaussianSigma, gaussianSize);
    detectors::DetectorHarris harris(imageProcessing.image(),gaussianSize,
                                     [&gaussianPatch, &gaussianSize](int row, int col) {
        return gaussianPatch[row * gaussianSize + col];
    });
    auto imageHarris = harris.apply(borders::BORDER_REFLECT).addPointsToImage();
    utils::render("harris", imageHarris);
    utils::save("../examples/lr3/harrisclassic", imageHarris);
}
