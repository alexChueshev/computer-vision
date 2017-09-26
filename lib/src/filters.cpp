#include "filters.h"

using namespace pi;

filters::Gaussian::Gaussian(float sigma)
        : sigma(sigma) {
    assert(sigma > 0);

    auto sum = 0.;
    auto hSize = (int) (sigma * 3);
    auto div = sqrt(2 * M_PI) * sigma;

    std::vector<double> coefficients(2 * hSize + 1);

    for (auto i = -hSize; i <= hSize; i++) {
        auto val = std::exp(-i * i / (2 * sigma * sigma)) / div;

        sum += val;
        coefficients[i + hSize] = val;
    }

    for (auto &coefficient : coefficients) {
        coefficient /= sum;
    }

    this->kernel = std::make_unique<kernels::SeparableKernel>(coefficients,coefficients);
}

void filters::Gaussian::apply(Img& src, borders::BorderTypes border) {
    assert(src.channels() == 1);

    this->kernel->apply(src, borders::Factory::get(border));
}

filters::Sobel::Sobel()
    : kernelX(new kernels::SeparableKernel({1, 0, -1}, {1, 2, 1})),
      kernelY(new kernels::SeparableKernel({1, 2, 1}, {1, 0, -1})) {
}

void filters::Sobel::apply(Img& src, borders::BorderTypes border) {
    assert(src.channels() == 1);

    auto fBorder = borders::Factory::get(border);

    Img xSrc = src.clone();
    Img ySrc = src.clone();

    this->kernelX->apply(xSrc, fBorder);
    this->kernelY->apply(ySrc, fBorder);

    auto* data = src.data();
    auto* xData = xSrc.data();
    auto* yData = ySrc.data();

    for(auto i = 0, size = src.dataSize(); i < size; i++) {
        data[i] = std::sqrt(xData[i] * xData[i] + yData[i] * yData[i]);
    }
}
