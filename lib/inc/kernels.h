#ifndef COMPUTER_VISION_KERNEL_H
#define COMPUTER_VISION_KERNEL_H

#include <borders.h>
#include <vector>

namespace pi::kernels {
    class Kernel;
}

class pi::kernels::Kernel {

protected:
    int _height;
    int _width;
    std::unique_ptr<float[]> _data;

public:
    Kernel(int height, int width);

    Kernel(int height, int width, const float* data);

    Kernel(const Kernel& kernel);

    Kernel(Kernel&& kernel) = default;

    Kernel& operator=(const Kernel& kernel);

    Kernel& operator=(Kernel&& kernel) = default;

    const float* data() const;

    float* data();

    const float* at(int row, int col) const;

    float* at(int row, int col);

    int width() const;

    int height() const;

    ~Kernel() = default;
};

#endif //COMPUTER_VISION_KERNEL_H
