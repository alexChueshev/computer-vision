#ifndef COMPUTER_VISION_PYRAMID_H
#define COMPUTER_VISION_PYRAMID_H

#include <filters.h>
#include <operations.h>

#include <vector>

namespace pi::pyramids {
    struct Layer;

    class Octave;

    typedef std::function<int(int)> OctavesNumberFunction;
    typedef std::function<void(const Octave&)> LoopOctaveFunction;
    typedef std::function<void(const Layer&)> LoopLayerFunction;

    std::vector<Octave> gpyramid(const Img& img, int layers, const OctavesNumberFunction& op);

    int logOctavesCount(int dimension);

    void iterate(const std::vector<Octave>& octaves, const LoopOctaveFunction& loopFunction);

    void iterate(const std::vector<Octave>& octaves, const LoopLayerFunction& loopFunction);
}

struct pi::pyramids::Layer {
    const Img img;
    const float sigma;
    const float sigmaEffective;
};

class pi::pyramids::Octave {

public:
    constexpr static int MIN_OCTAVE_IMG_SIZE = 16;
    constexpr static float SIGMA_ZERO = 1.4f;
    constexpr static float SIGMA_START = .5f;

protected:
    float _step;
    int _numLayers;
    std::vector<Layer> _layers;

public:
    Octave(const Layer& layer, int numLayers);

    Octave(Layer&& layer, int numLayers);

    Octave(const Img& img, int numLayers, float sigmaPrev, float sigmaNext);

    Octave nextOctave() const;

    Octave& createLayers();

    const std::vector<Layer>& layers() const;

protected:
    float _calcStep(int numLayers);

    float _sigmaDelta(float sigmaPrev, float sigmaNext);
};

#endif // COMPUTER_VISION_PYRAMID_H
