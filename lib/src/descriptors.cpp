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

descriptors::Descriptor descriptors::histogrid(const detectors::Point& point, const std::pair<Img, Img>& sobel,
                                         float angle, int histoSize, int blockSize,
                                         int bins, borders::BorderTypes border) {
    assert(blockSize % histoSize == 0);

    auto bandwidth = 2 * M_PI / bins;
    auto hBlockSize = blockSize / 2;
    auto histoNums = blockSize / histoSize;
    Descriptor descriptor(point, histoNums * histoNums * bins);

    auto fCos = std::cos(angle);
    auto fSin = std::sin(angle);

    auto fBorder = borders::get(border);
    auto gaussian = kernels::gaussian2d(std::log10(blockSize), blockSize);

    for(auto row = 0; row < blockSize; row++) {
        for(auto col = 0; col < blockSize; col++) {
            auto rR = row - hBlockSize;
            auto rC = col - hBlockSize;

            int r = rR * fCos - rC * fSin + hBlockSize;
            int c = rC * fCos + rR * fSin + hBlockSize;

            if(r < 0 || r >= blockSize || c < 0 || c >= blockSize) continue;

            auto magnitudeVal = *gaussian.at(row, col) * filters::magnitudeVal(
                        fBorder(point.row + rR, point.col + rC, sobel.first),
                        fBorder(point.row + rR, point.col + rC, sobel.second));
            auto phi = filters::phiVal(
                        fBorder(point.row + rR, point.col + rC, sobel.first),
                        fBorder(point.row + rR, point.col + rC, sobel.second)) + M_PI - angle;
            if(phi < 0 || phi > 2 * M_PI) phi += -std::copysign(1, phi) * 2 * M_PI;

            auto clbin = (phi / bandwidth) - .5f;
            auto distance = phi - bandwidth * (std::floor(clbin) + std::copysignf(.5f, clbin));
            auto lbin = clbin < 1e-8 ? bins - 1 : (int) floor(clbin);

            auto histoNum = r / histoSize * histoNums + c / histoSize;
            descriptor.data[histoNum * bins + lbin] += (1 - distance / bandwidth) * magnitudeVal;
            descriptor.data[histoNum * bins + (lbin + 1) % bins] += distance / bandwidth * magnitudeVal;
        }
    }

    return descriptor;
}

std::vector<descriptors::Descriptor> descriptors::histogrid(const std::vector<detectors::Point>& points,
                                                      const std::pair<Img, Img>& sobel, const NormalizeFunction& norm,
                                                      int histoSize, int blockSize, int bins, borders::BorderTypes border) {
    std::vector<Descriptor> descriptors;
    descriptors.reserve(points.size());

    for(const auto &point : points) {
        descriptors.push_back(norm(histogrid(point, sobel, .0f, histoSize, blockSize, bins, border)));
    }

    return descriptors;
}

std::vector<descriptors::Descriptor> descriptors::rhistogrid(const detectors::Point& point, const std::pair<Img, Img>& sobel,
                                                       int histoSize, int blockSize, int bins,
                                                       borders::BorderTypes border) {
    assert(blockSize % histoSize == 0);

    auto parabolic3bfit = [](const Descriptor& descriptor, int peak) {
        assert(peak >= 0 && peak < descriptor.size);

        auto ym1 = descriptor.data[(peak - 1) % descriptor.size];
        auto y0 = descriptor.data[peak];
        auto yp1 = descriptor.data[(peak + 1) % descriptor.size];
        auto p = (ym1 - yp1) / (2 * (ym1 - 2 * y0 + yp1)); // [-1/2;1/2]
        return ((peak + p < 0) ? descriptor.size : peak) + p;
    };

    std::vector<Descriptor> descriptors;
    auto base = histogrid(point, sobel, .0f, blockSize, blockSize, 36, border);
    auto peaksIndexes = peaks(base, .8f, 2);

    for(auto index : peaksIndexes) {
        descriptors.push_back(histogrid(point, sobel, parabolic3bfit(base, index) * 2 * M_PI / base.size, histoSize,
                                  blockSize, bins, border));
    }

    return descriptors;
}

std::vector<descriptors::Descriptor> descriptors::rhistogrid(const std::vector<detectors::Point>& points,
                                                       const std::pair<Img, Img>& sobel, const NormalizeFunction& norm,
                                                       int histoSize, int blockSize, int bins, borders::BorderTypes border) {
    std::vector<Descriptor> descriptors;
    descriptors.reserve(points.size());

    for(const auto &point : points) {
        for(const auto &unnormalized : rhistogrid(point, sobel, histoSize, blockSize, bins, border)) {
            descriptors.push_back(norm(unnormalized));
        }
    }

    return descriptors;
}

descriptors::Descriptor descriptors::normalize(const Descriptor& descriptor) {
    Descriptor normalized(descriptor.point, descriptor.size);

    auto hAccumulator = std::sqrt(
                            std::accumulate(descriptor.data.get(), descriptor.data.get() + descriptor.size, .0f,
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

std::vector<int> descriptors::peaks(const Descriptor& descriptor, float threshold, int nums) {
    std::vector<int> positions;

    auto maxValue = std::max_element(descriptor.data.get(), descriptor.data.get() + descriptor.size);
    auto fIndex = std::distance(descriptor.data.get(), maxValue);
    positions.push_back(fIndex);

    for(auto i = 0; i < descriptor.size && positions.size() < nums; i++) {
        if(descriptor.data[i] >= *maxValue * threshold && i != fIndex) {
            positions.push_back(i);
        }
    }

    return positions;
}

float descriptors::distance(const Descriptor& descriptor1, const Descriptor& descriptor2) {
    assert(descriptor1.size == descriptor2.size);

    auto distance = .0f;
    for(auto i = 0; i < descriptor1.size; i++) {
        distance += std::pow(descriptor1.data[i] - descriptor2.data[i], 2);
    }

    return std::sqrt(distance);
}

std::vector<std::pair<descriptors::Descriptor, descriptors::Descriptor>> descriptors::match(
                                                                            const std::vector<Descriptor>& descriptors1,
                                                                            const std::vector<Descriptor>& descriptors2,
                                                                            float threshold) {
    std::vector<std::pair<Descriptor, Descriptor>> matches;

    for(const auto &descriptor1 : descriptors1) {
        auto minDistance1 = FLT_MAX, minDistance2 = FLT_MAX;
        auto index = 0, counter = 0;

        for(const auto &descriptor2 : descriptors2) {
            auto distance = descriptors::distance(descriptor1, descriptor2);
            if(distance < minDistance1) {
                minDistance2 = minDistance1;
                minDistance1 = distance;
                index = counter;
            } else if(distance < minDistance2) {
                minDistance2 = distance;
            }
            counter++;
        }

        if(minDistance1 / minDistance2 <= threshold) {
            matches.push_back(std::pair<Descriptor, Descriptor>(descriptor1, descriptors2[index]));
        }
    }

    return matches;
}
