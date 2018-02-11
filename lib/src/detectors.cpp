#include <detectors.h>

using namespace pi;

namespace {
    using namespace detectors;
    using namespace detectors::utils;
    using RIInitList = std::initializer_list<std::reference_wrapper<const Img>>;

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

    bool _isExtremum(RIInitList images, int r, int c, float value, const borders::Function& fBorder) {
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

    bool _isEdge(const pi::Img& img, int r, int c, float threshold, const borders::Function& fBorder) {
        auto hessian2d = detectors::utils::hessian2d(img, r, c, fBorder);
        auto tr = hessian2d[0] + hessian2d[3];
        auto det = hessian2d[0] * hessian2d[3] - hessian2d[1] * hessian2d[2];

        return tr * tr / det > std::pow(threshold + 1, 2) / threshold;
    }

    std::array<double, 3> _interpolationSteps(RIArray3d images, int r, int c, const borders::Function& fBorder) {
        int s;
        double hInvData[9], xData[3];
        auto hData = ::hessian3d(images, r, c, fBorder);
        auto pDData = ::pDerivatives3d(images, r, c, fBorder);

        auto* p = gsl_permutation_alloc(3);
        auto x = gsl_vector_view_array(xData, 3);
        auto hessian3dInv = gsl_matrix_view_array(hInvData, 3, 3);
        auto hessian3d = gsl_matrix_view_array(hData.data(), 3, 3);
        auto pDerivatives3d = gsl_vector_view_array(pDData.data(), 3);

        gsl_linalg_LU_decomp(&hessian3d.matrix, p, &s);
        gsl_linalg_LU_invert(&hessian3d.matrix, p, &hessian3dInv.matrix);
        gsl_blas_dgemv(CblasNoTrans, -1.0, &hessian3dInv.matrix, &pDerivatives3d.vector, 0.0, &x.vector);

        gsl_permutation_free(p);

        return { xData[1], xData[0], xData[2] }; //row, col, interval
    }

    double _interpolationContrast(RIArray3d images, int r, int c, const std::array<double, 3>& steps,
                                  const borders::Function& fBorder) {
        double dData[1];
        double xData[] { steps[1], steps[0], steps[2] };
        auto pDData = ::pDerivatives3d(images, r, c, fBorder);

        auto pDerivatives3d = gsl_matrix_view_array(pDData.data(), 3, 1);
        auto x = gsl_vector_view_array(xData, 3);
        auto d = gsl_vector_view_array(dData, 1);

        gsl_blas_dgemv(CblasTrans, .5, &pDerivatives3d.matrix, &x.vector, 0.0, &d.vector);

        return dData[0] + fBorder(r, c, images[1]);
    }

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
                points.push_back(std::move(*bIt));
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
        auto gaussian = kernels::gaussian2d(layer.sigma);

        for(;o == bIt->octave && l == bIt->layer && bIt != end; bIt++) {
            auto value = utils::shiTomasi(_harrisValues(pDerivatives, gaussian, bIt->localRow, bIt->localCol, fBorder));
            if(value > threshold) {
                points.push_back(std::move(*bIt));
            }
        }
    }

    return points;
}

