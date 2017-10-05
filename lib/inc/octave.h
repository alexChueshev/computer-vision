#ifndef COMPUTER_VISION_OCTAVE_H
#define COMPUTER_VISION_OCTAVE_H

#include <filters.h>
#include <operations.h>

#include <vector>

namespace pi {
namespace pyramids {
    struct Layer;

    class Octave;
}}

struct pi::pyramids::Layer {
    const Img img;
    const float sigma;
    const float sigmaEffective;
};

class pi::pyramids::Octave
{

public:
    typedef std::function<void(const Layer&)> LoopFunction;
    typedef std::function<float(float, float)> SigmaDeltaFunction;

protected:
    float _step;
    size_t _numLayers;
    std::vector<Layer> _layers;
    SigmaDeltaFunction _sigmaDelta;

public:
    Octave(const Layer& layer, size_t numLayers, const SigmaDeltaFunction& sigmaDelta);

    Octave(Layer&& layer, size_t numLayers, const SigmaDeltaFunction& sigmaDelta);

    Octave(const Img& img, size_t numLayers, float sigmaPrev, float sigmaNext,
           const SigmaDeltaFunction& sigmaDelta);

    Octave nextOctave() const;

    Octave& createLayers();

    const Octave& whileLoop(const LoopFunction& loopFuction) const;

    const std::vector<Layer>& layers() const;

protected:
    float step(size_t numLayers);
};

#endif // COMPUTER_VISION_OCTAVE_H
