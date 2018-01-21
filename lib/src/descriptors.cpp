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

    std::pair<int, float> _linearBinsInterpolation(float phi, float bandwidth, int bins) {
        auto clbin = (phi / bandwidth) - .5f;
        auto distance = phi - bandwidth * (std::floor(clbin) + std::copysignf(.5f, clbin));
        auto lbin = clbin < 1e-8 ? bins - 1 : (int) floor(clbin);

        return std::pair<int, float>(lbin, (1 - distance / bandwidth));
    }
}

descriptors::Descriptor::Descriptor(detectors::Point point, int size)
    : size(size)
    , point(std::move(point))
    , data(std::make_unique<float[]>(size))
{
    std::fill(data.get(), data.get() + size, 0);
}

descriptors::Descriptor::Descriptor(detectors::Point point, int size, std::unique_ptr<float[]> data)
    : size(size)
    , point(std::move(point))
    , data(std::move(data))
{
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

std::unique_ptr<float[]> descriptors::histogrid(const std::pair<Img, Img>& sobel, int pR, int pC, float angle,
                                                int histoSize, int histoNums, int bins, float sigma,
                                                borders::BorderTypes border, bool is3LInterp) {
    auto bandwidth = 2 * M_PI / bins;
    auto blockSize = histoSize * histoNums;
    auto hBlockSize = blockSize / 2;

    auto descriptorSize = histoNums * histoNums * bins;
    auto descriptor = std::make_unique<float[]>(descriptorSize);
    std::fill(descriptor.get(), descriptor.get() + descriptorSize, 0);

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

            auto dx = fBorder(pR + rR, pC + rC, sobel.first);
            auto dy = fBorder(pR + rR, pC + rC, sobel.second);
            auto magnitudeVal = *gaussian.at(row, col) * filters::magnitudeVal(dx, dy);
            auto phi = filters::phiVal(dx, dy) + M_PI - angle;

            while(phi < 0) {
                phi += 2 * M_PI;
            }
            while(phi >= 2 * M_PI) {
                phi -= 2 * M_PI;
            }

            auto r0 = r / histoSize;
            auto c0 = c / histoSize;

            if(histoNums > 1 && is3LInterp) {
                for(auto rI = 0; rI <= 1; rI++) {
                    auto rh = r0 + rI;
                    rh = (rh < 0 || rh >= histoNums)? rh % histoNums : rh;
                    auto rw = 1 - std::fabs(r - rh * histoSize + (double) histoSize / 2) / histoSize ;

                    for(auto cI = 0; cI <= 1; cI++) {
                        auto ch = c0 + cI;
                        ch = (ch < 0 || ch >= histoNums)? ch % histoNums : ch;
                        auto cw = 1 - std::fabs(c - ch * histoSize + (double) histoSize / 2) / histoSize;

                        auto weight = rw * cw;
                        auto interp = _linearBinsInterpolation(phi, bandwidth, bins);
                        auto histoBin = (rh * histoNums + ch) * bins;

                        descriptor[histoBin + interp.first] += weight * interp.second * magnitudeVal;
                        descriptor[histoBin + (interp.first + 1) % bins] += weight * (1 - interp.second) * magnitudeVal;
                    }
                }
            } else {
                auto interp = _linearBinsInterpolation(phi, bandwidth, bins);
                auto histoBin = (r0 * histoNums + c0) * bins;

                descriptor[histoBin + interp.first] += interp.second * magnitudeVal;
                descriptor[histoBin + (interp.first + 1) % bins] += (1 - interp.second) * magnitudeVal;
            }
        }
    }

    return descriptor;
}

std::vector<descriptors::Descriptor> descriptors::histogrid(const std::vector<detectors::Point>& points,
                                                            const std::pair<Img, Img>& sobel, const NormalizeFunction& norm,
                                                            int histoSize, int histoNums, int bins,
                                                            borders::BorderTypes border, bool is3LInterp) {
    std::vector<Descriptor> descriptors;
    descriptors.reserve(points.size());
    auto blockSize = histoSize * histoNums;
    auto sigma = std::log10(blockSize);
    auto descriptorSize = histoNums * histoNums * bins;

    for(const auto &point : points) {
        descriptors.push_back(Descriptor(point, descriptorSize, histogrid(sobel, point.row, point.col
                                                                          , 0, histoSize, histoNums, bins, sigma
                                                                          , border, is3LInterp)));
    }

    return descriptors;
}

