#include <pyramid.h>

using namespace pi;

pyramids::Octave::Octave(const Layer& layer, int numLayers)
    : _step(_calcStep(numLayers))
    , _numLayers(numLayers)
{
    _layers.reserve(numLayers);
    _layers.push_back(layer);
}

pyramids::Octave::Octave(Layer&& layer, int numLayers)
    : _step(_calcStep(numLayers))
    , _numLayers(numLayers)
{
    _layers.reserve(numLayers);
    _layers.push_back(std::move(layer));
}

pyramids::Octave::Octave(const Img& img, int numLayers, float sigmaPrev, float sigmaNext)
    : _step(_calcStep(numLayers))
    , _numLayers(numLayers)
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
                  }, _numLayers);
}

pyramids::Octave& pyramids::Octave::createLayers() {
    for(auto i = 1; i < _numLayers; i++) {
        auto &prevLayer = _layers.back();
        auto sigma = _step * prevLayer.sigma;
        auto sigmaEffective = _step * prevLayer.sigmaEffective;

        _layers.push_back({filters::gaussian(prevLayer.img,
                                             _sigmaDelta(prevLayer.sigma, sigma),
                                             borders::BORDER_REFLECT), sigma, sigmaEffective});
    }

    return *this;
}

const std::vector<pyramids::Layer>& pyramids::Octave::layers() const {
    return _layers;
}

float pyramids::Octave::_calcStep(int numLayers) {
    return std::pow(2.f, 1.f / numLayers);
}

float pyramids::Octave::_sigmaDelta(float sigmaPrev, float sigmaNext) {
    return std::sqrt(std::pow(sigmaNext, 2) - std::pow(sigmaPrev, 2));
}

std::vector<pyramids::Octave> pyramids::gpyramid(const Img& img, int layers,
                                                 const OctavesNumberFunction& op) {
    assert(img.channels() == 1);

    std::vector<pyramids::Octave> octaves;
    octaves.reserve(op(std::min(img.width(), img.height())));

    //construct first octave
    octaves.push_back(Octave(img, layers, Octave::SIGMA_START, Octave::SIGMA_ZERO).createLayers());

    //construct other octaves
    for(size_t i = 1, size = octaves.capacity(); i < size; i++) {
        octaves.push_back(octaves.back().nextOctave().createLayers());
    }

    return octaves;
}

int pyramids::logOctavesCount(int dimension) {
    assert(dimension > 0);

    return std::lround(std::log2(dimension / Octave::MIN_OCTAVE_IMG_SIZE)) + 1;
}

void pyramids::iterate(const std::vector<Octave>& octaves, const LoopOctaveFunction& loopFunction) {
    for(const auto &octave : octaves) {
        loopFunction(octave);
    }
}

void pyramids::iterate(const std::vector<Octave>& octaves, const LoopLayerFunction& loopFunction) {
    for(const auto &octave : octaves) {
        for(const auto &layer : octave.layers()) {
            loopFunction(layer);
        }
    }
}
