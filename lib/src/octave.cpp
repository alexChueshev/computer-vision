#include "octave.h"

using namespace pi;

pyramids::Octave::Octave(const Layer& layer, size_t numLayers,
                         const SigmaDeltaFunction& sigmaDelta)
    : _step(_calcStep(numLayers))
    , _numLayers(numLayers)
    , _sigmaDelta(sigmaDelta)
{
    _layers.reserve(numLayers);
    _layers.push_back(layer);
}

pyramids::Octave::Octave(Layer&& layer, size_t numLayers,
                         const SigmaDeltaFunction& sigmaDelta)
    : _step(_calcStep(numLayers))
    , _numLayers(numLayers)
    , _sigmaDelta(sigmaDelta)
{
    _layers.reserve(numLayers);
    _layers.push_back(std::move(layer));
}

pyramids::Octave::Octave(const Img& img, size_t numLayers,
                         float sigmaPrev, float sigmaNext,
                         const SigmaDeltaFunction& sigmaDelta)
    : _step(_calcStep(numLayers))
    , _numLayers(numLayers)
    , _sigmaDelta(sigmaDelta)
{
    _layers.push_back({filters::gaussian(img,
                                         _sigmaDelta(sigmaPrev, sigmaNext),
                                         borders::BORDER_REFLECT), sigmaNext, sigmaNext});
}

pi::pyramids::Octave pyramids::Octave::nextOctave() const {
    auto &lastOctaveLayer = _layers.back();
    auto &firstOctaveLayer = _layers.front();

    return Octave({opts::scale(lastOctaveLayer.img),
                   firstOctaveLayer.sigma,
                   lastOctaveLayer.sigmaEffective * _step
                  }, _numLayers, _sigmaDelta);
}

pyramids::Octave& pyramids::Octave::createLayers() {
    for(size_t i = 1; i < _numLayers; i++) {
        auto &prevLayer = _layers.back();
        auto sigma = _step * prevLayer.sigma;
        auto sigmaEffective = _step * prevLayer.sigmaEffective;

        _layers.push_back({filters::gaussian(prevLayer.img,
                                             _sigmaDelta(prevLayer.sigma, sigma),
                                             borders::BORDER_REFLECT), sigma, sigmaEffective});
    }

    return *this;
}

const pyramids::Octave& pyramids::Octave::iterate(const LoopFunction& loopFuction) const {
    for(const auto &layer : _layers) {
        loopFuction(layer);
    }

    return *this;
}

const std::vector<pyramids::Layer>& pyramids::Octave::layers() const {
    return _layers;
}

float pyramids::Octave::_calcStep(size_t numLayers) {
    return std::pow(2.f, 1.f / numLayers);
}
