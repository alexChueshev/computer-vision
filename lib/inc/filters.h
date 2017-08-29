#ifndef COMPUTER_VISION_FILTERS_H
#define COMPUTER_VISION_FILTERS_H

#include <borders.h>
#include <kernels.h>

namespace pi::filters {
    class Filter;

    class Gaussian;

    class Sobel;
}

class pi::filters::Filter {

public:
    virtual void apply(cv::Mat &src, pi::borders::BorderTypes border) = 0;
};

class pi::filters::Gaussian : public Filter {

protected:
    float sigma;
    std::unique_ptr<pi::kernels::Kernel> kernel;

public:
    explicit Gaussian(float sigma);

    void apply(cv::Mat &src, pi::borders::BorderTypes border) override;
};

class pi::filters::Sobel : public Filter {

protected:
    std::unique_ptr<pi::kernels::Kernel> kernelX;
    std::unique_ptr<pi::kernels::Kernel> kernelY;

public:
    Sobel();

    void apply(cv::Mat &src, pi::borders::BorderTypes border) override;
};

#endif //COMPUTER_VISION_FILTERS_H
