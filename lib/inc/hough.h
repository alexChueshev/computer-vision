#ifndef COMPUTER_VISION_HOUGH_H
#define COMPUTER_VISION_HOUGH_H

#include <transforms.h>

#include <map>

namespace pi::transforms::t_hough {
    struct Hough;

    typedef std::pair<detectors::SPoint, detectors::SPoint> PPairs;

    constexpr float SCALE_F = 2.f;
    constexpr float LOCATION_C = .25f;
    constexpr int ORIENT_BINS = 12;
}

namespace pi::transforms {
    t_hough::Hough hough(Size iSize, Size oSize, const std::vector<t_hough::PPairs>& pairs,
                         float scMin = 1.f / 8, int scLevels = 10,
                         float scFactor = t_hough::SCALE_F,
                         float lcCoeff = t_hough::LOCATION_C,
                         int orntBins = t_hough::ORIENT_BINS);
}

struct pi::transforms::t_hough::Hough {
    float x;
    float y;
    float angle;
    float scale;
};

#endif //COMPUTER_VISION_HOUGH_H
