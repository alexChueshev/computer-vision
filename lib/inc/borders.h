#ifndef COMPUTER_VISION_BORDERS_H
#define COMPUTER_VISION_BORDERS_H

#include <opencv2/core.hpp>

namespace pi::borders {
    enum BorderTypes {
        BORDER_CONSTANT,
        BORDER_REPLICATE,
        BORDER_REFLECT,
        BORDER_WRAP
    };

    int constant(int row, int col, const cv::Mat &src);

    int replicate(int row, int col, const cv::Mat &src);

    int reflect(int row, int col, const cv::Mat &src);

    int wrap(int row, int col, const cv::Mat &src);
}

#endif //COMPUTER_VISION_BORDERS_H
