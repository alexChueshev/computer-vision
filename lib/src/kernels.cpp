#include "kernels.h"

using namespace pi;

kernels::Kernel::Kernel(int width, int height) {
    assert(width > 0 && height > 0);
    assert(width % 2 == 1 && height % 2 == 1);

    _width = width;
    _height = height;
}

int kernels::Kernel::width() const {
    return _width;
}

int kernels::Kernel::height() const {
    return _height;
}

kernels::SeparableKernel::SeparableKernel(const Array1d& mRow, const Array1d& mCol)
        : Kernel(mRow.size(), mCol.size()) {
    _mRow = mRow;
    _mCol = mCol;
}

kernels::SeparableKernel::SeparableKernel(Array1d&& mRow, Array1d&& mCol)
        : Kernel(mRow.size(), mCol.size()) {
    _mRow = std::move(mRow);
    _mCol = std::move(mCol);
}

Img kernels::SeparableKernel::apply(const Img& src, const borders::Function& fBorder) {
    assert(src.channels() == 1);

    auto cPosX = _width / 2,
            cPosY = _height / 2;

    Img dst(src.height(), src.width(), src.channels());
    Img tmp(src.height(), src.width(), src.channels());

    //apply mRow
    for (auto rI = 0, rEnd = src.height(); rI < rEnd; rI++) {
        for (auto cI = 0, cEnd = src.width(); cI < cEnd; cI++) {
            auto val = 0.f;

            for (auto kC = 0; kC < _width; kC++) {
                val += _mRow[kC] * fBorder(rI, cI + kC - cPosX, src);
            }
            *tmp.at(rI, cI) = val;
        }
    }

    //apply mCol
    for (auto rI = 0, rEnd = src.height(); rI < rEnd; rI++) {
        for (auto cI = 0, cEnd = src.width(); cI < cEnd; cI++) {
            auto val = 0.f;

            for (auto kR = 0; kR < _height; kR++) {
                val += _mCol[kR] * fBorder(rI + kR - cPosY, cI, tmp);
            }
            *dst.at(rI, cI) = val;
        }
    }

    return dst;
}
