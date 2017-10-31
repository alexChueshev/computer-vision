#include <kernels.h>

using namespace pi;

kernels::Kernel::Kernel(int height, int width) {
    assert(width > 0 && height > 0);

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

pi::kernels::Kernel kernels::Kernel::transpose() {
    Kernel transposed(_width, _height);
    auto* data = transposed.data();

    for (auto i = 0; i < _height; i++) {
        for (auto j = 0; j< _width; j++) {
           data[j * _height + i] = _data[i * _width + j];
        }
    }

    return transposed;
}

kernels::Kernel kernels::gaussian1d(float sigma, int size) {
    assert(sigma > 0);

    auto sum = 0.f;
    auto halfSize = size / 2;
    float div = std::sqrt(2 * M_PI) * sigma;

    kernels::Kernel gaussian(1, size);
    auto* data = gaussian.data();

    for (auto i = 0; i < size; i++) {
        auto ri = i - halfSize;
        auto val = std::exp(-ri * ri / (2 * sigma * sigma)) / div;

        sum += val;
        data[i] = val;
    }

    std::for_each(data, data + size, [&sum](float &elem) {
        elem /= sum;
    });

    return gaussian;
}

kernels::Kernel kernels::gaussian2d(float sigma, int size) {
    assert(sigma > 0);

    auto sum = 0.f;
    auto halfSize = size / 2;
    float div = 2 * M_PI * sigma * sigma;

    kernels::Kernel gaussian(size, size);
    auto* data = gaussian.data();

    for (auto i = 0; i < size; i++) {
        for(auto j = 0; j < size; j++) {
            auto ri = i - halfSize;
            auto rj = j - halfSize;
            auto val = std::exp(-(ri * ri + rj * rj) / (2 * sigma * sigma)) / div;

            sum += val;
            data[i * size + j] = val;
        }
    }

    std::for_each(data, data + size * size, [&sum](float &elem) {
        elem /= sum;
    });

    return gaussian;
}

std::pair<kernels::Kernel, kernels::Kernel> kernels::gaussian(float sigma) {
    auto size = 2 * (int)(sigma * 3) + 1;
    auto gaussian = gaussian1d(sigma, size);

    return std::pair<kernels::Kernel, kernels::Kernel>(
                gaussian,
                gaussian.transpose());
}

std::pair<kernels::Kernel, kernels::Kernel> kernels::gaussian(float sigma, int size) {
    auto gaussian = gaussian1d(sigma, size);

    return std::pair<kernels::Kernel, kernels::Kernel>(
                gaussian,
                gaussian.transpose());
}

std::pair<kernels::Kernel, kernels::Kernel> kernels::sobelX() {
    return std::pair<kernels::Kernel, kernels::Kernel>(
                kernels::Kernel(1, 3, new float[3]{1, 0, -1}),
                kernels::Kernel(3, 1, new float[3]{1, 2, 1}));
}

std::pair<kernels::Kernel, kernels::Kernel> kernels::sobelY() {
    return std::pair<kernels::Kernel, kernels::Kernel>(
                kernels::Kernel(1, 3, new float[3]{1, 2, 1}),
                kernels::Kernel(3, 1, new float[3]{1, 0, -1}));
}
