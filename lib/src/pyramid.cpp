#include "pyramid.h"

using namespace pi;

pyramids::GaussianPyramid& pyramids::GaussianPyramid::apply(const Img &img) {

    return *this;
}

pyramids::GaussianPyramid& pyramids::GaussianPyramid::whileLoop(const LoopOctaveFunction &loopFunction) {

    return *this;
}

pyramids::GaussianPyramid& pyramids::GaussianPyramid::whileLoop(const LoopLayerFunction &loopFunction) {

    return *this;
}

const std::vector<pyramids::Octave>& pyramids::GaussianPyramid::octaves() const {
    return _octaves;
}
