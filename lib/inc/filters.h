#ifndef COMPUTER_VISION_FILTERS_H
#define COMPUTER_VISION_FILTERS_H

#include <kernels.h>
#include <cmath>

namespace pi::filters {
    class Filter;

    class Gaussian;

    class Sobel;
}

class pi::filters::Filter {

public:
    virtual Img apply(const Img &src, borders::BorderTypes border) = 0;

    virtual ~Filter() = default;

protected:
    Img _convolve(const Img& src, const kernels::Kernel& kernel, const borders::Function& fBorder);
};

class pi::filters::Gaussian : public Filter {

protected:
    float _sigma;
    int _size;
    kernels::Kernel _kernelH;
    kernels::Kernel _kernelV;

public:
    explicit Gaussian(float sigma);

    Img apply(const Img& src, borders::BorderTypes border) override;

public:
    static std::vector<float> data1d(float sigma, int size);

    static std::vector<float> data2d(float sigma, int size);
};

class pi::filters::Sobel : public Filter {

protected:
    std::pair<kernels::Kernel, kernels::Kernel> _kernelX;
    std::pair<kernels::Kernel, kernels::Kernel> _kernelY;

public:
    explicit Sobel();

    Img apply(const Img& src, borders::BorderTypes border) override;

    Img applyX(const Img& src, borders::BorderTypes border);

    Img applyY(const Img& src, borders::BorderTypes border);
};

#endif //COMPUTER_VISION_FILTERS_H
