#include <img.h>

using namespace pi;

Img::Img(int height, int width, int channels)
    : _width(width)
    , _height(height)
    , _channels(channels)
    , _step(_width * _channels)
    , _data(std::make_unique<float[]>(_height * _step))
{
}

Img::Img(const Img& img)
    : _width(img._width)
    , _height(img._height)
    , _channels(img._channels)
    , _step(img._step)
    , _data(std::make_unique<float[]>(_height * _step))
{
    std::copy(img._data.get(), img._data.get() + img.dataSize(), _data.get());
}

Img& Img::operator=(const Img& img) {
    if(this != &img) {
        _width = img._width;
        _height = img._height;
        _channels = img._channels;
        _step = img._step;

        _data = std::make_unique<float[]>(_height * _step);
        std::copy(img._data.get(), img._data.get() + img.dataSize(), _data.get());
    }
    return *this;
}

const float* Img::data() const {
    return _data.get();
}

float* Img::data() {
    return _data.get();
}

const float *Img::ptr(int row) const {
    assert(0 <= row && row < _height);

    return _data.get() + _step * row;
}

float* Img::ptr(int row) {
    assert(0 <= row && row < _height);

    return _data.get() + _step * row;
}

const float *Img::at(int row, int col) const {
    assert(0 <= row && row < _height);
    assert(0 <= col && col < _width);

    return _data.get() + _step * row + _channels * col;
}

float* Img::at(int row, int col) {
    assert(0 <= row && row < _height);
    assert(0 <= col && col < _width);

    return _data.get() + _step * row + _channels * col;
}

bool Img::isContinuous() const {
    return true;
}

int Img::width() const {
    return _width;
}

int Img::height() const {
    return _height;
}

int Img::channels() const {
    return _channels;
}

int Img::step() const {
    return _step;
}

int Img::imageSize() const {
    return _height * _width;
}

int Img::dataSize() const {
    return _height * _step;
}