std::vector<detectors::SPoint> detectors::blobs(const std::vector<pyramids::Octave>& dog, float contrastThreshold,
                                                borders::BorderTypes border) {
    std::vector<SPoint> blobs;
    auto fBorder = borders::get(border);

    for(int i = 0, oSize = dog.size(); i < oSize; i++) {
        const auto &layers = dog[i].layers();
        const auto &fLayer = layers.front();

        const auto lSize = layers.size() - 2;
        const auto nContrastThreshold = contrastThreshold / lSize;
        const auto preContrastThreshold = nContrastThreshold * .5f;
        const auto oStep = std::pow(2, i);

        for(int j = 1, size = lSize + 1; j < size; j++) {
            const auto &layer = layers[j];

            for(auto r = 0, height = layer.img.height(); r < height; r++) {
                for(auto c = 0, widht = layer.img.width(); c < widht; c++) {
                    auto value = *layer.img.at(r, c);

                    //1 step: check pre-contrast
                    if(std::abs(value) < preContrastThreshold) continue;

                    //2 step: find extremum
                    if(!_isExtremum({ layers[j - 1].img
                                      , layer.img
                                      , layers[j + 1].img }, r, c, value, fBorder)) continue;

                    //3 step: interpolation
                    bool interpolated = false;
                    auto ir = r, ic = c, il = j;
                    auto xr = .0, xc = .0, xl = .0;
                    for(auto ii = 0; ii < 5; ii++) {
                        auto steps = _interpolationSteps({ layers[il - 1].img
                                                           , layers[il].img
                                                           , layers[il + 1].img
                                                         }, ir, ic, fBorder);
                        xr = steps[0]; xc = steps[1]; xl = steps[2];

                        if(std::fabs(xr) < .5 && std::fabs(xc) < .5 && std::fabs(xl) < .5) {
                            interpolated = true;
                            break;
                        }

                        ir += std::round(xr);
                        ic += std::round(xc);
                        il += std::round(xl);

                        if(il < 1 || il > lSize
                                  || ir < 0 || ir >= fLayer.img.height()
                                  || ic < 0 || ic >= fLayer.img.width()) {
                            break;
                        }
                    }
                    if(!interpolated) continue;

                    //4 step: check contrast
                    auto contrast = _interpolationContrast({ layers[il - 1].img
                                                             , layers[il].img
                                                             , layers[il + 1].img
                                                           }, ir, ic, { xr, xc, xl }, fBorder);
                    if(std::fabs(contrast) < nContrastThreshold) continue;

                    //5 step: check edges
                    if(_isEdge(layers[il].img, ir, ic, 10, fBorder)) continue;

                    //6 step:: add blob
                    const auto ratio = (il + xl) / lSize;
                    blobs.push_back({
                                        std::round((ir + xr) * oStep),
                                        std::round((ic + xc) * oStep),
                                        value, 0,
                                        ir, ic, i, il,
                                        fLayer.sigma * std::pow(2., ratio),
                                        fLayer.sigma * std::pow(2., i + ratio)
                                    });
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

std::array<double, 3> detectors::utils::pDerivatives3d(const RIArray3d images, int r, int c,
                                                       const borders::Function& fBorder) {
    auto dx = (fBorder(r, c + 1, images[1]) - fBorder(r, c - 1, images[1])) / 2.;
    auto dy = (fBorder(r + 1, c, images[1]) - fBorder(r - 1, c, images[1])) / 2.;
    auto ds = (fBorder(r, c, images[2]) - fBorder(r, c, images[0])) / 2.;

    return { dx, dy, ds };
}

std::array<double, 9> detectors::utils::hessian3d(const RIArray3d images, int r, int c,
                                                  const borders::Function& fBorder) {
    auto val = fBorder(r, c, images[1]);
    auto dxx = fBorder(r, c + 1, images[1]) +  fBorder(r, c - 1, images[1]) - 2 * val;
    auto dyy = fBorder(r + 1, c, images[1]) +  fBorder(r - 1, c, images[1]) - 2 * val;
    auto dss = fBorder(r, c, images[0]) +  fBorder(r, c, images[2]) - 2 * val;

    auto dxy = (fBorder(r + 1, c + 1, images[1]) - fBorder(r + 1, c - 1, images[1])
                - fBorder(r - 1, c + 1, images[1]) + fBorder(r - 1, c - 1, images[1])) / 4.;
    auto dxs = (fBorder(r, c + 1, images[2]) - fBorder(r, c - 1, images[2])
                - fBorder(r, c + 1, images[0]) + fBorder(r, c - 1, images[0])) / 4.;
    auto dys = (fBorder(r + 1, c, images[2]) - fBorder(r - 1, c, images[2])
                - fBorder(r + 1, c, images[0]) + fBorder(r - 1, c, images[0])) / 4.;

    return { dxx, dxy, dxs, dxy, dyy, dys, dxs, dys, dss };
}

std::array<double, 4> detectors::utils::hessian2d(const pi::Img& img, int r, int c,
                                                  const borders::Function& fBorder) {
    auto val = fBorder(r, c, img);
    auto dxx = fBorder(r, c + 1, img) +  fBorder(r, c - 1, img) - 2 * val;
    auto dyy = fBorder(r + 1, c, img) +  fBorder(r - 1, c, img) - 2 * val;
    auto dxy = (fBorder(r + 1, c + 1, img) - fBorder(r + 1, c - 1, img)
                - fBorder(r - 1, c + 1, img) + fBorder(r - 1, c - 1, img)) / 4.;

    return { dxx, dxy, dxy, dyy };
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
