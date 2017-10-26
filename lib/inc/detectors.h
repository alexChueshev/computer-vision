#ifndef COMPUTER_VISION_DETECTORS_H
#define COMPUTER_VISION_DETECTORS_H

#include <operations.h>
#include <filters.h>

#include <cfloat>
#include <algorithm>
#include <vector>

namespace pi::detectors {
    struct Point;

    struct Direction;

    class Detector;

    class DetectorMoravec;

    class DetectorHarris;
}

struct pi::detectors::Direction {
    const int row;
    const int col;
};

struct pi::detectors::Point {
    int row;
    int col;
    float value;
};

class pi::detectors::Detector {

public:
    typedef std::function<float(int, int, int, int)> DistanceFunction;

public:
    constexpr static int PATCH_SHIFT = 2;
    constexpr static float THRESHOLD = .04f;
    constexpr static float ADAPT_NM_SUPR_COEFFICIENT = .9f;

protected:
    Img _img;
    std::vector<Point> _points;

public:
    Detector(const Img& img);

    Img addPointsToImage();

    Detector& adaptNonMaximumSuppr(int points, const DistanceFunction& distanceFunction);

    const std::vector<Point>& points() const;

    Img image() const;

    virtual Detector& apply(borders::BorderTypes border) = 0;

    virtual ~Detector() = default;

protected:
    void applyThreshold(const Img& dst, borders::BorderTypes border);
};

class pi::detectors::DetectorMoravec : public Detector {

public:
    constexpr static int PATCH_SHIFT = 2;
    constexpr static float THRESHOLD = .03f;

protected:
    std::array<Direction, 8> _directions;

public:
    DetectorMoravec(const Img& img);

    DetectorMoravec& apply(borders::BorderTypes border) override;

protected:
    void applyPatch(const Img& src, Img& dst, borders::BorderTypes border);
};

class pi::detectors::DetectorHarris : public Detector {

public:
    typedef std::function<float(int, int)> WindowFunction;

public:
    constexpr static float K = .04f;
    constexpr static float THRESHOLD = 4.f;
    constexpr static int PATCH_SHIFT = 2;

protected:
    int _windowSize;
    WindowFunction _windowFunction;

public:
    DetectorHarris(const Img& img, int windowSize, WindowFunction windowFunction);

    DetectorHarris& apply(borders::BorderTypes border) override;

protected:
    void applyPatch(const Img& src, Img& dst, borders::BorderTypes border);
};

#endif // COMPUTER_VISION_DETECTORS_H
