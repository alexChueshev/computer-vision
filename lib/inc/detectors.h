#ifndef COMPUTER_VISION_DETECTORS_H
#define COMPUTER_VISION_DETECTORS_H

#include <operations.h>
#include <filters.h>

#include <cfloat>

namespace pi {
namespace detectors {
    struct Point;

    struct Direction;

    class Detector;

    class DetectorMoravec;

    class DetectorHarris;
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

public:
    constexpr static int PATCH_SHIFT = 2;
    constexpr static float THRESHOLD = .04f;

protected:
    std::vector<Point> _points;

public:
    virtual Detector& apply(const Img& img, borders::BorderTypes border) = 0;

    Detector& addPointsTo(Img& img);

    const std::vector<Point>& points() const;

    virtual ~Detector() = default;

protected:
    void applyThreshold(Img& dst, borders::BorderTypes border);
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
    void applyPatch(const Img& src, Img& dst, borders::BorderTypes border);
};

class pi::detectors::DetectorHarris : public Detector {

public:
    typedef std::function<float(int, int)> WindowFunction;

public:
    constexpr static float K = .06f;
    constexpr static float THRESHOLD = 3.5f;
    constexpr static int PATCH_SHIFT = 2;

protected:
    int _windowSize;
    WindowFunction _windowFunction;

public:
    DetectorHarris(int windowSize, WindowFunction windowFunction);

    DetectorHarris& apply(const Img& img, borders::BorderTypes border) override;

protected:
    void applyPatch(const Img& src, Img& dst, borders::BorderTypes border);
};

#endif // COMPUTER_VISION_DETECTORS_H
