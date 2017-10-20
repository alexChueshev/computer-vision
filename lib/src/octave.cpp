#include "octave.h"

using namespace pi;

pyramids::Octave::Octave(const Layer& layer, size_t numLayers,
                         const SigmaDeltaFunction& sigmaDelta)
    : _step(step(numLayers))
    , _numLayers(numLayers)
    , _sigmaDelta(sigmaDelta)
{
    _layers.reserve(numLayers);
    _layers.push_back(layer);
}

pyramids::Octave::Octave(Layer&& layer, size_t numLayers,
                         const SigmaDeltaFunction& sigmaDelta)
    : _step(step(numLayers))
    , _numLayers(numLayers)
    , _sigmaDelta(sigmaDelta)
{
    _layers.reserve(numLayers);
    _layers.push_back(std::move(layer));
}

pyramids::Octave::Octave(const Img& img, size_t numLayers,
                         float sigmaPrev, float sigmaNext,
                         const SigmaDeltaFunction& sigmaDelta)
    : _step(step(numLayers))
    , _numLayers(numLayers)
    , _sigmaDelta(sigmaDelta)
{
    auto layerImg = img;

    filters::Gaussian gaussian(_sigmaDelta(sigmaPrev, sigmaNext));
    gaussian.apply(layerImg, borders::BORDER_REFLECT);

    _layers.push_back({std::move(layerImg), sigmaNext, sigmaNext});
}

pi::pyramids::Octave pyramids::Octave::nextOctave() const {
    auto &lastOctaveLayer = _layers.back();
    auto &firstOctaveLayer = _layers.front();

    auto layerImg = lastOctaveLayer.img;
    auto sigma = firstOctaveLayer.sigma;
    auto sigmaEffective = _step * lastOctaveLayer.sigmaEffective;

    opts::scale(layerImg);

    return Octave({std::move(layerImg), sigma, sigmaEffective}, _numLayers, _sigmaDelta);
}

pyramids::Octave& pyramids::Octave::createLayers() {
    for(size_t i = 1; i < _numLayers; i++) {
        auto &prevLayer = _layers.back();
        auto layerImg = prevLayer.img;

        auto sigma = _step * prevLayer.sigma;
        auto sigmaEffective = _step * prevLayer.sigmaEffective;

        filters::Gaussian gaussian(_sigmaDelta(prevLayer.sigma, sigma));
        gaussian.apply(layerImg, borders::BORDER_REFLECT);

        _layers.push_back({std::move(layerImg), sigma, sigmaEffective});
    }

    return *this;
}

const pyramids::Octave& pyramids::Octave::whileLoop(const LoopFunction& loopFuction) const {
    for(const auto &layer : _layers) {
        loopFuction(layer);
    }

    return *this;
}

const std::vector<pyramids::Layer>& pyramids::Octave::layers() const {
    return _layers;
}

float pyramids::Octave::step(size_t numLayers) {
    return std::pow(2.f, 1.f / numLayers);
}
