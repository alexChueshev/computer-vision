#ifndef COMPUTER_VISION_HOMOGRAPHY_H
#define COMPUTER_VISION_HOMOGRAPHY_H

#include <transforms.h>

#include <random>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

namespace pi::transforms::t_homography {
    constexpr float H_THRESHOLD = 4.5f;
    constexpr int H_ITERS = 1200;
    constexpr int H_SIZE = 3;

    typedef std::pair<detectors::Point, detectors::Point> PPairs;
}

namespace pi::transforms {
    Transform2d homography(const std::vector<t_homography::PPairs>& matches,
                           float threshold = t_homography::H_THRESHOLD,
                           int iters = t_homography::H_ITERS);
}

#endif //COMPUTER_VISION_HOMOGRAPHY_H
