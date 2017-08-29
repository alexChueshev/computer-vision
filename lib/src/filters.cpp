#include "filters.h"

pi::filters::Gaussian::Gaussian(float sigma)
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

    this->kernel = std::make_unique<pi::kernels::SeparableKernel>(
            coefficients,
            coefficients
    );
}

void pi::filters::Gaussian::apply(cv::Mat &src, pi::borders::BorderTypes border) {
    assert(src.type() == CV_32FC1);

    this->kernel->apply(src, pi::borders::Factory::get(border));
}

pi::filters::Sobel::Sobel()
        : kernelX(new pi::kernels::SeparableKernel({1, 0, -1}, {1, 2, 1})),
          kernelY(new pi::kernels::SeparableKernel({1, 2, 1}, {1, 0, -1})) {
}

void pi::filters::Sobel::apply(cv::Mat &src, pi::borders::BorderTypes border) {
    assert(src.type() == CV_32FC1);

    auto fBorder = pi::borders::Factory::get(border);

    cv::Mat xSrc = src.clone();
    cv::Mat ySrc = src.clone();

    this->kernelX->apply(xSrc, fBorder);
    this->kernelY->apply(ySrc, fBorder);

    for (auto it = src.begin<float>(), end = src.end<float>(),
                 xSrcIt = xSrc.begin<float>(), ySrcIt = ySrc.begin<float>();
         it != end; ++it, ++xSrcIt, ++ySrcIt) {

        *it = std::sqrt((*xSrcIt) * (*xSrcIt) + (*ySrcIt) * (*ySrcIt));
    }
}