#include <detectors.h>

using namespace pi;

namespace {
    std::vector<detectors::Point> _extractPoints(const Img& src, int patchShift, float threshold,
                                                 const borders::Function& fBorder) {
        std::vector<detectors::Point> points;

        for(auto row = 0, height = src.height(); row < height; row++) {
            for(auto col = 0, width = src.width(); col < width; col++) {
                auto isMax = true;
                const auto &pixel = *src.at(row, col);

                if(pixel < threshold) continue;

                for(auto kR = -patchShift; kR <= patchShift && isMax; kR++) {
                    for(auto kC = -patchShift; kC <= patchShift; kC++) {
                        if(kR == 0 && kC == 0) continue; //only environs

                        if(pixel <= fBorder(row + kR, col + kC, src)) {
                            isMax = false;
                            break;
                        };
                    }
                }

                if(isMax) {
                    points.push_back({row, col, pixel});
                }
            }
        }

        return points;
    }

    bool _isExtremum(std::initializer_list<std::reference_wrapper<const Img>> images, int r, int c, float value,
                     const borders::Function& fBorder) {
        auto eps = 1e-5f;
        auto min = true, max = true;
        int directions[9][2] = {{-1,-1}, {0,-1}, {1,-1}, {0, 0}, {-1,0}, {1,1}, {1,0}, {-1,1}, {0,1}};

        for(const auto &img : images) {
            for(const auto &direction : directions) {
                auto val = fBorder(r + direction[0], c + direction[1], img);
                if(val - value > eps) max = false;
                if(value - val > eps) min = false;
            }
            if(min == max) return false;
        }
        return min != max;
    };

    std::array<float, 3> _harrisValues(const std::pair<Img, Img>& pDerivatives, const kernels::Kernel& gaussian,
                                       int row, int col, const borders::Function& fBorder) {
        auto A = 0.f, B = 0.f, C = 0.f;
        auto hSize = gaussian.width() / 2;

        for(auto kR = -hSize; kR <= hSize; kR++) {
            for(auto kC = -hSize; kC <= hSize; kC++) {
                auto w = *gaussian.at(kR + hSize, kC + hSize);

                auto pIx = fBorder(row + kR, col + kC, pDerivatives.first);
                auto pIy = fBorder(row + kR, col + kC, pDerivatives.second);

                A += w * pIx * pIx;
                B += w * pIx * pIy;
                C += w * pIy * pIy;
            }
        }

        return {A, B, C};
    }
}

std::vector<detectors::Point> detectors::moravec(const Img& src, int patchSize, float threshold,
                                                 borders::BorderTypes border) {
    assert(src.channels() == 1);
    assert(patchSize > 0 && patchSize % 2 == 1);

    auto patchShift = patchSize / 2;
    auto fBorder = borders::get(border);

    Img dst(src.height(), src.width(), 1);
    int directions[8][2] = {{-1,-1}, {0,-1}, {1,-1}, {-1,0}, {1,1}, {1,0}, {-1,1}, {0,1}};

    for(auto row = 0, height = src.height(); row < height; row++) {
        for(auto col = 0, width = src.width(); col < width; col++) {
            auto errorMin = FLT_MAX;

            for(const auto &direction : directions) {
                auto error = .0f;

                for(auto kR = -patchShift; kR <= patchShift; kR++) {
                    for(auto kC = -patchShift; kC <= patchShift; kC++) {
                        auto value = fBorder(row + kR, col + kC, src)
                                - fBorder(row + kR + direction[0], col + kC + direction[1], src);
                        error += value * value;
                    }
                }

                errorMin = std::min(errorMin, error);
            }

            *dst.at(row, col) = errorMin;
        }
    }

    return _extractPoints(dst, patchShift, threshold, fBorder);
}

std::vector<detectors::Point> detectors::harris(const Img& src, int patchSize, float threshold,
                                                float k, borders::BorderTypes border) {
    assert(src.channels() == 1);
    assert(patchSize > 0 && patchSize % 2 == 1);

    Img dst(src.height(), src.width(), 1);

    auto fBorder = borders::get(border);
    auto pDerivatives = filters::sobel(src, border);

    auto sigma = std::log10(patchSize) * 2;
    auto gaussian = kernels::gaussian2d(sigma, patchSize);

    for(auto row = 0, height = src.height(); row < height; row++) {
        for(auto col = 0, width = src.width(); col < width; col++) {
            *dst.at(row, col) = utils::harris(_harrisValues(pDerivatives, gaussian, row, col, fBorder), k);
        }
    }

    return _extractPoints(dst, patchSize / 2, threshold, fBorder);
}

