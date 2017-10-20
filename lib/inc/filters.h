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
    virtual void apply(Img &src, borders::BorderTypes border) = 0;

    virtual ~Filter() = default;
};

class pi::filters::Gaussian : public Filter {

protected:
    float _sigma;
    std::unique_ptr<kernels::Kernel> _kernel;

public:
    explicit Gaussian(float sigma);

    void apply(Img& src, borders::BorderTypes border) override;

public:
    static std::vector<float> data1d(float sigma, int size);

    static std::vector<float> data2d(float sigma, int size);
};

class pi::filters::Sobel : public Filter {

protected:
    std::unique_ptr<kernels::Kernel> _kernelX;
    std::unique_ptr<kernels::Kernel> _kernelY;

public:
    explicit Sobel();

    void apply(Img& src, borders::BorderTypes border) override;

    void applyX(Img& src, borders::BorderTypes border);

    void applyY(Img& src, borders::BorderTypes border);
};

#endif //COMPUTER_VISION_FILTERS_H
