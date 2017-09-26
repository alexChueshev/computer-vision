#include "kernels.h"

using namespace pi;

kernels::Kernel::Kernel(int width, int height) {
    assert(width > 0 && height > 0);
    assert(width % 2 == 1 && height % 2 == 1);

    this->width = width;
    this->height = height;
}

kernels::SeparableKernel::SeparableKernel(const Array1d& mRow, const Array1d& mCol)
        : Kernel(mRow.size(), mCol.size()) {
    this->mRow = mRow;
    this->mCol = mCol;
}

kernels::SeparableKernel::SeparableKernel(Array1d&& mRow, Array1d&& mCol)
        : Kernel(mRow.size(), mCol.size()) {
    this->mRow = std::move(mRow);
    this->mCol = std::move(mCol);
}

void kernels::SeparableKernel::apply(Img& src, const borders::Function& fBorder) {
    assert(src.channels() == 1);

    auto cPosX = this->width / 2,
            cPosY = this->height / 2;

    Img tmp(src.height(), src.width(), src.channels());

    //apply mRow
    for (auto rI = 0, rEnd = src.height(); rI < rEnd; rI++) {
        for (auto cI = 0, cEnd = src.width(); cI < cEnd; cI++) {
            auto val = 0.f;

            for (auto kC = 0; kC < this->width; kC++) {
                val += this->mRow[kC] * fBorder(rI, cI + kC - cPosX, src);
            }
            *tmp.at(rI, cI) = val;
        }
    }

    //apply mCol
    for (auto rI = 0, rEnd = src.height(); rI < rEnd; rI++) {
        for (auto cI = 0, cEnd = src.width(); cI < cEnd; cI++) {
            auto val = 0.f;

            for (auto kR = 0; kR < this->height; kR++) {
                val += this->mCol[kR] * fBorder(rI + kR - cPosY, cI, tmp);
            }
            *src.at(rI, cI) = val;
        }
    }
}
