#include <borders.h>

using namespace pi;

borders::Function borders::get(borders::BorderTypes border) {
    switch (border) {
        case BORDER_REPLICATE:
            return replicate;
        case BORDER_REFLECT:
            return reflect;
        case BORDER_WRAP:
            return wrap;
        case BORDER_CONSTANT:
        default:
            return constant;
    }
}

float borders::constant(int row, int col, const Img& src) {
    assert(src.channels() == 1);

    auto height = src.height(), width = src.width();

    if (row >= 0 && row < height && col >= 0 && col < width)
        return *src.at(row, col);

    return 0;
}

float borders::replicate(int row, int col, const Img& src) {
    assert(src.channels() == 1);

    std::function<int(int, int)> range = [](int dimension, int pos){
        if(pos < 0) return 0;
        if(pos >= dimension) return dimension - 1;
        return pos;
    };

    auto height = src.height(), width = src.width();
    auto nRow = range(height, row), nCol = range(width, col);

    return *src.at(nRow,nCol);
}

float borders::reflect(int row, int col, const Img& src) {
    assert(src.channels() == 1);

    std::function<int(int, int)> range = [](int dimension, int pos){
        if(pos < 0) return -pos;
        if(pos >= dimension) return 2 * dimension - pos - 2;
        return pos;
    };

    auto height = src.height(), width = src.width();
    auto nRow = range(height, row), nCol = range(width, col);

    return *src.at(nRow,nCol);
}

float borders::wrap(int row, int col, const Img& src) {
    assert(src.channels() == 1);

    auto height = src.height(), width = src.width();
    auto nRow = (height + row) % height, nCol = (width + col) % width;

    return *src.at(nRow,nCol);
}
