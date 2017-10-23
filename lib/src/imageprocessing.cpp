#include "imageprocessing.h"

using namespace pi;

ImageProcessing::ImageProcessing(const Img& img)
    : _img(img) {
}

ImageProcessing::ImageProcessing(Img&& img)
    : _img(std::move(img)) {
}

ImageProcessing::ImageProcessing(const std::string& path,
                                 const LoadFunction& loadFunction)
    : _img(loadFunction(path))
{
}

ImageProcessing& ImageProcessing::load(const std::string& path,
                                       const LoadFunction& loadFunction) {
    _img = loadFunction(path);

    return *this;
}

ImageProcessing& ImageProcessing::save(const std::string& path, const SaveFunction& saveFunction,
                                       const std::string& ext, bool addTime) {
    saveFunction(path, _img, ext, addTime);

    return *this;
}

ImageProcessing& ImageProcessing::opts(std::initializer_list<OperationFunction> opts) {
    _img = std::accumulate(std::begin(opts), std::end(opts), _img,
                           [](const Img& img, const OperationFunction& opts) {
        return opts(img);
    });

    return *this;
}

ImageProcessing& ImageProcessing::filters(std::initializer_list<
                                          std::pair<borders::BorderTypes, FilterFunction>> filters) {
    _img = std::accumulate(std::begin(filters), std::end(filters), _img,
                           [](const Img& img,
                              const std::pair<borders::BorderTypes, FilterFunction>& filter) {
        return filter.second(img, filter.first);
    });

    return *this;
}

ImageProcessing& ImageProcessing::render(const std::string& windowName,
                                         const RenderFunction& renderFunction) {
    renderFunction(windowName, _img);

    return *this;
}

const Img& ImageProcessing::image() const {
    return _img;
}
