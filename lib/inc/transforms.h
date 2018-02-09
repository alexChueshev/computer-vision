#ifndef COMPUTER_VISION_TRANSFORMS_H
#define COMPUTER_VISION_TRANSFORMS_H

#include <detectors.h>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

#include <random>
#include <map>

namespace pi::transforms::t_homography {
    constexpr float H_THRESHOLD = 4.5f;
    constexpr int H_ITERS = 1200;
}

namespace pi::transforms::t_hough {
    constexpr float SCALE_MIN = 1.f / 4;
    constexpr float SCALE_F = 2.f;
    constexpr float LOCATION_C = .25f;
    constexpr int ORIENT_BINS = 12;
}

namespace pi::transforms {
    constexpr int T_SIZE = 3;

    typedef std::array<float, T_SIZE> Transform1d;
    typedef std::array<Transform1d, T_SIZE> Transform2d;

    template<typename T>
    using PPairs = std::pair<T, T>;
    using SPPairs = PPairs<detectors::SPoint>;

    template<typename T>
    using Hypothesis = std::pair<Transform2d, std::vector<PPairs<T>>>;

    template<typename T>
    using Hypotheses = std::vector<Hypothesis<T>>;

    template<typename T>
    Transform2d dltHomography(const std::vector<PPairs<T>>& pairs);

    template<typename T>
    Transform2d dltAffine(const std::vector<PPairs<T>>& pairs);

    template<typename T>
    std::vector<PPairs<T>> inliers(const Transform2d& h, const std::vector<PPairs<T>>& matches,
                                   float threshold);

    template<typename T>
    std::pair<Hypothesis<T>, float> verify(const Hypotheses<T>& hypotheses, int matches, float threshold);

    template<typename T>
    Transform2d homography(const std::vector<PPairs<T>>& matches,
                           float threshold = t_homography::H_THRESHOLD,
                           int iters = t_homography::H_ITERS);

    Hypotheses<detectors::SPoint> hough(Size iSize, Size oSize, const std::vector<SPPairs>& pairs,
                                        float scMin = t_hough::SCALE_MIN, int scLevels = 12,
                                        float scFactor = t_hough::SCALE_F,
                                        float lcCoeff = t_hough::LOCATION_C,
                                        int orntBins = t_hough::ORIENT_BINS);
}

#include <transforms.hpp>

#endif
