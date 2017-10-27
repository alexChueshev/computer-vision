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
    auto image = opts::normalize(
                    opts::grayscale(
                        utils::load("/home/alexander/Lenna.png")));

    auto moravecImage = detectors::addPointsTo(image,
                            detectors::adaptiveNonMaximumSuppresion(
                                detectors::moravec(image), 300,
                                detectors::maxRadius(image), detectors::eulerDistance));
    utils::render("moravec", moravecImage);
    utils::save("../examples/lr3/moravec300points", moravecImage);

    auto harrisImage = detectors::addPointsTo(image, detectors::harris(image));
    utils::render("harris", harrisImage);
    utils::save("../examples/lr3/harrisclassic", harrisImage);
}
