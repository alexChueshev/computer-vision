#include "img.h"

using namespace pi;

Img::Img()
    : _width(0)
    , _height(0)
    , _channels(0)
    , _step(0)
    , _data(nullptr)
{
}

Img::Img(int height, int width, int channels)
    : _width(width)
    , _height(height)
    , _channels(channels)
    , _step(_width * _channels)
{
    _data = this->makeSharedArray(_height * _step);
}

Img::Img(const Img& img)
    : _width(img._width)
    , _height(img._height)
    , _channels(img._channels)
    , _step(img._step)
    , _data(img._data)
{
}

Img::Img(Img&& img)
    : Img()
{
    std::swap(_width, img._width);
    std::swap(_height, img._height);
    std::swap(_channels, img._channels);
    std::swap(_step, img._step);

    _data.swap(img._data);
}

Img& Img::operator=(const Img& img) {
    if(this != &img) {
        _width = img._width;
        _height = img._height;
        _channels = img._channels;
        _step = img._step;
        _data = img._data;
    }
    return *this;
}

Img& Img::operator=(Img&& img) {
    if(this != &img) {
        _width = 0;
        std::swap(_width, img._width);

        _height = 0;
        std::swap(_height, img._height);

        _channels = 0;
        std::swap(_channels, img._channels);

        _step = 0;
        std::swap(_step, img._step);

        _data = nullptr;
        img._data.swap(_data);
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

Img Img::clone() const {
    Img img(_height, _width, _channels);

    auto* src = _data.get();
    auto* dst = img.data();

    for(auto i = 0, size = _height * _step; i < size; i++) {
        dst[i] = src[i];
    }

    return img;
}

std::shared_ptr<float> Img::makeSharedArray(int size) {
    return std::shared_ptr<float>(new float[size], std::default_delete<float[]>());
}

