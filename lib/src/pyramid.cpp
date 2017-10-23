#include "pyramid.h"

using namespace pi;

pyramids::GaussianPyramid::GaussianPyramid(const Img& img, size_t numLayers) {
    _numOctaves = _numOctavesCalculations(std::min(img.width(), img.height()));
    _octaves.reserve(_numOctaves);

    //construct first octave
    _octaves.push_back(Octave(img, numLayers, SIGMA_START, SIGMA_ZERO,
                              [](float sigmaPrev, float sigmaNext) {
        return std::sqrt(std::pow(sigmaNext, 2) - std::pow(sigmaPrev, 2));
    }).createLayers());

    //construct other octaves
    for(size_t i = 1; i < _numOctaves; i++) {
        _octaves.push_back(_octaves.back().nextOctave().createLayers());
    }
}

const pyramids::GaussianPyramid& pyramids::GaussianPyramid::iterate(
        const LoopOctaveFunction& loopFunction) const {
    for(const auto &octave : _octaves) {
        loopFunction(octave);
    }

    return *this;
}

const pyramids::GaussianPyramid& pyramids::GaussianPyramid::iterate(
        const LoopLayerFunction& loopFunction) const {
    for(const auto &octave : _octaves) {
        for(const auto &layer : octave.layers()) {
            loopFunction(layer);
        }
    }

    return *this;
}

const std::vector<pyramids::Octave>& pyramids::GaussianPyramid::octaves() const {
    return _octaves;
}

size_t pyramids::GaussianPyramid::_numOctavesCalculations(int minImgMeasurement) {
    assert(minImgMeasurement > 0);

    return std::lround(std::log2(minImgMeasurement / MIN_OCTAVE_IMG_SIZE)) + 1;
}
