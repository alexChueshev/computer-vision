#include "detectors.h"

using namespace pi;

/***********************
 * Detector methods
 ***********************/
detectors::Detector::Detector(const Img& img)
    : _img(img.clone())
{
}

Img detectors::Detector::addPointsToImage() {
    assert(_img.channels() == 1);

    //convert to 3 ch
    Img tmp(_img.height(), _img.width(), 3);

    auto* dataTmp = tmp.data();
    auto* dataImg = _img.data();
    auto channels = tmp.channels();

    for(auto i = 0, size = _img.dataSize(); i < size; i++) {
        dataTmp[i * channels] = dataImg[i];
        dataTmp[i * channels + 1] = dataImg[i];
        dataTmp[i * channels + 2] = dataImg[i];
    }

    //add points to image
    for(const auto &point : _points) {
        auto* pixel = tmp.at(point.row, point.col);
        *(pixel + 0) = .0f;
        *(pixel + 1) = 1.0f;
        *(pixel + 2) = 1.f; //yellow color
    }

    return tmp;
}

detectors::Detector& detectors::Detector::adaptNonMaximumSuppr(int points,
                                                               const DistanceFunction& distanceFunction) {
    auto radiusMax = std::sqrt(_img.height() * _img.height() + _img.width() * _img.width()) / 2;

    for(auto radius = 0; radius <= radiusMax && _points.size() > points; radius++) {
        for(auto i = 0; i < _points.size(); i++) {
            auto pointCur = _points[i];
            _points.erase(std::remove_if(std::begin(_points) + i + 1, std::end(_points),
                           [&distanceFunction, &radius, &pointCur](const Point& p) {
                if(distanceFunction(pointCur.row, p.row, pointCur.col, p.col) < radius
                        && pointCur.value * ADAPT_NM_SUPR_COEFFICIENT > p.value) {
                    return true;
                }
                return false;
            }), std::end(_points));
        }
    }

    return *this;
}

const std::vector<detectors::Point>& detectors::Detector::points() const {
    return _points;
}

Img detectors::Detector::image() const {
    return _img;
}

void detectors::Detector::applyThreshold(const Img& src, borders::BorderTypes border) {
    auto fBorder = borders::Factory::get(border);
    _points.clear();

    for(auto row = 0, height = src.height(); row < height; row++) {
        for(auto col = 0, width = src.width(); col < width; col++) {
            auto isMax = true;
            const auto &pixel = *src.at(row, col);

            if(pixel < THRESHOLD) continue;

            for(auto kR = -PATCH_SHIFT; kR <= PATCH_SHIFT && isMax; kR++) {
                for(auto kC = -PATCH_SHIFT; kC <= PATCH_SHIFT; kC++) {
                    if(kR == 0 && kC == 0) continue; //only environs

                    if(pixel <= fBorder(row + kR, col + kC, src)) {
                        isMax = false;
                        break;
                    };
                }
            }

            if(isMax) {
                _points.push_back({row, col, pixel});
            }
        }
    }
}

/***********************
 * DetectorMoravec methods
 ***********************/
detectors::DetectorMoravec::DetectorMoravec(const Img& img)
    : Detector(img)
    , _directions({ {{-1,-1}, {0,-1}, {1,-1}, {-1,0}, {1,1}, {1,0}, {-1,1}, {0,1}} })
{
}

detectors::DetectorMoravec& detectors::DetectorMoravec::apply(borders::BorderTypes border) {
    assert(_img.channels() == 1);

    Img dst(_img.height(), _img.width(), _img.channels());

    this->applyPatch(_img, dst, border);
    this->applyThreshold(dst, border);

    return *this;
}

void detectors::DetectorMoravec::applyPatch(const Img& src, Img& dst, borders::BorderTypes border) {
    auto fBorder = borders::Factory::get(border);

    for(auto row = 0, height = src.height(); row < height; row++) {
        for(auto col = 0, width = src.width(); col < width; col++) {
            auto errorMin = FLT_MAX;

            for(const auto &direction : _directions) {
                auto error = .0f;

                for(auto kR = -PATCH_SHIFT; kR <= PATCH_SHIFT; kR++) {
                    for(auto kC = -PATCH_SHIFT; kC <= PATCH_SHIFT; kC++) {
                        auto value = fBorder(row + kR, col + kC, src)
                                - fBorder(row + kR + direction.row, col + kC + direction.col, src);
                        error += value * value;
                    }
                }

                errorMin = std::min(errorMin, error);
            }

            *dst.at(row, col) = errorMin;
        }
    }
}

/***********************
 * DetectorHarris methods
 ***********************/
detectors::DetectorHarris::DetectorHarris(const Img& img, int windowSize, WindowFunction windowFunction)
    : Detector(std::move(img))
    , _windowSize(windowSize)
    , _windowFunction(windowFunction)
{
}

detectors::DetectorHarris& detectors::DetectorHarris::apply(borders::BorderTypes border) {
    assert(_img.channels() == 1);

    Img dst(_img.height(), _img.width(), _img.channels());

    this->applyPatch(_img, dst, border);
    this->applyThreshold(dst, border);

    return *this;
}

void detectors::DetectorHarris::applyPatch(const Img& src, Img& dst, borders::BorderTypes border) {
    assert(_windowSize % 2 == 1);

    auto pDerivativeX = src.clone();
    auto pDerivativeY = src.clone();

    filters::Sobel sobel;
    sobel.applyX(pDerivativeX, border);
    sobel.applyY(pDerivativeY, border);

    auto fBorder = borders::Factory::get(border);
    auto hSize = _windowSize / 2;

    for(auto row = 0, height = src.height(); row < height; row++) {
        for(auto col = 0, width = src.width(); col < width; col++) {
            auto A = 0.f, B = 0.f, C = 0.f;

            for(auto kR = -hSize; kR <= hSize; kR++) {
                for(auto kC = -hSize; kC <= hSize; kC++) {
                    auto w = _windowFunction(kR + hSize, kC + hSize);

                    auto pIx = fBorder(row + kR, col + kC, pDerivativeX);
                    auto pIy = fBorder(row + kR, col + kC, pDerivativeY);

                    A += w * pIx * pIx;
                    B += w * pIx * pIy;
                    C += w * pIy * pIy;
                }
            }
            *dst.at(row, col) = A * C - B * B - K * (A + C) * (A + C);
        }
    }
}
