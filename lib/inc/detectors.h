#ifndef COMPUTER_VISION_DETECTORS_H
#define COMPUTER_VISION_DETECTORS_H

#include <borders.h>

#include <vector>

namespace pi {
namespace detectors {
    struct Point;

    class Detector;

    class DetectorMoravek;
}}

struct pi::detectors::Point {
    const int x;
    const int y;
};

class pi::detectors::Detector {

public:
    virtual Detector& apply(const Img& img, borders::BorderTypes border);

    virtual ~Detector() = default;
};

class pi::detectors::DetectorMoravek : public Detector {

protected:
    std::vector<Point> _points;

public:
    DetectorMoravek& apply(const Img& img, borders::BorderTypes border) override;

    const std::vector<Point>& points() const;
};

#endif // COMPUTER_VISION_DETECTORS_H
