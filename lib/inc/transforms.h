#ifndef COMPUTER_VISION_TRANSFORMS_H
#define COMPUTER_VISION_TRANSFORMS_H

#include <detectors.h>

#include <random>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

namespace pi::transforms {
    typedef std::pair<detectors::Point, detectors::Point> PPairs;

    void homography(const std::vector<PPairs>& matches, int iters = 2000);
}

#endif //COMPUTER_VISION_TRANSFORMS_H
