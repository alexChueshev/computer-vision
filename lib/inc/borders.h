#ifndef COMPUTER_VISION_BORDERS_H
#define COMPUTER_VISION_BORDERS_H

#include <img.h>

namespace pi::borders {
    enum BorderTypes {
        BORDER_CONSTANT,
        BORDER_REPLICATE,
        BORDER_REFLECT,
        BORDER_WRAP
    };

    typedef std::function<float(int, int, const Img&)> Function;

    Function get(BorderTypes border);

    float constant(int row, int col, const Img& src);

    float replicate(int row, int col, const Img& src);

    float reflect(int row, int col, const Img& src);

    float wrap(int row, int col, const Img& src);
}

#endif //COMPUTER_VISION_BORDERS_H
