#include <descriptors.h>
#include <hough.h>

#include <utils.h>

#include <boost/filesystem.hpp>

using namespace pi;
using namespace boost;

std::vector<descriptors::SiDescriptor> descriptors(const pi::Img&);

void serialize(const std::string&, const std::vector<std::string>&, const transforms::Transform2d&, float);

std::vector<cv::Point2f> rect(const pi::Img&, const transforms::Transform2d&, float);

int main(int argc, char *argv[]) {
    cv::CommandLineParser parser(argc, argv, "{obj||}{src||}{dst||}");
    const auto objPath = parser.get<std::string>("obj");
    const auto srcPath = parser.get<std::string>("src");
    const auto dstPath = parser.get<std::string>("dst");

    if(!parser.check()) {
        parser.printErrors();
        return 0;
    }

    std::vector<std::pair<std::string, pi::Img>> images;
    auto object = utils::load(objPath);
    for(const auto &image : utils::load(srcPath, true)) {
        images.push_back(std::pair<std::string, pi::Img>(
                             std::move(image.first),
                             opts::normalize(opts::grayscale(image.second))));
    }

    if(images.size() == 0 || object.empty()) {
        return 0;
    }

    object = opts::normalize(opts::grayscale(object));
    auto objDescriptors = ::descriptors(object);

    for(const auto &image : images) {
        const auto &img = image.second;
        const auto filename = filesystem::path{dstPath} /= filesystem::path{image.first}.stem();

        auto matches = descriptors::match<detectors::SPoint>(objDescriptors, ::descriptors(img), .65f);
        auto vHypothesis = transforms::verify(transforms::hough(img.dimensions(), object.dimensions(), matches)
                                              , matches.size(), .8f);

        const auto &transform2d = vHypothesis.first;
        const auto probability = vHypothesis.second;

        if(probability > 0) {
            ::serialize((filesystem::path{filename} += ".yml").string(), {objPath, image.first}
                        , transform2d.first, probability);
            utils::save(filename.string(), utils::addRectTo(img, ::rect(object, transform2d.first, 5.f)));
        }
    }

    return 0;
}

std::vector<descriptors::SiDescriptor> descriptors(const pi::Img& img) {
    auto gpyramid = pyramids::gpyramid(img, 3, 3, pyramids::logOctavesCount);
    auto dog = pyramids::dog(gpyramid);

    return descriptors::siDescriptors(detectors::shiTomasi(dog, detectors::blobs(dog), 1e-5f)
                                      , gpyramid, [](const auto& descriptor) {
        return descriptors::normalize(descriptors::trim(descriptors::normalize(descriptor)));
    });
}

void serialize(const std::string& filename, const std::vector<std::string>& paths,
               const transforms::Transform2d& transform2d, float probability) {
    cv::FileStorage file(filename, cv::FileStorage::WRITE);

    file << "Paths" << paths;
    file << "Probability" << probability;
    file << "Transform2d" << utils::convertToMat(transform2d);
}

std::vector<cv::Point2f> rect(const pi::Img& obj, const transforms::Transform2d& transform2d, float shift) {

    auto x1 = 0.f, y1 = 0.f;
    auto x2 = obj.width() + shift, y2 = y1;
    auto x3 = x2, y3 = obj.height() + shift;
    auto x4 = x1, y4 = y3;

    return utils::applyTransform({cv::Point2f(x1, y1), cv::Point2f(x2, y2)
                                  , cv::Point2f(x3, y3), cv::Point2f(x4, y4)
                                 }, transform2d);
}
