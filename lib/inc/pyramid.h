#ifndef COMPUTER_VISION_PYRAMID_H
#define COMPUTER_VISION_PYRAMID_H

#include <octave.h>

namespace pi::pyramids {
    class GaussianPyramid;
}

class pi::pyramids::GaussianPyramid {

public:
    typedef std::function<void(const Octave&)> LoopOctaveFunction;
    typedef std::function<void(const Layer&)> LoopLayerFunction;

public:
    constexpr static int MIN_OCTAVE_IMG_SIZE = 16;
    constexpr static float SIGMA_ZERO = 1.4f;
    constexpr static float SIGMA_START = .5f;

protected:
    size_t _numOctaves;
    std::vector<Octave> _octaves;

public:
    GaussianPyramid(const Img& img, size_t numLayers);

    const GaussianPyramid& iterate(const LoopOctaveFunction& loopFunction) const;

    const GaussianPyramid& iterate(const LoopLayerFunction& loopFunction) const;

    const std::vector<Octave>& octaves() const;

protected:
    size_t _numOctavesCalculations(int minImgMeasurement);
};

#endif // COMPUTER_VISION_PYRAMID_H
