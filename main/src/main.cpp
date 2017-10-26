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
    l2();
    //l3();

    return 0;
}

void l1() {
    auto image = filters::sobel(
                      filters::gaussian(
                        opts::normalize(
                            opts::grayscale(
                                utils::load("/home/alexander/Lenna.png"))),
                                    1.8f, borders::BORDER_REPLICATE),
                                        borders::BORDER_REPLICATE, filters::magnitude);

    utils::render("result", image);
    utils::save("../examples/lr1/sobel", image);
}

void l2() {
    pyramids::iterate(
        pyramids::gpyramid(
            opts::normalize(
                opts::grayscale(
                    utils::load("/home/alexander/Lenna.png"))),
                        2, pyramids::logOctavesCount),
                            [](const pyramids::Layer& layer) {
        utils::save("../examples/lr2/" + std::to_string(layer.sigmaEffective), layer.img);
    });
}

void l3() {
    /*ImageProcessing imageProcessing("/home/alexander/Lenna.png", utils::load);
    imageProcessing.opts({opts::grayscale, opts::normalize});

    detectors::DetectorMoravec moravec(imageProcessing.image());
    auto imageMoravec = moravec.apply(borders::BORDER_REFLECT)
            .adaptNonMaximumSuppr(300, [](int x1, int x2, int y1, int y2) {
        return std::hypot((x1 - x2), (y1 - y2));
    }).addPointsToImage();
    utils::render("moravec", imageMoravec);
    utils::save("../examples/lr3/moravec300points", imageMoravec);

    auto gaussianSigma = 1.2f;
    auto gaussianSize = 2 * (int)(gaussianSigma * 3) + 1;
    const auto gaussianPatch = filters::Gaussian::data2d(gaussianSigma, gaussianSize);
    detectors::DetectorHarris harris(imageProcessing.image(),gaussianSize,
                                     [&gaussianPatch, &gaussianSize](int row, int col) {
        return gaussianPatch[row * gaussianSize + col];
    });
    auto imageHarris = harris.apply(borders::BORDER_REFLECT).addPointsToImage();
    utils::render("harris", imageHarris);
    utils::save("../examples/lr3/harrisclassic", imageHarris);*/
}
