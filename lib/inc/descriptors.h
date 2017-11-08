#ifndef COMPUTER_VISION_DESCRIPTORS_H
#define COMPUTER_VISION_DESCRIPTORS_H

#include <detectors.h>

#include <numeric>

namespace pi::descriptors {
    struct Descriptor;

    Descriptor hog(const detectors::Point& point, const std::pair<Img, Img>& sobel, int histoSize = 4,
                   int blockSize = 16, int bins = 8, borders::BorderTypes border = borders::BORDER_REPLICATE);

    Descriptor normalize(const Descriptor& descriptor);

    Descriptor trim(const Descriptor& descriptor, float threshold = .2f);

    float distance(const Descriptor& descriptor1, const Descriptor& descriptor2);

    std::vector<std::pair<Descriptor, Descriptor>> similar(const std::vector<Descriptor>& descriptors1,
                                                           const std::vector<Descriptor>& descriptors2,
                                                           float threshold = .8f);

    template<typename Functor, typename ...Args>
    std::vector<Descriptor> asDescriptors(const std::vector<detectors::Point>& points,
                                          Functor&& func, Args&&... args);
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

    ~Descriptor() = default;
};

#endif // COMPUTER_VISION_DESCRIPTORS_H
