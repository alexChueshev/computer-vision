#include "borders.h"

pi::borders::Function pi::borders::Factory::get(pi::borders::BorderTypes border) {
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

float pi::borders::constant(int row, int col, const cv::Mat &src) {
    assert(src.type() == CV_32FC1);

    auto height = src.rows, width = src.cols;

    if (row >= 0 && row < height && col >= 0 && col < width)
        return src.at<float>(row, col);

    return 0;
}

float pi::borders::replicate(int row, int col, const cv::Mat &src) {
    assert(src.type() == CV_32FC1);

    std::function<int(int, int)> range = [](int dimension, int pos){
        if(pos < 0) return 0;
        if(pos >= dimension) return dimension - 1;
        return pos;
    };

    auto height = src.rows, width = src.cols;
    auto nRow = range(height, row), nCol = range(width, col);

    return src.at<float>(nRow,nCol);
}

float pi::borders::reflect(int row, int col, const cv::Mat &src) {
    assert(src.type() == CV_32FC1);

    std::function<int(int, int)> range = [](int dimension, int pos){
        if(pos < 0) return -pos;
        if(pos >= dimension) return 2 * dimension - pos - 2;
        return pos;
    };

    auto height = src.rows, width = src.cols;
    auto nRow = range(height, row), nCol = range(width, col);

    return src.at<float>(nRow,nCol);
}

float pi::borders::wrap(int row, int col, const cv::Mat &src) {
    assert(src.type() == CV_32FC1);

    auto height = src.rows, width = src.cols;
    auto nRow = (height + row) % height, nCol = (width + col) % width;

    return src.at<float>(nRow,nCol);
}
