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

    class Factory;

    typedef std::function<float(int, int, const cv::Mat&)> Function;

    float constant(int row, int col, const cv::Mat &src);

    float replicate(int row, int col, const cv::Mat &src);

    float reflect(int row, int col, const cv::Mat &src);

    float wrap(int row, int col, const cv::Mat &src);
}

class pi::borders::Factory {

public:
    static Function get(pi::borders::BorderTypes border);
};

#endif //COMPUTER_VISION_BORDERS_H