std::vector<descriptors::Descriptor> descriptors::rhistogrid(const detectors::Point& point, const std::pair<Img, Img>& sobel,
                                                             int histoSize, int histoNums, int bins,
                                                             borders::BorderTypes border, bool is3LInterp) {
    std::vector<Descriptor> descriptors;
    auto blockSize = histoNums * histoSize;
    auto descriptorSize = histoNums * histoNums * bins;
    auto sigma = 5 * std::log10(blockSize);

    Descriptor base(point, 36, histogrid(sobel, point.row, point.col, .0f, blockSize, 1, 36, sigma, border));

    auto peaksIndexes = peaks(base, ORI_PEAK_RATIO, 2);

    for(auto index : peaksIndexes) {
        descriptors.push_back(Descriptor(point, descriptorSize, histogrid(sobel, point.row, point.col
                                                                          , _parabolic3bfit(base, index) * 2 * M_PI / base.size
                                                                          , histoSize, histoNums, bins, sigma
                                                                          , border, is3LInterp)));
    }

    return descriptors;
}

std::vector<descriptors::Descriptor> descriptors::rhistogrid(const std::vector<detectors::Point>& points,
                                                             const std::pair<Img, Img>& sobel,
                                                             const NormalizeFunction& norm, int histoSize,
                                                             int histoNums, int bins, borders::BorderTypes border,
                                                             bool is3LInterp) {
    std::vector<Descriptor> descriptors;
    descriptors.reserve(points.size());

    for(const auto &point : points) {
        for(const auto &unnormalized : rhistogrid(point, sobel, histoSize, histoNums, bins, border, is3LInterp)) {
            descriptors.push_back(norm(unnormalized));
        }
    }

    return descriptors;
}

std::vector<descriptors::Descriptor> descriptors::shistogrid(const detectors::SPoint& point,
                                                             const std::pair<Img, Img>& sobel, int histoSize,
                                                             int histoNums, int bins, borders::BorderTypes border,
                                                             bool is3LInterp) {
    std::vector<Descriptor> descriptors;
    auto scaleHistoSize = histoSize * (int) std::roundf(point.sigma);
    auto scaleBlockSize = scaleHistoSize * histoNums;
    auto descriptorSize = histoNums * histoNums * bins;

    Descriptor base(point, 36, histogrid(sobel, point.localRow, point.localCol
                                         , .0f, 3 * ORI_SIGMA_C * point.sigma
                                         , 1, 36, ORI_SIGMA_C * point.sigma, border));

    auto peaksIndexes = peaks(base, ORI_PEAK_RATIO, 2);

    for(auto index : peaksIndexes) {
        descriptors.push_back(Descriptor(point, descriptorSize, histogrid(sobel, point.localRow, point.localCol
                                                                          , _parabolic3bfit(base, index) * 2 * M_PI / base.size
                                                                          , scaleHistoSize, histoNums, bins
                                                                          , MAGNITUDE_SIGMA_C * scaleBlockSize
                                                                          , border, is3LInterp)));
    }

    return descriptors;
}

std::vector<descriptors::Descriptor> descriptors::shistogrid(const std::vector<detectors::SPoint>& points,
                                                             const std::vector<pyramids::Octave>& gpyramid,
                                                             const NormalizeFunction& norm, int histoSize, int histoNums,
                                                             int bins, borders::BorderTypes border, bool is3LInterp) {
    std::vector<Descriptor> descriptors;
    descriptors.reserve(points.size());

    for(auto ptIt = std::begin(points), end = std::end(points); ptIt != end;) {
        auto o = ptIt->octave, l = ptIt->layer;
        auto sobel = filters::sobel(gpyramid[o].layers()[l].img, border);

        for(;o == ptIt->octave && l == ptIt->layer && ptIt != end; ptIt++) {
            for(const auto &unnormalized : shistogrid(*ptIt, sobel, histoSize, histoNums, bins, border, is3LInterp)) {
                descriptors.push_back(norm(unnormalized));
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

    return distance;
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
