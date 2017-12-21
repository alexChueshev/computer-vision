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

    std::vector<Octave> gpyramid(const Img& img, int layers, int addLayers, const OctavesNumberFunction& op);

    std::vector<Octave> dog(const Img& img, int layers, const OctavesNumberFunction& op);

    std::vector<Octave> dog(const std::vector<Octave>& gpyramid);

    Layer dog(const Layer& first, const Layer& second);

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
    constexpr static float SIGMA_ZERO = 1.6f;
    constexpr static float SIGMA_START = .5f;

protected:
    float _step;
    int _numLayers;
    int _addLayers;
    std::vector<Layer> _layers;

public:
    Octave(Layer layer, int numLayers, int addLayers);

    Octave(const Img& img, int numLayers, int addLayers, float sigmaPrev, float sigmaNext);

    Octave(std::vector<Layer> layers, float step, int addLayers);

    Octave nextOctave() const;

    Octave& createLayers();

    const std::vector<Layer>& layers() const;

    float step() const;

protected:
    float _calcStep(int numLayers);

    float _sigmaDelta(float sigmaPrev, float sigmaNext);
};

#endif // COMPUTER_VISION_PYRAMID_H
