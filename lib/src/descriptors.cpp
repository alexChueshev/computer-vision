#include <descriptors.h>

using namespace pi;

descriptors::Descriptor::Descriptor(detectors::Point point, int size)
    : size(size)
    , point(std::move(point))
    , data(std::make_unique<float[]>(size))
{
    std::fill(data.get(), data.get() + size, 0);
}

descriptors::Descriptor::Descriptor(const descriptors::Descriptor& descriptor)
    : size(descriptor.size)
    , point(descriptor.point)
    , data(std::make_unique<float[]>(size))
{
    std::copy(descriptor.data.get(), descriptor.data.get() + size, data.get());
}

descriptors::Descriptor& descriptors::Descriptor::operator=(const descriptors::Descriptor& descriptor) {
    if(this != &descriptor) {
        point = descriptor.point;
        size = descriptor.size;

        data = std::make_unique<float[]>(size);
        std::copy(descriptor.data.get(), descriptor.data.get() + size, data.get());
    }
    return *this;
}

descriptors::Descriptor descriptors::hog(const detectors::Point& point, const std::pair<Img, Img>& sobel,
                                         int histoSize, int blockSize, int bins, borders::BorderTypes border) {
    assert(blockSize % histoSize == 0);

    auto bandwidth = 2 * M_PI / bins;
    auto histoNums = blockSize / histoSize;
    Descriptor descriptor(point, histoNums * histoNums * bins);

    auto fBorder = borders::get(border);
    auto gaussian = kernels::gaussian2d(std::log10(blockSize), blockSize);

    for(auto row = 0; row < blockSize; row++) {
        for(auto col = 0; col < blockSize; col++) {
            auto rI = point.row + row - blockSize / 2;
            auto rC = point.col + col - blockSize / 2;

            auto magnitudeVal = *gaussian.at(row, col) * filters::magnitudeVal(
                        fBorder(rI, rC, sobel.first),
                        fBorder(rI, rC, sobel.second));
            auto phi = filters::phiVal(
                        fBorder(rI, rC, sobel.first),
                        fBorder(rI, rC, sobel.second)) + M_PI;

            auto clbin = (phi / bandwidth) - .5f;
            auto distance = phi - bandwidth * (std::floor(clbin) + std::copysignf(.5f, clbin));
            auto lbin = clbin < 1e-6 ? bins - 1 : (int) floor(clbin);

            auto histoNum = row / histoSize * histoNums + col / histoSize;
            descriptor.data[histoNum * bins + lbin] += (1 - distance / bandwidth) * magnitudeVal;
            descriptor.data[histoNum * bins + (lbin + 1) % bins] += distance / bandwidth * magnitudeVal;
        }
    }

    return descriptor;
}

descriptors::Descriptor descriptors::normalize(const Descriptor& descriptor) {
    Descriptor normalized(descriptor.point, descriptor.size);

    auto hAccumulator = std::sqrt(
                            std::accumulate(descriptor.data.get(), descriptor.data.get() + descriptor.size, 0,
                                       [] (float accumulator, float value) {
        return accumulator + value * value;
    }));

    std::transform(descriptor.data.get(), descriptor.data.get() + descriptor.size, normalized.data.get(),
                   [&hAccumulator ] (float value) {
        return value / hAccumulator;
    });

    return normalized;
}

descriptors::Descriptor descriptors::trim(const Descriptor& descriptor, float threshold) {
    Descriptor trimmed(descriptor.point, descriptor.size);

    std::transform(descriptor.data.get(), descriptor.data.get() + descriptor.size, trimmed.data.get(),
                   [&threshold] (float value) {
        return std::min(value, threshold);
    });

    return trimmed;
}

float descriptors::distance(const Descriptor& descriptor1, const Descriptor& descriptor2) {
    assert(descriptor1.size == descriptor2.size);

    auto distance = .0f;
    for(auto i = 0; i < descriptor1.size; i++) {
        distance += std::pow(descriptor1.data[i] - descriptor2.data[i], 2);
    }

    return std::sqrt(distance);
}

template<typename Functor, typename ...Args>
std::vector<descriptors::Descriptor> descriptors::asDescriptors(const std::vector<detectors::Point>& points,
                                                                Functor&& func, Args&&... args) {
    std::vector<Descriptor> descriptors(points.size());

    for(const auto &point : points) {
        func(point, std::forward(args)...);
    }

    return descriptors;
}
