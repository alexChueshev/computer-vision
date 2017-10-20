#include "filters.h"

using namespace pi;

/***********************
 * Gaussian methods
 ***********************/
filters::Gaussian::Gaussian(float sigma)
        : _sigma(sigma) {
    assert(sigma > 0);

    std::vector<float> coefficients = data1d(_sigma, (2 * (int)(sigma * 3) + 1));

    _kernel = std::make_unique<kernels::SeparableKernel>(coefficients,coefficients);
}

void filters::Gaussian::apply(Img& src, borders::BorderTypes border) {
    assert(src.channels() == 1);

    _kernel->apply(src, borders::Factory::get(border));
}

std::vector<float> filters::Gaussian::data1d(float sigma, int size) {
    assert(sigma > 0);
    assert(size % 2 == 1);

    auto sum = 0.f;
    auto halfSize = size / 2;
    float div = std::sqrt(2 * M_PI) * sigma;
    std::vector<float> coefficients(size);

    for (auto i = -halfSize; i <= halfSize; i++) {
        auto val = std::exp(-i * i / (2 * sigma * sigma)) / div;

        sum += val;
        coefficients[i + halfSize] = val;
    }

    for (auto &coefficient : coefficients) {
        coefficient /= sum;
    }

    return coefficients;
}

std::vector<float> filters::Gaussian::data2d(float sigma, int size) {
    assert(sigma > 0);
    assert(size % 2 == 1);

    auto sum = 0.f;
    auto halfSize = size / 2;
    float div = 2 * M_PI * sigma * sigma;
    std::vector<float> coefficients(size * size);

    for (auto i = -halfSize; i <= halfSize; i++) {
        for(auto j = -halfSize; j <= halfSize; j++) {
            auto val = std::exp(-(i * i + j * j) / (2 * sigma * sigma)) / div;

            sum += val;
            coefficients[(i + halfSize) * size + (j + halfSize)] = val;
        }
    }

    for (auto &coefficient : coefficients) {
        coefficient /= sum;
    }

    return coefficients;
}

/***********************
 * Sobel methods
 ***********************/
filters::Sobel::Sobel()
    : _kernelX(new kernels::SeparableKernel({1, 0, -1}, {1, 2, 1})),
      _kernelY(new kernels::SeparableKernel({1, 2, 1}, {1, 0, -1})) {
}

void filters::Sobel::apply(Img& src, borders::BorderTypes border) {
    assert(src.channels() == 1);

    auto fBorder = borders::Factory::get(border);

    Img xSrc = src;
    Img ySrc = src;

    _kernelX->apply(xSrc, fBorder);
    _kernelY->apply(ySrc, fBorder);

    auto* data = src.data();
    auto* xData = xSrc.data();
    auto* yData = ySrc.data();

    for(auto i = 0, size = src.dataSize(); i < size; i++) {
        data[i] = std::hypot(xData[i], yData[i]);
    }
}

void filters::Sobel::applyX(Img& src, borders::BorderTypes border) {
    auto fBorder = borders::Factory::get(border);
    _kernelX->apply(src, fBorder);
}

void filters::Sobel::applyY(Img& src, borders::BorderTypes border) {
    auto fBorder = borders::Factory::get(border);
    _kernelY->apply(src, fBorder);
}

