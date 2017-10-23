#include "filters.h"

using namespace pi;

/***********************
 * Filter methods
 ***********************/
Img filters::Filter::_convolve(const Img& src, const kernels::Kernel& kernel,
                               const borders::Function& fBorder)
{
    assert(src.channels() == 1);

    auto cPosX = kernel.width() / 2, cPosY = kernel.height() / 2;
    Img tmp(src.height(), src.width(), src.channels());

    for (auto rI = 0, rEnd = src.height(); rI < rEnd; rI++) {
        for (auto cI = 0, cEnd = src.width(); cI < cEnd; cI++) {
            auto val = 0.f;

            for (auto kR = 0, kREnd = kernel.height(); kR < kREnd; kR++) {
                for(auto kC = 0, kCEnd = kernel.width(); kC < kCEnd; kC++) {
                    auto r = rI + kR - cPosY,
                         c = cI + kC - cPosX;

                    val += *kernel.at(kR, kC) * fBorder(r, c, src);
                }
            }
            *tmp.at(rI, cI) = val;
        }
    }

    return tmp;
}

/***********************
 * Gaussian methods
 ***********************/
filters::Gaussian::Gaussian(float sigma)
    : _sigma(sigma)
    , _size(2 * (int)(sigma * 3) + 1)
    , _kernelH(1, _size)
    , _kernelV(_size, 1)
{
    assert(sigma > 0);

    std::vector<float> coefficients = data1d(_sigma, _size);
    std::copy(std::begin(coefficients), std::end(coefficients), _kernelH.data());
    std::copy(std::begin(coefficients), std::end(coefficients), _kernelV.data());
}

Img filters::Gaussian::apply(const Img& src, borders::BorderTypes border) {
    assert(src.channels() == 1);

    auto fBorder = borders::Factory::get(border);

    return _convolve(_convolve(src, _kernelH, fBorder), _kernelV, fBorder);
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
    : _kernelX(kernels::Kernel(1, 3, new float[3]{1, 0, -1}),
               kernels::Kernel(3, 1, new float[3]{1, 2, 1}))
    , _kernelY(kernels::Kernel(1, 3, new float[3]{1, 2, 1}),
               kernels::Kernel(3, 1, new float[3]{1, 0, -1}))
{
}

Img filters::Sobel::apply(const Img& src, borders::BorderTypes border) {
    assert(src.channels() == 1);

    auto fBorder = borders::Factory::get(border);

    Img dst(src.height(), src.width(), src.channels());
    Img xSrc = _convolve(_convolve(src, _kernelX.first, fBorder), _kernelX.second, fBorder);
    Img ySrc = _convolve(_convolve(src, _kernelY.first, fBorder), _kernelY.second, fBorder);

    auto* data = dst.data();
    auto* xData = xSrc.data();
    auto* yData = ySrc.data();

    for(auto i = 0, size = src.dataSize(); i < size; i++) {
        data[i] = std::hypot(xData[i], yData[i]);
    }

    return dst;
}

Img filters::Sobel::applyX(const Img& src, borders::BorderTypes border) {
    auto fBorder = borders::Factory::get(border);
    return _convolve(_convolve(src, _kernelX.first, fBorder), _kernelX.second, fBorder);
}

Img filters::Sobel::applyY(const Img& src, borders::BorderTypes border) {
    auto fBorder = borders::Factory::get(border);
    return _convolve(_convolve(src, _kernelY.first, fBorder), _kernelY.second, fBorder);
}
