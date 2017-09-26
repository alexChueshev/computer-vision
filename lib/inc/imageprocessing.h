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
    typedef std::function<void(std::string, const Img&)> SaveFunction;
    typedef std::function<void(std::string, const Img&)> RenderFunction;

protected:
    Img _img;

public:
    explicit ImageProcessing(const Img& img);

    explicit ImageProcessing(Img&& img);

    ImageProcessing(const std::string& path, const LoadFunction& loadFuntion);

    ImageProcessing& load(const std::string& path, const LoadFunction& loadFuntion);

    ImageProcessing& opts(std::initializer_list<OperationFunction> opts);

    ImageProcessing& filters(std::initializer_list<
                             std::pair<borders::BorderTypes, FilterFunction>> filters);

    ImageProcessing& render(const std::string& windowName, const RenderFunction& renderFunction);

    ImageProcessing clone();

    Img image() const;
};

#endif // COMPUTER_VISION_IMAGEPROCESSING_H
