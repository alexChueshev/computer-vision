#ifndef COMPUTER_VISION_DETECTORS_H
#define COMPUTER_VISION_DETECTORS_H

#include <operations.h>
#include <filters.h>

#include <cfloat>
#include <algorithm>
#include <vector>

namespace pi::detectors {
    struct Point;

    typedef std::function<float(int, int, int, int)> DistanceFunction;

    std::vector<Point> moravec(const Img& src, int patchSize = 5, float threshold = .03f,
                               borders::BorderTypes border = borders::BORDER_REPLICATE);

    std::vector<Point> harris(const Img& src, int patchSize = 5, float threshold = .03f,
                              float k = .04f, borders::BorderTypes border = borders::BORDER_REPLICATE);

    std::vector<Point> adaptiveNonMaximumSuppresion(const std::vector<Point>& points, int quantity, float radiusMax,
                                                    const DistanceFunction& distanceFunction, float coefficient = .9f);

    Img addPointsTo(const Img& src, const std::vector<Point>& points);

    float eulerDistance(int x1, int x2, int y1, int y2);

    float maxRadius(const Img& img);
}

struct pi::detectors::Point {
    int row;
    int col;
    float value;
};

#endif // COMPUTER_VISION_DETECTORS_H
