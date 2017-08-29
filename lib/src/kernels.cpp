#include "kernels.h"

pi::kernels::Kernel::Kernel(int width, int height) {
    assert(width > 0 && height > 0);
    assert(width % 2 == 1 && height % 2 == 1);

    this->width = width;
    this->height = height;
}

pi::kernels::SeparableKernel::SeparableKernel(const Array1d &mRow, const Array1d &mCol)
        : Kernel(mRow.size(), mCol.size()) {
    this->mRow = mRow;
    this->mCol = mCol;
}

void pi::kernels::SeparableKernel::apply(cv::Mat &src, const borders::Function &fBorder) {
    assert(src.type() == CV_32FC1);

    auto cPosX = this->width / 2,
            cPosY = this->height / 2;

    cv::Mat tmp(src.rows, src.cols, src.type());

    //apply mRow
    for (auto rI = 0, rEnd = src.rows; rI < rEnd; rI++) {
        for (auto cI = 0, cEnd = src.cols; cI < cEnd; cI++) {
            auto val = 0.f;

            for (auto kC = 0; kC < this->width; kC++) {
                val += this->mRow[kC] * fBorder(rI, cI + kC - cPosX, src);
            }
            tmp.at<float>(rI, cI) = val;
        }
    }

    //apply mCol
    for (auto rI = 0, rEnd = src.rows; rI < rEnd; rI++) {
        for (auto cI = 0, cEnd = src.cols; cI < cEnd; cI++) {
            auto val = 0.f;

            for (auto kR = 0; kR < this->height; kR++) {
                val += this->mCol[kR] * fBorder(rI + kR - cPosY, cI, tmp);
            }
            src.at<float>(rI, cI) = val;
        }
    }
}
