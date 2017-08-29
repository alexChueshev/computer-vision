#include "kernels.h"

pi::kernels::Kernel::Kernel(int width, int height) {
    assert(width <= 0 || height <= 0);
    assert(width % 2 == 0 || height % 2 == 0);

    this->width = width;
    this->height = height;
}

pi::kernels::SeparableKernel::SeparableKernel(const Array1d &mRow, const Array1d &mCol)
        : Kernel(mRow.size(), mCol.size()) {
    this->mRow = mRow;
    this->mCol = mCol;
}

void pi::kernels::SeparableKernel::apply(cv::Mat &bordered) {
    assert(bordered.type() == CV_32FC1);

    auto cPosX = this->width / 2,
            cPosY = this->height / 2;

    auto bRows = bordered.rows;
    auto rowPts = std::make_unique<float *[]>(bRows);

    //collect ptrs to the rows of bordered image
    for (auto rI = 0; rI < bRows; rI++) {
        rowPts[rI] = bordered.ptr<float>(rI);
    }

    //apply mRow
    for (auto rI = cPosY, rEnd = bordered.rows - cPosY; rI < rEnd; rI++) {
        for (auto cI = cPosX, cEnd = bordered.cols - cPosX; cI < cEnd; cI++) {
            auto val = 0.f;
            for(auto kC = 0; kC < this->width; kC++) {
                val += this->mRow[kC] * rowPts[rI][cI + kC - cPosX];
            }
            rowPts[rI][cI] = val;
        }
    }

    //apply mCol
    for (auto rI = cPosY, rEnd = bordered.rows - cPosY; rI < rEnd; rI++) {
        for (auto cI = cPosX, cEnd = bordered.cols - cPosX; cI < cEnd; cI++) {
            auto val = 0.f;
            for(auto kR = 0; kR < this->height; kR++) {
                val += this->mCol[kR] * rowPts[rI + kR - cPosY][cI];
            }
            rowPts[rI][cI] = val;
        }
    }
}


