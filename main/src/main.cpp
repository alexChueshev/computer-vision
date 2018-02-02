#include <descriptors.h>
#include <homography.h>
#include <hough.h>

#include <utils.h>

using namespace pi;

void l1();

void l2();

void l3();

void l4();

void l5();

void l6();

void l7();

void l8();

void l9();

int main() {
    //l1();
    //l2();
    //l3();
    //l4();
    //l5();
    //l6();
    //l7();
    //l8();
    l9();

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
        utils::save("../examples/lr2/" + std::to_string(layer.sigmaGlobal), layer.img);
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
    auto normalize = [](const auto& descriptor) {
        return descriptors::normalize(descriptors::trim(descriptors::normalize(descriptor)));
    };

    auto image1 = opts::normalize(
                    opts::grayscale(
                        utils::load("/home/alexander/Lenna.png")));

    auto image2 = opts::normalize(
                    opts::grayscale(
                        utils::load("/home/alexander/Lenna.png")));

    auto matchImage = utils::drawMatches(image2, image1,
                                         descriptors::match<detectors::Point>(
                                               descriptors::bDescriptors(detectors::harris(image2),
                                                    filters::sobel(image2, borders::BORDER_REFLECT),
                                                    normalize),
                                               descriptors::bDescriptors(detectors::harris(image1),
                                                    filters::sobel(image1, borders::BORDER_REFLECT),
                                                    normalize)));
    utils::render("matches", matchImage);
    utils::save("../examples/lr4/matches", matchImage);
}

void l5() {
    auto normalize = [](const auto& descriptor) {
        return descriptors::normalize(descriptors::trim(descriptors::normalize(descriptor)));
    };

    auto image1 = opts::normalize(
                    opts::grayscale(
                        utils::load("/home/alexander/Lenna.png")));

    auto image2 = opts::normalize(
                    opts::grayscale(
                        utils::load("/home/alexander/Lenna.png")));

    auto matchImage = utils::drawMatches(image2, image1,
                                         descriptors::match<detectors::Point>(
                                                descriptors::riDescriptors(detectors::harris(image2, 5, .015f),
                                                     filters::sobel(image2, borders::BORDER_REFLECT),
                                                     normalize),
                                                descriptors::riDescriptors(detectors::harris(image1, 5, .015f),
                                                     filters::sobel(image1, borders::BORDER_REFLECT),
                                                     normalize), .55f));
    utils::render("matches", matchImage);
    utils::save("../examples/lr5/matches", matchImage);
}

void l6() {
    auto normalize = [](const auto& descriptor) {
        return descriptors::normalize(descriptors::trim(descriptors::normalize(descriptor)));
    };

    auto image1 = opts::normalize(
                    opts::grayscale(
                        utils::load("/home/alexander/Lenna.png")));
    auto gpyramid1 = pyramids::gpyramid(image1, 3, 3, pyramids::logOctavesCount);
    auto dog1 = pyramids::dog(gpyramid1);
    auto points1 = detectors::shiTomasi(dog1, detectors::blobs(dog1), 25e-5f);

    auto image2 = opts::normalize(
                    opts::grayscale(
                        utils::load("/home/alexander/Lenna.png")));
    auto gpyramid2 = pyramids::gpyramid(image2, 3, 3, pyramids::logOctavesCount);
    auto dog2 = pyramids::dog(gpyramid2);
    auto points2 = detectors::shiTomasi(dog2, detectors::blobs(dog2), 25e-5f);

    auto mImage1 = utils::addBlobsTo(image1, points1);
    auto mImage2 = utils::addBlobsTo(image2, points2);

    auto matchImage = utils::drawMatches(mImage2, mImage1,
                                          descriptors::match<detectors::Point>(
                                                 descriptors::siDescriptors(points2, gpyramid2, normalize
                                                                            , descriptors::D_HISTO_SIZE
                                                                            , descriptors::D_HISTO_NUMS
                                                                            , descriptors::D_BINS
                                                                            , borders::BORDER_REPLICATE, false),
                                                 descriptors::siDescriptors(points1, gpyramid1, normalize
                                                                            , descriptors::D_HISTO_SIZE
                                                                            , descriptors::D_HISTO_NUMS
                                                                            , descriptors::D_BINS
                                                                            , borders::BORDER_REPLICATE, false)
                                                 , .62f));

    utils::render("matches", matchImage);
    utils::save("../examples/lr6/matches", matchImage);
}

