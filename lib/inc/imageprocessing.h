#ifndef COMPUTER_VISION_IMAGEPROCESSING_H
#define COMPUTER_VISION_IMAGEPROCESSING_H

#include <filters.h>

namespace pi {
    class ImageProcessing;
}

class pi::ImageProcessing
{

public:
    typedef std::function<Img(std::string)> LoadFunction;
    typedef std::function<void(Img&)> OperationFunction;
    typedef std::function<void(Img&, borders::BorderTypes)> FilterFunction;
    typedef std::function<void(std::string, const Img&, std::string, bool)> SaveFunction;
    typedef std::function<void(std::string, const Img&)> RenderFunction;

protected:
    Img _img;

public:
    ImageProcessing() = default;

    explicit ImageProcessing(const Img& img);

    explicit ImageProcessing(Img&& img);

    ImageProcessing(const std::string& path, const LoadFunction& loadFuntion);

    ImageProcessing& load(const std::string& path, const LoadFunction& loadFuntion);

    ImageProcessing& save(const std::string& path, const SaveFunction& saveFunction,
                          const std::string& ext="png", bool addTime=true);

    ImageProcessing& opts(std::initializer_list<OperationFunction> opts);

    ImageProcessing& filters(std::initializer_list<
                             std::pair<borders::BorderTypes, FilterFunction>> filters);

    ImageProcessing& render(const std::string& windowName, const RenderFunction& renderFunction);

    ImageProcessing& apply(const Img& img);

    ImageProcessing& apply(Img&& img);

    ImageProcessing clone() const;

    Img image() const;
};

#endif // COMPUTER_VISION_IMAGEPROCESSING_H
