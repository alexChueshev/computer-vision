#include <filters.h>

using namespace pi;

Img filters::gaussian(const Img& src, float sigma, borders::BorderTypes border) {
    auto kernels = kernels::gaussian(sigma);
    return convolve(convolve(src, kernels.first, border), kernels.second, border);
}

Img filters::sobel(const Img& src, borders::BorderTypes border, const SobelFunction& op) {
    auto images = sobel(src, border);
    return op(images.first, images.second);
}

std::pair<Img, Img> filters::sobel(const Img &src, borders::BorderTypes border) {
    auto kernelX = kernels::sobelX();
    auto kernelY = kernels::sobelY();
    return std::pair<Img, Img>(
                convolve(convolve(src, kernelX.first, border), kernelX.second, border),
                convolve(convolve(src, kernelY.first, border), kernelY.second, border));
}

Img filters::magnitude(const Img& dx, const Img& dy) {
    assert(dx.channels() == 1);
    assert(dy.channels() == 1);

    Img dst(dx.height(), dx.width(), 1);

    auto* data = dst.data();
    auto* xData = dx.data();
    auto* yData = dy.data();

    for(auto i = 0, size = dst.dataSize(); i < size; i++) {
        data[i] = std::hypot(xData[i], yData[i]);
    }

    return dst;
}

Img filters::phi(const Img& dx, const Img& dy) {
    assert(dx.channels() == 1);
    assert(dy.channels() == 1);

    Img dst(dx.height(), dx.width(), 1);

    auto* data = dst.data();
    auto* xData = dx.data();
    auto* yData = dy.data();

    for(auto i = 0, size = dst.dataSize(); i < size; i++) {
        data[i] = std::atan2(yData[i], xData[i]);
    }

    return dst;
}

Img filters::convolve(const Img& src, const kernels::Kernel& kernel, borders::BorderTypes border) {
    assert(src.channels() == 1);

    auto fBorder = borders::Factory::get(border);
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
