#include <descriptors.h>

using namespace pi;

namespace {
    template<typename T>
    int _parabolic3bfit(const descriptors::Descriptor<T>& descriptor, int peak) {
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
                auto hHistoSize = (double) histoSize / 2;
                auto cr0 = r0 * histoSize + hHistoSize;
                auto cc0 = c0 * histoSize + hHistoSize;

                for(auto rI = 0; rI <= 1; rI++) {
                    auto rh = r0 + ((r < cr0)? -1 : 1) * rI;
                    auto rw = 1 - std::fabs(r - rh * histoSize - hHistoSize) / histoSize ;

                    rh += histoNums;
                    rh = (rh < 0 || rh >= histoNums)? rh % histoNums : rh;

                    for(auto cI = 0; cI <= 1; cI++) {
                        auto ch = c0 + ((c < cc0)? -1 : 1) * cI;
                        auto cw = 1 - std::fabs(c - ch * histoSize - hHistoSize) / histoSize;
                        auto weight = rw * cw;

                        ch += histoNums;
                        ch = (ch < 0 || ch >= histoNums)? ch % histoNums : ch;

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

std::vector<descriptors::BDescriptor> descriptors::bDescriptors(const std::vector<detectors::Point>& points,
                                                                const std::pair<Img, Img>& sobel, const BNormalizeFunction& norm,
                                                                int histoSize, int histoNums, int bins,
                                                                borders::BorderTypes border, bool is3LInterp) {
    std::vector<BDescriptor> descriptors;
    descriptors.reserve(points.size());
    auto blockSize = histoSize * histoNums;
    auto sigma = std::log10(blockSize);
    auto descriptorSize = histoNums * histoNums * bins;

    for(const auto &point : points) {
        descriptors.emplace_back(point, descriptorSize, histogrid(sobel, point.row, point.col, 0, histoSize
                                                                  , histoNums, bins, sigma, border, is3LInterp));
    }

    return descriptors;
}

std::vector<descriptors::RiDescriptor> descriptors::rhistogrid(const detectors::Point& point, const std::pair<Img, Img>& sobel,
                                                               int histoSize, int histoNums, int bins,
                                                               borders::BorderTypes border, bool is3LInterp) {
    std::vector<RiDescriptor> descriptors;
    auto blockSize = histoNums * histoSize;
    auto descriptorSize = histoNums * histoNums * bins;
    auto sigma = 5 * std::log10(blockSize);

    BDescriptor base(point, 36, histogrid(sobel, point.row, point.col, .0f, blockSize, 1, 36, sigma, border));

    auto peaksIndexes = peaks(base, ORI_PEAK_RATIO, 2);

    for(auto index : peaksIndexes) {
        auto angle = _parabolic3bfit(base, index) * 2 * M_PI / base.size;
        detectors::RPoint p{point.row, point.col, point.value, angle};
        descriptors.emplace_back(std::move(p), descriptorSize, histogrid(sobel, point.row, point.col, angle
                                                                        , histoSize, histoNums, bins, sigma
                                                                        , border, is3LInterp));
    }

    return descriptors;
}

std::vector<descriptors::RiDescriptor> descriptors::riDescriptors(const std::vector<detectors::Point>& points,
                                                                  const std::pair<Img, Img>& sobel,
                                                                  const RiNormalizeFunction& norm, int histoSize,
                                                                  int histoNums, int bins, borders::BorderTypes border,
                                                                  bool is3LInterp) {
    std::vector<RiDescriptor> descriptors;
    descriptors.reserve(points.size());

    for(const auto &point : points) {
        for(const auto &unnormalized : rhistogrid(point, sobel, histoSize, histoNums, bins, border, is3LInterp)) {
            descriptors.push_back(norm(unnormalized));
        }
    }

    return descriptors;
}

std::vector<descriptors::SiDescriptor> descriptors::shistogrid(detectors::SPoint point, const std::pair<Img, Img>& sobel, int histoSize,
                                                               int histoNums, int bins, borders::BorderTypes border,
                                                               bool is3LInterp) {
    std::vector<SiDescriptor> descriptors;
    auto scaleHistoSize = histoSize * (int) std::ceil(point.sigma);
    auto scaleBlockSize = scaleHistoSize * histoNums;
    auto descriptorSize = histoNums * histoNums * bins;

    SiDescriptor base(point, 36, histogrid(sobel, point.localRow, point.localCol
                                         , .0f, 3 * ORI_SIGMA_C * point.sigma
                                         , 1, 36, ORI_SIGMA_C * point.sigma, border));

    auto peaksIndexes = peaks(base, ORI_PEAK_RATIO, 2);

    for(auto index : peaksIndexes) {
        auto angle = _parabolic3bfit(base, index) * 2 * M_PI / base.size;
        point.angle = angle;
        descriptors.emplace_back(std::move(point), descriptorSize, histogrid(sobel, point.localRow, point.localCol
                                                                             , angle, scaleHistoSize, histoNums, bins
                                                                             , MAGNITUDE_SIGMA_C * scaleBlockSize
                                                                             , border, is3LInterp));
    }

    return descriptors;
}

std::vector<descriptors::SiDescriptor> descriptors::siDescriptors(const std::vector<detectors::SPoint>& points,
                                                                  const std::vector<pyramids::Octave>& gpyramid,
                                                                  const SiNormalizeFunction& norm, int histoSize, int histoNums,
                                                                  int bins, borders::BorderTypes border, bool is3LInterp) {
    std::vector<SiDescriptor> descriptors;
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
