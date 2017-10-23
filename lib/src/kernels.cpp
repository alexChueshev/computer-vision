#include "kernels.h"

using namespace pi;

kernels::Kernel::Kernel(int height, int width) {
    assert(width > 0 && height > 0);
    assert(width % 2 == 1 && height % 2 == 1);

    _width = width;
    _height = height;
    _data = std::make_unique<float[]>(_width * _height);
}

kernels::Kernel::Kernel(int height, int width, const float *data)
    : kernels::Kernel(height, width) {
    std::copy(data, data + _height * _width, _data.get());
}

kernels::Kernel::Kernel(const Kernel& kernel)
    : _height(kernel._height)
    , _width(kernel._width)
    , _data(std::make_unique<float[]>(_height * _width))
{
    std::copy(kernel._data.get(), kernel._data.get() + _width * _height, _data.get());
}

kernels::Kernel& kernels::Kernel::operator=(const Kernel& kernel) {
    if(this != &kernel) {
        _height = kernel._height;
        _width = kernel._width;

        _data = std::make_unique<float[]>(_height * _width);
        std::copy(kernel._data.get(), kernel._data.get() + _width * _height, _data.get());
    }
    return *this;
}

const float* kernels::Kernel::data() const {
    return _data.get();
}

float* kernels::Kernel::data() {
    return _data.get();
}

const float* kernels::Kernel::at(int row, int col) const {
    assert(0 <= row && row < _height);
    assert(0 <= col && col < _width);

    return _data.get() + _width * row + col;
}

float* kernels::Kernel::at(int row, int col) {
    assert(0 <= row && row < _height);
    assert(0 <= col && col < _width);

    return _data.get() + _width * row + col;
}

int kernels::Kernel::width() const {
    return _width;
}

int kernels::Kernel::height() const {
    return _height;
}
