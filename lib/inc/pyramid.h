#ifndef COMPUTER_VISION_PYRAMID_H
#define COMPUTER_VISION_PYRAMID_H

#include <octave.h>

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
    virtual Pyramid& apply(const Img &img) = 0;

    virtual Pyramid& whileLoop(const LoopOctaveFunction &loopFunction) = 0;

    virtual Pyramid& whileLoop(const LoopLayerFunction &loopFunction) = 0;

    virtual ~Pyramid() = default;
};

class pi::pyramids::GaussianPyramid : public Pyramid {

protected:
    std::vector<Octave> _octaves;

public:
    GaussianPyramid& apply(const Img &img) override;

    GaussianPyramid& whileLoop(const LoopOctaveFunction &loopFunction) override;

    GaussianPyramid& whileLoop(const LoopLayerFunction &loopFunction) override;

    const std::vector<Octave>& octaves() const;
};

#endif // COMPUTER_VISION_PYRAMID_H