void l7() {
    auto normalize = [](const auto& descriptor) {
        return descriptors::normalize(descriptors::trim(descriptors::normalize(descriptor)));
    };

    auto image1 = opts::normalize(
                    opts::grayscale(
                        utils::load("/home/alexander/Lenna.png")));
    auto gpyramid1 = pyramids::gpyramid(image1, 3, 3, pyramids::logOctavesCount);
    auto dog1 = pyramids::dog(gpyramid1);
    auto points1 = detectors::shiTomasi(dog1, detectors::blobs(dog1), 25e-5f);

    auto image2 = opts::normalize(
                    opts::grayscale(
                        utils::load("/home/alexander/Lenna.png")));
    auto gpyramid2 = pyramids::gpyramid(image2, 3, 3, pyramids::logOctavesCount);
    auto dog2 = pyramids::dog(gpyramid2);
    auto points2 = detectors::shiTomasi(dog2, detectors::blobs(dog2), 25e-5f);

    auto mImage1 = utils::addBlobsTo(image1, points1);
    auto mImage2 = utils::addBlobsTo(image2, points2);

    auto matchImage = utils::drawMatches(mImage2, mImage1,
                                          descriptors::match<detectors::Point>(
                                                 descriptors::siDescriptors(points2, gpyramid2, normalize)
                                                 , descriptors::siDescriptors(points1, gpyramid1, normalize)
                                                 , .62f));

    utils::render("matches", matchImage);
    utils::save("../examples/lr7/matches", matchImage);
}

void l8() {
    auto normalize = [](const auto& descriptor) {
        return descriptors::normalize(descriptors::trim(descriptors::normalize(descriptor)));
    };

    auto image1 = opts::normalize(
                    opts::grayscale(
                        utils::load("/home/alexander/panorama_1.jpg")));
    auto gpyramid1 = pyramids::gpyramid(image1, 3, 3, pyramids::logOctavesCount);
    auto dog1 = pyramids::dog(gpyramid1);

    auto image2 = opts::normalize(
                    opts::grayscale(
                        utils::load("/home/alexander/panorama_2.jpg")));
    auto gpyramid2 = pyramids::gpyramid(image2, 3, 3, pyramids::logOctavesCount);
    auto dog2 = pyramids::dog(gpyramid2);

    auto transform2d = transforms::homography(descriptors::match<detectors::Point>(
                                                     descriptors::siDescriptors(
                                                            detectors::shiTomasi(dog2, detectors::blobs(dog2)
                                                                                 , 25e-5f)
                                                            , gpyramid2, normalize)
                                                     , descriptors::siDescriptors(
                                                            detectors::shiTomasi(dog1, detectors::blobs(dog1)
                                                                                 , 25e-5f)
                                                            , gpyramid1, normalize)
                                                     , .62f));

    auto width = image1.width() + image2.width();
    auto height = image1.height() + image2.height();

    auto pano = utils::simpleStitching({utils::applyTransform(image2, transform2d, width, height)}
                                       , image1, width, height);

    utils::render("pano", pano);
    utils::save("../examples/lr8/result", pano);
}

void l9() {
    auto normalize = [](const auto& descriptor) {
        return descriptors::normalize(descriptors::trim(descriptors::normalize(descriptor)));
    };

    auto image1 = opts::normalize(
                    opts::grayscale(
                        utils::load("/home/alexander/hough/box_in_scene.png")));
    auto gpyramid1 = pyramids::gpyramid(image1, 3, 3, pyramids::logOctavesCount);
    auto dog1 = pyramids::dog(gpyramid1);

    auto image2 = opts::normalize(
                    opts::grayscale(
                        utils::load("/home/alexander/hough/box.png")));
    auto gpyramid2 = pyramids::gpyramid(image2, 3, 3, pyramids::logOctavesCount);
    auto dog2 = pyramids::dog(gpyramid2);

    auto transform2d = transforms::hough(image1.dimensions(), image2.dimensions()
                                         , descriptors::match<detectors::SPoint>(
                                             descriptors::siDescriptors(
                                                 detectors::shiTomasi(dog2, detectors::blobs(dog2), 15e-5f)
                                                 , gpyramid2, normalize)
                                             , descriptors::siDescriptors(
                                                 detectors::shiTomasi(dog1, detectors::blobs(dog1), 15e-5f)
                                                 , gpyramid1, normalize)
                                             , .65f));

    cv::Rect rect(transform2d.x - image2.width() * transform2d.scale / 2
             , transform2d.y - image2.height() * transform2d.scale / 2
             , image2.width() * transform2d.scale
             , image2.height() * transform2d.scale);

    auto result = utils::convertToMat(utils::convertTo3Ch(image1));
    cv::rectangle(result, rect, cv::Scalar(.0, 1., 1.));

    utils::render("hough", result);
}
