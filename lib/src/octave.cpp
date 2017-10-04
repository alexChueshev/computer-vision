#include "octave.h"

using namespace pi;

const pyramids::Octave& pi::pyramids::Octave::whileLoop(const LoopFunction &loopFuction) const {

    return *this;
}

const std::vector<pyramids::Layer>& pyramids::Octave::layers() const {
    return _layers;
}
