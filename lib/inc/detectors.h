#ifndef COMPUTER_VISION_DETECTORS_H
#define COMPUTER_VISION_DETECTORS_H

#include <borders.h>
#include <operations.h>

#include <vector>
#include <cfloat>
#include <cmath>

namespace pi {
namespace detectors {
    struct Point;

    struct Direction;

    class Detector;

    class DetectorMoravec;
}}

struct pi::detectors::Direction {
    const int row;
    const int col;
};

struct pi::detectors::Point {
    const int row;
    const int col;
};

class pi::detectors::Detector {

protected:
    std::vector<Point> _points;

public:
    virtual Detector& apply(const Img& img, borders::BorderTypes border) = 0;

    Detector& addPointsTo(Img& img);

    const std::vector<Point>& points() const;

    virtual ~Detector() = default;
};

class pi::detectors::DetectorMoravec : public Detector {

public:
    constexpr static int PATCH_SHIFT = 2;
    constexpr static float THRESHOLD = .03f;

protected:
    std::array<Direction, 8> _directions;

public:
    DetectorMoravec();

    DetectorMoravec& apply(const Img& img, borders::BorderTypes border) override;

protected:
    void applyThreshold(Img& dst, borders::BorderTypes border);

    void applyPatch(const Img& src, Img& dst, borders::BorderTypes border);
};

#endif // COMPUTER_VISION_DETECTORS_H
