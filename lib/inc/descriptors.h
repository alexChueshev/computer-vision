#ifndef COMPUTER_VISION_DESCRIPTORS_H
#define COMPUTER_VISION_DESCRIPTORS_H

#include <detectors.h>

namespace pi::descriptors {
    struct Descriptor;

    std::vector<Descriptor> hog(const detectors::Point& point);

    std::vector<Descriptor> normalize(const std::vector<Descriptor>& descriptor);

    std::vector<Descriptor> trim(const std::vector<Descriptor>& descriptor);
}

struct pi::descriptors::Descriptor {
    detectors::Point point;
    int height;
    int width;
    std::unique_ptr<float> content;

    Descriptor() = default;

    Descriptor(detectors::Point point, int height, int width);

    Descriptor(const Descriptor& descriptor);

    Descriptor(Descriptor&& descriptor) = default;

    Descriptor& operator=(const Descriptor& descriptor);

    Descriptor& operator=(Descriptor&& descriptor) = default;

    ~Descriptor() = default;
};

#endif // COMPUTER_VISION_DESCRIPTORS_H
