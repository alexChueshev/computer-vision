#include <pyramid.h>

using namespace pi;

pyramids::Octave::Octave(Layer layer, int numLayers, int addLayers)
    : _step(_calcStep(numLayers))
    , _numLayers(numLayers)
    , _addLayers(addLayers)
{
    _layers.reserve(_numLayers + _addLayers);
    _layers.push_back(std::move(layer));
}

pyramids::Octave::Octave(const Img& img, int numLayers, int addLayers, float sigmaPrev, float sigmaNext)
    : _step(_calcStep(numLayers))
    , _numLayers(numLayers)
    , _addLayers(addLayers)
{
    _layers.reserve(_numLayers + _addLayers);
    _layers.push_back({filters::gaussian(img,
                                         _sigmaDelta(sigmaPrev, sigmaNext),
                                         borders::BORDER_REFLECT), sigmaNext, sigmaNext});
}

pyramids::Octave::Octave(std::vector<Layer> layers, float step, int addLayers)
    : _step(step)
    , _numLayers(layers.size() - addLayers)
    , _addLayers(addLayers)
    , _layers(std::move(layers))
{
}

pyramids::Octave pyramids::Octave::nextOctave() const {
    auto &lastOctaveLayer = _layers.at(_numLayers - 1);
    auto &firstOctaveLayer = _layers.front();

    return Octave({opts::scale(lastOctaveLayer.img),
                   firstOctaveLayer.sigma,
                   lastOctaveLayer.sigmaGlobal * _step
                  }, _numLayers, _addLayers);
}

pyramids::Octave& pyramids::Octave::createLayers() {
    for(auto i = 1, size = _numLayers + _addLayers; i < size; i++) {
        auto &prevLayer = _layers.back();
        auto sigma = _step * prevLayer.sigma;
        auto sigmaEffective = _step * prevLayer.sigmaGlobal;

        _layers.push_back({filters::gaussian(prevLayer.img,
                                             _sigmaDelta(prevLayer.sigma, sigma),
                                             borders::BORDER_REFLECT), sigma, sigmaEffective});
    }

    return *this;
}

float pyramids::Octave::step() const {
    return _step;
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

std::vector<pyramids::Octave> pyramids::gpyramid(const Img& img, int layers, const OctavesNumberFunction& op) {
    return gpyramid(img, layers, 0, op);
}

std::vector<pyramids::Octave> pyramids::gpyramid(const Img& img, int layers, int addLayers, const OctavesNumberFunction& op) {
    assert(img.channels() == 1);

    std::vector<Octave> octaves;
    octaves.reserve(op(std::min(img.width(), img.height())));

    //construct first octave
    octaves.push_back(Octave(img, layers, addLayers, Octave::SIGMA_START, Octave::SIGMA_ZERO).createLayers());

    //construct other octaves
    for(size_t i = 1, size = octaves.capacity(); i < size; i++) {
        octaves.push_back(octaves.back().nextOctave().createLayers());
    }

    return octaves;
}

std::vector<pyramids::Octave> pyramids::dog(const std::vector<Octave>& gpyramid) {
    auto octaves = gpyramid.size();
    std::vector<Octave> dpyramid;
    dpyramid.reserve(octaves);

    for(auto i = 0; i < octaves; i++) {
        auto &octave = gpyramid[i];
        auto &glayers = octave.layers();

        auto layers = glayers.size() - 1;
        std::vector<Layer> dlayers;
        dlayers.reserve(layers);

        for(auto j = 0; j < layers; j++) {
            auto &first = glayers[j];
            auto &second = glayers[j + 1];
            dlayers.push_back({opts::difference(first.img, second.img), first.sigma, first.sigmaGlobal});
        }

        dpyramid.push_back(Octave(std::move(dlayers), octave.step(), 0));
    }

    return dpyramid;
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
