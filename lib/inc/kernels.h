#ifndef COMPUTER_VISION_KERNEL_H
#define COMPUTER_VISION_KERNEL_H

#include <borders.h>
#include <vector>

namespace pi::kernels {
    class Kernel;

    class SeparableKernel;
}

class pi::kernels::Kernel {

public:
    typedef std::vector<float> Array1d;

protected:
    int _width;
    int _height;

public:
    Kernel(int width, int height);

    virtual Img apply(const Img& src, const borders::Function& fBorder) = 0;

    int width() const;

    int height() const;

    virtual ~Kernel() = default;
};


class pi::kernels::SeparableKernel : public Kernel {

private:
    Array1d _mRow;
    Array1d _mCol;

public:
    SeparableKernel(const Array1d& mRow, const Array1d& mCol);

    SeparableKernel(Array1d&& mRow, Array1d&& mCol);

    Img apply(const Img& src, const borders::Function& fBorder) override;
};

#endif //COMPUTER_VISION_KERNEL_H
