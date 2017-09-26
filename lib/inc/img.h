#ifndef COMPUTER_VISION_IMG_H
#define COMPUTER_VISION_IMG_H

#include <memory>
#include <cassert>

namespace pi {
    class Img;
}

class pi::Img
{

protected:
    int _width;
    int _height;
    int _channels;
    int _step;
    std::shared_ptr<float> _data;

private:
    Img();

    std::shared_ptr<float> makeSharedArray(int size);

public:
    Img(int height, int width, int channels);

    Img(const Img& img);

    Img(Img&& img);

    Img& operator=(const Img& img);

    Img& operator=(Img&& img);

    const float* data() const;

    float* data();

    float* ptr(int row);

    float* at(int row, int col);

    bool isContinuous();

    int width() const;

    int height() const;

    int channels() const;

    int step() const;

    int imageSize() const;

    int dataSize() const;

    Img clone();

    virtual ~Img() = default;
};

#endif // COMPUTER_VISION_IMG_H
