#ifndef COMPUTER_VISION_DESCRIPTORS_H
#define COMPUTER_VISION_DESCRIPTORS_H

#include <descriptors.tpp>

namespace pi::descriptors {
    template<typename T>
    using NormalizeFunction = std::function<Descriptor<T>(const Descriptor<T>&)>;
    using BDescriptor = Descriptor<detectors::Point>;
    using BNormalizeFunction = NormalizeFunction<detectors::Point>;
    using RiDescriptor = Descriptor<detectors::RPoint>;
    using RiNormalizeFunction = NormalizeFunction<detectors::RPoint>;
    using SiDescriptor = Descriptor<detectors::SPoint>;
    using SiNormalizeFunction = NormalizeFunction<detectors::SPoint>;

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

    std::vector<BDescriptor> bDescriptors(const std::vector<detectors::Point>& points, const std::pair<Img, Img>& sobel,
                                          const BNormalizeFunction& norm, int histoSize = D_HISTO_SIZE,
                                          int histoNums = D_HISTO_NUMS, int bins = D_BINS,
                                          borders::BorderTypes border = borders::BORDER_REPLICATE, bool is3LInterp = false);

    std::vector<RiDescriptor> rhistogrid(const detectors::Point& point, const std::pair<Img, Img>& sobel,
                                         int histoSize = D_HISTO_SIZE, int histoNums = D_HISTO_NUMS, int bins = D_BINS,
                                         borders::BorderTypes border = borders::BORDER_REPLICATE, bool is3LInterp = false);

    std::vector<RiDescriptor> riDescriptors(const std::vector<detectors::Point>& points, const std::pair<Img, Img>& sobel,
                                            const RiNormalizeFunction& norm, int histoSize = D_HISTO_SIZE,
                                            int histoNums = D_HISTO_NUMS, int bins = D_BINS,
                                            borders::BorderTypes border = borders::BORDER_REPLICATE, bool is3LInterp = false);

    std::vector<SiDescriptor> shistogrid(detectors::SPoint point, const std::pair<Img, Img>& sobel,
                                         int histoSize = D_HISTO_SIZE, int histoNums = D_HISTO_NUMS, int bins = D_BINS,
                                         borders::BorderTypes border = borders::BORDER_REPLICATE, bool is3LInterp = true);

    std::vector<SiDescriptor> siDescriptors(const std::vector<detectors::SPoint>& points, const std::vector<pyramids::Octave>& gpyramid,
                                            const SiNormalizeFunction& norm, int histoSize = D_HISTO_SIZE, int histoNums = D_HISTO_NUMS,
                                            int bins = D_BINS, borders::BorderTypes border = borders::BORDER_REPLICATE,
                                            bool is3LInterp = true);
}

#endif // COMPUTER_VISION_DESCRIPTORS_H
