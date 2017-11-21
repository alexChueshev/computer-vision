#include <pyramid.h>
#include <descriptors.h>

#include <utils.h>

using namespace pi;

void l1();

void l2();

void l3();

void l4();

void l5();

int main() {
    //l1();
    //l2();
    //l3();
    //l4();
    l5();

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

    auto moravecImage = utils::addPointsTo(image,
                            detectors::adaptiveNonMaximumSuppresion(
                                detectors::moravec(image), 300,
                                utils::radius(image), utils::euclidDistance));
    utils::render("moravec", moravecImage);
    utils::save("../examples/lr3/moravec300points", moravecImage);

    auto harrisImage = utils::addPointsTo(image, detectors::harris(image));
    utils::render("harris", harrisImage);
    utils::save("../examples/lr3/harrisclassic", harrisImage);
}

void l4() {
    auto normalize = [](const descriptors::Descriptor& descriptor) {
        return descriptors::normalize(descriptors::trim(descriptors::normalize(descriptor)));
    };

    auto image1 = opts::normalize(
                    opts::grayscale(
                        utils::load("/home/alexander/Lenna.png")));

    auto image2 = opts::normalize(
                    opts::grayscale(
                        utils::load("/home/alexander/Lenna.png")));

    auto matchImage = utils::drawMatches(image2, image1,
                                         descriptors::match(descriptors::hog(detectors::harris(image2),
                                                                 filters::sobel(image2, borders::BORDER_REFLECT),
                                                                 normalize),
                                                            descriptors::hog(detectors::harris(image1),
                                                                 filters::sobel(image1, borders::BORDER_REFLECT),
                                                                 normalize)));
    utils::render("matches", matchImage);
    utils::save("../examples/lr4/matches", matchImage);
}

void l5() {
}
