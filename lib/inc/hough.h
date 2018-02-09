#ifndef COMPUTER_VISION_HOUGH_H
#define COMPUTER_VISION_HOUGH_H

#include <transforms.tpp>

#include <map>

namespace pi::transforms::t_hough {
    constexpr float SCALE_MIN = 1.f / 4;
    constexpr float SCALE_F = 2.f;
    constexpr float LOCATION_C = .25f;
    constexpr int ORIENT_BINS = 12;
}

namespace pi::transforms {
    using SPPairs = PPairs<detectors::SPoint>;

    Hypotheses<detectors::SPoint> hough(Size iSize, Size oSize, const std::vector<SPPairs>& pairs,
                                        float scMin = t_hough::SCALE_MIN, int scLevels = 12,
                                        float scFactor = t_hough::SCALE_F,
                                        float lcCoeff = t_hough::LOCATION_C,
                                        int orntBins = t_hough::ORIENT_BINS);

    std::pair<Hypothesis<detectors::SPoint>, float> verify(const Hypotheses<detectors::SPoint>& hypotheses,
                                                    int matches, float threshold);
}

#endif //COMPUTER_VISION_HOUGH_H