std::vector<detectors::SPoint> detectors::harris(const std::vector<pyramids::Octave>& dog,
                                                 const std::vector<SPoint>& blobs, float threshold, float k,
                                                 borders::BorderTypes border) {
    std::vector<SPoint> points;
    auto fBorder = borders::get(border);

    for(auto bIt = std::begin(blobs), end = std::end(blobs); bIt != end;) {
        auto o = bIt->octave, l = bIt->layer;
        auto &layer = dog[o].layers()[l];
        auto pDerivatives = filters::sobel(layer.img, border);
        auto gaussian = kernels::gaussian2d(layer.sigma);

        for(;o == bIt->octave && l == bIt->layer && bIt != end; bIt++) {
            auto value = utils::harris(_harrisValues(pDerivatives, gaussian, bIt->localRow, bIt->localCol, fBorder), k);
            if(value > threshold) {
                points.push_back(*bIt);
            }
        }
    }

    return points;
}

std::vector<detectors::SPoint> detectors::shiTomasi(const std::vector<pyramids::Octave>& dog,
                                                    const std::vector<SPoint>& blobs, float threshold,
                                                    borders::BorderTypes border) {
    std::vector<SPoint> points;
    auto fBorder = borders::get(border);

    for(auto bIt = std::begin(blobs), end = std::end(blobs); bIt != end;) {
        auto o = bIt->octave, l = bIt->layer;
        auto &layer = dog[o].layers()[l];
        auto pDerivatives = filters::sobel(layer.img, border);
        auto gaussian = kernels::gaussian2d(layer.sigma, 5);

        for(;o == bIt->octave && l == bIt->layer && bIt != end; bIt++) {
            auto value = utils::shiTomasi(_harrisValues(pDerivatives, gaussian, bIt->localRow, bIt->localCol, fBorder));
            if(value > threshold) {
                points.push_back(*bIt);
            }
        }
    }

    return points;
}

std::vector<detectors::SPoint> detectors::blobs(const std::vector<pyramids::Octave>& dog, float preContrastThreshold,
                                                borders::BorderTypes border) {
    std::vector<SPoint> blobs;
    auto fBorder = borders::get(border);

    for(int i = 0, oSize = dog.size(); i < oSize; i++) {
        auto &layers = dog[i].layers();

        for(int j = 1, lSize = layers.size() - 1; j < lSize; j++) {
            auto &layer = layers[j];

            for(auto r = 0, height = layer.img.height(); r < height; r++) {
                for(auto c = 0, widht = layer.img.width(); c < widht; c++) {
                    auto value = *layer.img.at(r, c);
                    if(std::abs(value) > preContrastThreshold) {
                        if(_isExtremum({layers[j - 1].img, layer.img, layers[j + 1].img}, r, c, value, fBorder)) {
                            blobs.push_back({
                                                 (int)(r * std::pow(2, i)),
                                                 (int)(c * std::pow(2, i)),
                                                 value,
                                                 r, c, i, j,
                                                 layer.sigma,
                                                 layer.sigmaGlobal,
                                              });
                        }
                    }
                }
            }
        }
    }

    return blobs;
}

float detectors::utils::harris(const std::array<float, 3>& values, float k) {

    auto A = values[0], B = values[1], C = values[2];

    return A * C - B * B - k * (A + C) * (A + C);
}

float detectors::utils::shiTomasi(const std::array<float, 3>& values) {
    auto A = values[0], B = values[1], C = values[2];

    auto discriminant = std::sqrt((A - C) * (A - C) + 4 * B * B);
    return std::min(std::abs((A + C - discriminant) / 2), std::abs((A + C + discriminant) / 2));
}

std::vector<detectors::Point> detectors::adaptiveNonMaximumSuppresion(const std::vector<Point>& points,
                                                                      int quantity, float radiusMax,
                                                                      const DistanceFunction& distanceFunction,
                                                                      float coefficient) {
    std::vector<Point> filtered(points);

    for(auto radius = 0; radius <= radiusMax && filtered.size() > quantity; radius++) {
        for(auto i = 0; i < filtered.size(); i++) {
            auto point = filtered[i];
            filtered.erase(std::remove_if(std::begin(filtered) + i + 1, std::end(filtered),
                           [&distanceFunction, &point, radius, coefficient](const Point& p) {
                if(distanceFunction(point.row, p.row, point.col, p.col) < radius
                        && point.value * coefficient > p.value) {
                    return true;
                }
                return false;
            }), std::end(filtered));
        }
    }

    return filtered;
}
