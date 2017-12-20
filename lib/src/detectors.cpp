#include <detectors.h>

using namespace pi;

namespace {
    std::vector<detectors::Point> extractPoints(const Img& src, int patchShift, float threshold,
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

    return extractPoints(dst, patchShift, threshold, fBorder);
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
    auto hSize = patchSize / 2;

    for(auto row = 0, height = src.height(); row < height; row++) {
        for(auto col = 0, width = src.width(); col < width; col++) {
            auto A = 0.f, B = 0.f, C = 0.f;

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
            *dst.at(row, col) = A * C - B * B - k * (A + C) * (A + C);
        }
    }

    return extractPoints(dst, patchSize / 2, threshold, fBorder);
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
