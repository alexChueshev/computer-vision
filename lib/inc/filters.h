#ifndef COMPUTER_VISION_FILTERS_H
#define COMPUTER_VISION_FILTERS_H

#include <kernels.h>
#include <cmath>

namespace pi {
namespace filters {
    class Filter;

    class Gaussian;

    class Sobel;
}}

class pi::filters::Filter {

public:
    virtual void apply(Img &src, borders::BorderTypes border) = 0;
};

class pi::filters::Gaussian : public Filter {

protected:
    float sigma;
    std::unique_ptr<kernels::Kernel> kernel;

public:
    explicit Gaussian(float sigma);

    void apply(Img& src, borders::BorderTypes border) override;
};

class pi::filters::Sobel : public Filter {

protected:
    std::unique_ptr<kernels::Kernel> kernelX;
    std::unique_ptr<kernels::Kernel> kernelY;

public:
    explicit Sobel();

    void apply(Img& src, borders::BorderTypes border) override;
};

#endif //COMPUTER_VISION_FILTERS_H
