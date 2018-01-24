#ifndef COMPUTER_VISION_HOMOGRAPHY_H
#define COMPUTER_VISION_HOMOGRAPHY_H

#include <detectors.h>

#include <random>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

namespace pi::transforms::t_homography {
    constexpr float H_THRESHOLD = 4.5f;
    constexpr int H_ITERS = 2000;
    constexpr int H_SIZE = 3;
}

namespace pi::transforms {
    typedef std::array<float, t_homography::H_SIZE> Transform1d;
    typedef std::array<Transform1d, t_homography::H_SIZE> Transform2d;
    typedef std::pair<detectors::Point, detectors::Point> PPairs;

    Transform2d homography(const std::vector<PPairs>& matches,
                           float threshold = t_homography::H_THRESHOLD,
                           int iters = t_homography::H_ITERS);
}

#endif //COMPUTER_VISION_HOMOGRAPHY_H
