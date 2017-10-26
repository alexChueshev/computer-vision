#ifndef COMPUTER_VISION_FILTERS_H
#define COMPUTER_VISION_FILTERS_H

#include <kernels.h>
#include <cmath>
#include <vector>

namespace pi::filters {
    class Filter;

    class Gaussian;

    class Sobel;

    Img gaussian(const Img& src, float sigma, borders::BorderTypes border);

    Img sobel(const Img& src, borders::BorderTypes border,
              const std::function<Img(const Img&, const Img&)>& op);

    std::pair<Img, Img> sobel(const Img& src, borders::BorderTypes border);

    Img magnitude(const Img& dx, const Img& dy);

    Img phi(const Img& dx, const Img& dy);

    Img convolve(const Img& src, const kernels::Kernel& kernel, borders::BorderTypes border);
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
