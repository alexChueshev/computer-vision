#include <descriptors.h>

using namespace pi;

namespace {
    int _parabolic3bfit(const descriptors::Descriptor& descriptor, int peak) {
        assert(peak >= 0 && peak < descriptor.size);

        auto ym1 = descriptor.data[(peak - 1) % descriptor.size];
        auto y0 = descriptor.data[peak];
        auto yp1 = descriptor.data[(peak + 1) % descriptor.size];
        auto p = (ym1 - yp1) / (2 * (ym1 - 2 * y0 + yp1)); // [-1/2;1/2]
        return ((peak + p < 0) ? descriptor.size : peak) + p;
    }
}

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
                                               float angle, int histoSize, int histoNums,
                                               int bins, float sigma, borders::BorderTypes border) {
    auto bandwidth = 2 * M_PI / bins;
    auto blockSize = histoSize * histoNums;
    auto hBlockSize = blockSize / 2;
    Descriptor descriptor(point, histoNums * histoNums * bins);

    auto fCos = std::cos(angle);
    auto fSin = std::sin(angle);

    auto fBorder = borders::get(border);
    auto gaussian = kernels::gaussian2d(sigma, blockSize);

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
                                                            int histoSize, int histoNums, int bins,
                                                            borders::BorderTypes border) {
    std::vector<Descriptor> descriptors;
    descriptors.reserve(points.size());
    auto blockSize = histoSize * histoNums;
    auto sigma = std::log10(blockSize);

    for(const auto &point : points) {
        descriptors.push_back(norm(histogrid(point, sobel, .0f, histoSize, histoNums, bins, sigma, border)));
    }

    return descriptors;
}

std::vector<descriptors::Descriptor> descriptors::rhistogrid(const detectors::Point& point, const std::pair<Img, Img>& sobel,
                                                             int histoSize, int histoNums, int bins,
                                                             borders::BorderTypes border) {
    std::vector<Descriptor> descriptors;
    auto blockSize = histoNums * histoSize;
    auto sigma = std::log10(blockSize);
    auto base = histogrid(point, sobel, .0f, blockSize, 1, 36, sigma, border);
    auto peaksIndexes = peaks(base, ORI_PEAK_RATIO, 2);

    for(auto index : peaksIndexes) {
        descriptors.push_back(histogrid(point, sobel, _parabolic3bfit(base, index) * 2 * M_PI / base.size, histoSize,
                                        histoNums, bins, sigma, border));
    }

    return descriptors;
}

std::vector<descriptors::Descriptor> descriptors::rhistogrid(const std::vector<detectors::Point>& points,
                                                             const std::pair<Img, Img>& sobel,
                                                             const NormalizeFunction& norm, int histoSize,
                                                             int histoNums, int bins, borders::BorderTypes border) {
    std::vector<Descriptor> descriptors;
    descriptors.reserve(points.size());

    for(const auto &point : points) {
        for(const auto &unnormalized : rhistogrid(point, sobel, histoSize, histoNums, bins, border)) {
            descriptors.push_back(std::move(norm(unnormalized)));
        }
    }

    return descriptors;
}

std::vector<descriptors::Descriptor> descriptors::shistogrid(const detectors::SPoint& point,
                                                             const std::pair<Img, Img>& sobel, int histoSize,
                                                             int histoNums, int bins, borders::BorderTypes border) {
    std::vector<Descriptor> descriptors;
    auto scaleHistoSize = histoSize * (int) std::roundf(point.sigma);
    auto scaleBlockSize = scaleHistoSize * histoNums;
    auto base = histogrid(point, sobel, .0f, histoSize * histoNums, 1, 36, ORI_SIGMA_C * point.sigma, border);
    auto peaksIndexes = peaks(base, ORI_PEAK_RATIO, 2);

    for(auto index : peaksIndexes) {
        descriptors.push_back(histogrid(point, sobel, _parabolic3bfit(base, index) * 2 * M_PI / base.size, scaleHistoSize,
                                        histoNums, bins, MAGNITUDE_SIGMA_C * scaleBlockSize, border));
    }

    return descriptors;
}

std::vector<descriptors::Descriptor> descriptors::shistogrid(const std::vector<detectors::SPoint>& points,
                                                             const std::vector<pyramids::Octave>& gpyramid,
                                                             const NormalizeFunction& norm, int histoSize, int histoNums,
                                                             int bins, borders::BorderTypes border) {
    std::vector<Descriptor> descriptors;
    descriptors.reserve(points.size());

    for(auto ptIt = std::begin(points), end = std::end(points); ptIt != end;) {
        auto o = ptIt->octave, l = ptIt->layer;
        auto sobel = filters::sobel(gpyramid[o].layers()[l].img, border);

        for(;o == ptIt->octave && l == ptIt->layer && ptIt != end; ptIt++) {
            for(const auto &unnormalized : shistogrid(*ptIt, sobel, histoSize, histoNums, bins, border)) {
                descriptors.push_back(std::move(norm(unnormalized)));
            }
        }
    }

    return descriptors;
}

descriptors::Descriptor descriptors::normalize(Descriptor descriptor) {
    auto first = descriptor.data.get();
    auto last = descriptor.data.get() + descriptor.size;

    auto sumSq = std::accumulate(first, last, .0f, [] (auto accumulator, auto value) {
        return accumulator + value * value;
    });
    auto length = std::sqrt(sumSq);

    std::transform(first, last, first, [length] (auto value) {
        return value / length;
    });

    return descriptor;
}

descriptors::Descriptor descriptors::trim(Descriptor descriptor, float threshold) {
    auto first = descriptor.data.get();
    auto last = descriptor.data.get() + descriptor.size;

    std::transform(first, last, first, [threshold] (auto value) {
        return std::min(value, threshold);
    });

    return descriptor;
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
