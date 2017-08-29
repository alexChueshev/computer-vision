#ifndef COMPUTER_VISION_KERNEL_H
#define COMPUTER_VISION_KERNEL_H

#include <opencv2/core.hpp>

#include <borders.h>

#include <memory>
#include <vector>

namespace pi::kernels {
    class Kernel;

    class SeparableKernel;
}

class pi::kernels::Kernel {

public:
    typedef std::vector<double> Array1d;

protected:
    int width;
    int height;

public:
    Kernel(int width, int height);

    virtual void apply(cv::Mat &src, const borders::Function &fBorder) = 0;
};


class pi::kernels::SeparableKernel : public Kernel {

private:
    Array1d mRow;
    Array1d mCol;

public:
    SeparableKernel(const Array1d &mRow, const Array1d &mCol);

    void apply(cv::Mat &src, const borders::Function &fBorder) override;
};

#endif //COMPUTER_VISION_KERNEL_H
