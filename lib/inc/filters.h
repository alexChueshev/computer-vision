#ifndef COMPUTER_VISION_FILTERS_H
#define COMPUTER_VISION_FILTERS_H

#include <kernels.h>

namespace pi::filters {
    typedef std::function<Img(const Img&, const Img&)> SobelFunction;

    Img gaussian(const Img& src, float sigma, borders::BorderTypes border);

    Img sobel(const Img& src, borders::BorderTypes border, const SobelFunction& op);

    std::pair<Img, Img> sobel(const Img& src, borders::BorderTypes border);

    Img magnitude(const Img& dx, const Img& dy);

    Img phi(const Img& dx, const Img& dy);

    Img convolve(const Img& src, const kernels::Kernel& kernel, borders::BorderTypes border);
}

#endif //COMPUTER_VISION_FILTERS_H
