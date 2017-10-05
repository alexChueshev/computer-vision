#ifndef COMPUTER_VISION_PYRAMID_H
#define COMPUTER_VISION_PYRAMID_H

#include <octave.h>

#include <cmath>

namespace pi {
namespace pyramids {
    class Pyramid;

    class GaussianPyramid;
}}

class pi::pyramids::Pyramid
{

public:
    typedef std::function<void(const Octave&)> LoopOctaveFunction;
    typedef std::function<void(const Layer&)> LoopLayerFunction;

public:
    virtual Pyramid& apply(const Img &img, size_t numLayers) = 0;

    virtual Pyramid& whileLoop(const LoopOctaveFunction &loopFunction) = 0;

    virtual Pyramid& whileLoop(const LoopLayerFunction &loopFunction) = 0;

    virtual ~Pyramid() = default;
};

class pi::pyramids::GaussianPyramid : public Pyramid {

public:
    constexpr static int MIN_OCTAVE_IMG_SIZE = 16;
    constexpr static float SIGMA_ZERO = 1.4f;
    constexpr static float SIGMA_START = .5f;

protected:
    size_t _numOctaves;
    std::vector<Octave> _octaves;

public:
    GaussianPyramid& apply(const Img &img, size_t numLayers) override;

    GaussianPyramid& whileLoop(const LoopOctaveFunction &loopFunction) override;

    GaussianPyramid& whileLoop(const LoopLayerFunction &loopFunction) override;

    const std::vector<Octave>& octaves() const;

protected:
    size_t numOctavesCalculations(int minImgMeasurement);
};

#endif // COMPUTER_VISION_PYRAMID_H
