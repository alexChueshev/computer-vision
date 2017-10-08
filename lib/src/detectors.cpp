#include "detectors.h"

using namespace pi;

detectors::DetectorMoravek& detectors::DetectorMoravek::apply(const Img& img,
                                                              borders::BorderTypes border) {

    return *this;
}

const std::vector<detectors::Point>& detectors::DetectorMoravek::points() const {
    return _points;
}
