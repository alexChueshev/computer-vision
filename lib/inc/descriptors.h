#ifndef COMPUTER_VISION_DESCRIPTORS_H
#define COMPUTER_VISION_DESCRIPTORS_H

#include <detectors.h>

#include <numeric>

namespace pi::descriptors {
    struct Descriptor;

    typedef std::function<Descriptor(const Descriptor&)> NormalizeFunction;

    constexpr int D_HISTO_SIZE = 4;
    constexpr int D_HISTO_NUMS = 4;
    constexpr int D_BINS = 8;

    constexpr float ORI_PEAK_RATIO = .8f;
    constexpr float ORI_SIGMA_C = 1.5f;
    constexpr float MAGNITUDE_SIGMA_C = .5f;

    std::unique_ptr<float[]> histogrid(const std::pair<Img, Img>& sobel, int pR, int pC, float angle = .0f,
                                       int histoSize = D_HISTO_SIZE, int histoNums = D_HISTO_NUMS, int bins = D_BINS,
                                       float sigma = 5, borders::BorderTypes border = borders::BORDER_REPLICATE,
                                       bool is3LInterp = true);

    std::vector<Descriptor> histogrid(const std::vector<detectors::Point>& points, const std::pair<Img, Img>& sobel,
                                      const NormalizeFunction& norm, int histoSize = D_HISTO_SIZE, int histoNums = D_HISTO_NUMS,
                                      int bins = D_BINS, borders::BorderTypes border = borders::BORDER_REPLICATE,
                                      bool is3LInterp = false);

    std::vector<Descriptor> rhistogrid(const detectors::Point& point, const std::pair<Img, Img>& sobel,
                                       int histoSize = D_HISTO_SIZE, int histoNums = D_HISTO_NUMS, int bins = D_BINS,
                                       borders::BorderTypes border = borders::BORDER_REPLICATE, bool is3LInterp = false);

    std::vector<Descriptor> riDescriptors(const std::vector<detectors::Point>& points, const std::pair<Img, Img>& sobel,
                                          const NormalizeFunction& norm, int histoSize = D_HISTO_SIZE,
                                          int histoNums = D_HISTO_NUMS, int bins = D_BINS,
                                          borders::BorderTypes border = borders::BORDER_REPLICATE, bool is3LInterp = false);

    std::vector<Descriptor> shistogrid(const detectors::SPoint& point, const std::pair<Img, Img>& sobel,
                                       int histoSize = D_HISTO_SIZE, int histoNums = D_HISTO_NUMS, int bins = D_BINS,
                                       borders::BorderTypes border = borders::BORDER_REPLICATE, bool is3LInterp = true);

    std::vector<Descriptor> siDescriptors(const std::vector<detectors::SPoint>& points, const std::vector<pyramids::Octave>& gpyramid,
                                          const NormalizeFunction& norm, int histoSize = D_HISTO_SIZE, int histoNums = D_HISTO_NUMS,
                                          int bins = D_BINS, borders::BorderTypes border = borders::BORDER_REPLICATE,
                                          bool is3LInterp = true);

    Descriptor normalize(Descriptor descriptor);

    Descriptor trim(Descriptor descriptor, float threshold = .2f);

    float distance(const Descriptor& descriptor1, const Descriptor& descriptor2);

    std::vector<std::pair<Descriptor, Descriptor>> match(const std::vector<Descriptor>& descriptors1,
                                                         const std::vector<Descriptor>& descriptors2,
                                                         float threshold = .7f);

    std::vector<int> peaks(const Descriptor& descriptor, float threshold = .8f, int nums = 2);
}

struct pi::descriptors::Descriptor {
    int size;
    detectors::Point point;
    std::unique_ptr<float[]> data;

    Descriptor() = default;

    Descriptor(detectors::Point point, int size);

    Descriptor(const Descriptor& descriptor);

    Descriptor(Descriptor&& descriptor) = default;

    Descriptor& operator=(const Descriptor& descriptor);

    Descriptor& operator=(Descriptor&& descriptor) = default;

    Descriptor(detectors::Point point, int size, std::unique_ptr<float[]> data);

    ~Descriptor() = default;
};

#endif // COMPUTER_VISION_DESCRIPTORS_H
