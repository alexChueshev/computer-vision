#ifndef COMPUTER_VISION_IMG_H
#define COMPUTER_VISION_IMG_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <memory>

class Img
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

    Img(const std::string &path);

    Img(const Img& img);

    Img(Img&& img);

    Img& operator=(const Img& img);

    Img& operator=(Img&& img);

    float* data();

    float* ptr(int row);

    float* at(int row, int col);

    bool isContinuous();

    int width() const;

    int height() const;

    int channels() const;

    Img clone();

    ~Img() = default;
};

#endif // COMPUTER_VISION_IMG_H
