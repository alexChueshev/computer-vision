#include <transforms.h>

using namespace pi;

namespace {
    int _index(int x, int y, int a, int sc, int yBins, int orntBins, int scBins) {
        auto subWidth = orntBins * scBins;
        return x * yBins * subWidth + y * subWidth + a * scBins + sc;
    }

    int _size(int xBins, int yBins, int orntBins, int scBins) {
        return xBins * yBins * orntBins * scBins;
    }
}

transforms::Hypotheses<detectors::SPoint> transforms::hough(Size imgSize, Size objSize,
                                                            const std::vector<SPPairs>& pairs, float scMin,
                                                            int scBins, float scFactor, float lcCoeff, int orntBins) {
    assert(imgSize.width > 0 && imgSize.height > 0);
    assert(objSize.width > 0 && objSize.height > 0);
    assert(pairs.size() >= 3);
    assert(scMin > 0 && scBins > 0 && scFactor > 0);
    assert(lcCoeff > 0 && orntBins > 0);

    auto cX = (float) objSize.width / 2;
    auto cY = (float) objSize.height / 2;

    auto scales = std::make_unique<float[]>(scBins);
    auto firstScale = scales.get(), lastScale = scales.get() + scBins;
    std::transform(firstScale, lastScale, firstScale, [&firstScale, scMin, scFactor](auto &val) {
        return scMin * std::pow(scFactor, std::distance(firstScale, &val));
    });

    auto orntBandwidth = 2 * M_PI / orntBins;
    auto lcBandwidth = std::max(objSize.width, objSize.height) * lcCoeff;
    auto xBins = (int) std::ceil(imgSize.width / lcBandwidth),
         yBins = (int) std::ceil(imgSize.height / lcBandwidth);

    const auto size = _size(xBins, yBins, orntBins, scBins);
    std::unique_ptr<int[]> bins(new int[size]);
    std::vector<std::pair<int, int>> refs;
    std::fill(bins.get(), bins.get() + size, 0);

    for(auto pair = std::begin(pairs), end = std::end(pairs); pair != end; pair++) {
        const auto &fPoint = pair->first;
        const auto &sPoint = pair->second;

        auto angle = sPoint.angle - fPoint.angle;
        while(angle < 0) angle += 2 * M_PI;
        auto aCos = std::cos(angle), aSin = std::sin(angle);

        auto scale = sPoint.sigmaGlobal / fPoint.sigmaGlobal;
        auto x1 = fPoint.col - cX, y1 = fPoint.row - cY;
        auto x2 = sPoint.col - scale * (x1 * aCos - y1 * aSin),
             y2 = sPoint.row - scale * (x1 * aSin + y1 * aCos);

        auto xBin = x2 / lcBandwidth, yBin = y2 / lcBandwidth;
        auto lxBin = (int) std::floor(xBin), lyBin = (int) std::floor(yBin);

        auto aBin = angle / orntBandwidth;
        auto laBin = (int) std::floor(aBin);

        auto scBinVal = std::find_if(firstScale, lastScale, [scale, scFactor](auto level) {
            return level <= scale && scale < level * scFactor;
        });
        auto lscBin = std::distance(firstScale, *&scBinVal);
        if(lscBin >= scBins) continue;

        for(auto ix = 0; ix <= 1; ix++) {
            auto ixBin = lxBin + (xBin - lxBin >= .5f ?  1 : -1) * ix;
            if(ixBin < 0 || ixBin >= xBins) continue;

            for(auto iy = 0; iy <= 1; iy++) {
                auto iyBin = lyBin + (yBin - lyBin >= .5f ?  1 : -1) * iy;
                if(iyBin < 0 || iyBin >= yBins) continue;

                for(auto ia = 0; ia <= 1; ia++) {
                    auto iaBin = laBin + (aBin - laBin >= .5f ?  1 : -1) * ia;
                    if(iaBin < 0 || iaBin >= orntBins) continue;

                    for(auto isc = 0; isc <= 1; isc++) {
                        auto iscBin = lscBin + (((*scBinVal + scFactor * *scBinVal) / 2) <= scale ?  1 : -1) * isc;
                        if(iscBin < 0 || iscBin >= scBins) continue;

                        auto index = _index(ixBin, iyBin, iaBin, iscBin, yBins, orntBins, scBins);
                        bins[index] += 1;
                        refs.emplace_back(index, std::distance(std::begin(pairs), pair));
                    }
                }
            }
        }
    }

    transforms::Hypotheses<detectors::SPoint> hypotheses;
    for(auto i = 0; i < size; i++) {
        if(bins[i] < 3) continue;

        std::vector<SPPairs> voices;
        for(const auto &ref : refs) {
            if(ref.first == i) {
                voices.push_back(pairs[ref.second]);
            }
        }

        auto tmp = transforms::inliers(dltAffine(voices), pairs, lcBandwidth * .25f);
        if(tmp.size() >= 3) {
            hypotheses.emplace_back(dltAffine(tmp), std::move(tmp));
        }
    }

    return hypotheses;
}
