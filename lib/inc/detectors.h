#ifndef COMPUTER_VISION_DETECTORS_H
#define COMPUTER_VISION_DETECTORS_H

#include <pyramid.h>

#include <cfloat>
#include <vector>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

namespace pi::detectors {
    struct Point;

    struct RPoint;

    struct SPoint;

    typedef std::function<float(int, int, int, int)> DistanceFunction;

    typedef std::function<float(
            const std::pair<Img, Img>&, const kernels::Kernel&,
            int, int, int, const borders::Function&)> HarrisBasedAlgorithm;

    std::vector<Point> moravec(const Img& src, int patchSize = 5, float threshold = .03f,
                               borders::BorderTypes border = borders::BORDER_REPLICATE);

    std::vector<Point> harris(const Img& src, int patchSize = 5, float threshold = .03f,
                              float k = .04f, borders::BorderTypes border = borders::BORDER_REPLICATE);

    std::vector<SPoint> harris(const std::vector<pyramids::Octave>& dog, const std::vector<SPoint>& blobs,
                               float threshold = .001f, float k = .04f,
                               borders::BorderTypes border = borders::BORDER_REPLICATE);

    std::vector<SPoint> shiTomasi(const std::vector<pyramids::Octave>& dog, const std::vector<SPoint>& blobs,
                                  float threshold = .001f, borders::BorderTypes border = borders::BORDER_REPLICATE);

    std::vector<SPoint> blobs(const std::vector<pyramids::Octave>& dog, float contrastThreshold = .03,
                              borders::BorderTypes border = borders::BORDER_REPLICATE);

    std::vector<Point> adaptiveNonMaximumSuppresion(const std::vector<Point>& points, int quantity, float radiusMax,
                                                    const DistanceFunction& distanceFunction, float coefficient = .9f);
}

namespace pi::detectors::utils {
    using RIArray3d = std::array<std::reference_wrapper<const Img>, 3>;

    float harris(const std::array<float, 3>& values, float k = .04f);

    float shiTomasi(const std::array<float, 3>& values);

    std::array<double, 3> pDerivatives3d(const RIArray3d images, int r, int c, const borders::Function& fBorder);

    std::array<double, 9> hessian3d(const RIArray3d images, int r, int c, const borders::Function& fBorder);

    std::array<double, 4> hessian2d(const pi::Img& img, int r, int c, const borders::Function& fBorder);
}

struct pi::detectors::Point {
    int row;
    int col;
    float value;
};

struct pi::detectors::RPoint : Point {
    float angle;
};

struct pi::detectors::SPoint : RPoint {
    int localRow;
    int localCol;
    int octave;
    int layer;
    float sigma;
    float sigmaGlobal;
};

#endif // COMPUTER_VISION_DETECTORS_H
