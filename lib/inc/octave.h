#ifndef COMPUTER_VISION_OCTAVE_H
#define COMPUTER_VISION_OCTAVE_H

#include <img.h>

#include <vector>

namespace pi {
namespace pyramids {
    struct Layer;

    class Octave;
}}

struct pi::pyramids::Layer {
    Img img;
    float sigmaLocal;
    float sigmaGlobal;
};

class pi::pyramids::Octave
{

public:
    typedef std::function<void(const Layer&)> LoopFunction;

protected:
    std::vector<Layer> _layers;

public:
    const Octave& whileLoop(const LoopFunction &loopFuction) const;

    const std::vector<Layer>& layers() const;
};

#endif // COMPUTER_VISION_OCTAVE_H
