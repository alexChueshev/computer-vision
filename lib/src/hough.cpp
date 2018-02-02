#include <hough.h>

using namespace pi;

namespace {
    struct Bin {
        int xBin;
        int yBin;
        int aBin;
        int scBin;
    };
}

transforms::t_hough::Hough transforms::hough(Size imgSize, Size objSize, const std::vector<t_hough::PPairs>& pairs,
                                             float scMin, int scBins, float scFactor, float lcCoeff, int orntBins) {
    assert(imgSize.width > 0 && imgSize.height > 0);
    assert(objSize.width > 0 && objSize.height > 0);
    assert(pairs.size() > 2);
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

    auto binComparator = [yBins, orntBins, scBins](const auto &b1, const auto &b2) {
        auto subWidth = orntBins * scBins;
        return b1.xBin * yBins * subWidth + b1.yBin * subWidth + b1.aBin * scBins + b1.scBin
                < b2.xBin * yBins * subWidth + b2.yBin * subWidth + b2.aBin * scBins + b2.scBin;
    };
    std::map<Bin, std::vector<t_hough::PPairs>, decltype (binComparator)> bins(binComparator);

    for(const auto &pair: pairs) {
        const auto &fPoint = pair.first;
        const auto &sPoint = pair.second;

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

        auto scBinVal = std::find_if(firstScale, lastScale, [scale](auto level) { return level >= scale; });
        auto lscBin = std::distance(firstScale, *&scBinVal);
        if(lscBin == scBins) continue;

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

                        Bin bin{ixBin, iyBin, iaBin, iscBin};
                        auto bIt = bins.find(bin);
                        if(bIt != bins.end()) {
                            bIt->second.push_back(pair);
                        } else {
                            bins.insert(std::pair<Bin, std::vector<t_hough::PPairs>>(bin, {pair}));
                        }
                    }
                }
            }
        }
    }

    auto maxBin = std::max_element(std::begin(bins), std::end(bins), [](const auto &b1, const auto &b2) {
        return b1.second.size() < b2.second.size();
    })->first;

    return {(maxBin.xBin + .5f) * lcBandwidth
                , (maxBin.yBin + .5f) * lcBandwidth
                , (maxBin.aBin + .5f) * orntBandwidth
                , (scales[maxBin.scBin] + scales[maxBin.scBin] * scFactor) / 2 };
}
