#include "operations.h"

using namespace pi;

void opts::grayscale(Img& src) {
    assert(src.channels() == 3);

    Img graycale(src.height(), src.width(), 1);

    auto* dstData = graycale.data();
    auto* srcData = src.data();
    auto channels = src.channels();

    for(auto i = 0, size = src.imageSize(); i < size; i++) {
        dstData[i] = (.299f * srcData[channels * i + 2] +
                      .587f * srcData[channels * i + 1] +
                      .114f * srcData[channels * i]) / 255;
    }

    src = std::move(graycale);
}

void opts::normalize(Img& src) {
    assert(src.channels() == 1);
    assert(src.height() > 0 && src.width() > 0);

    //find max, min values for one-channel image
    auto* data = src.data();
    auto minmax = std::minmax_element(data, data + src.dataSize());
    auto min = *minmax.first;
    auto max = *minmax.second;

    //normalize
    for(auto i = 0, size = src.dataSize(); i < size; i++) {
        data[i] = (data[i] - min) / (max - min);
    }
}

void opts::scale(Img& src) {
    assert(src.channels() == 1);

    Img scaled(src.height() / 2, src.width() / 2, src.channels());

    for(auto i = 0, height = scaled.height(); i < height; i++) {
        for(auto j = 0, width = scaled.width(); j < width; j++) {
            *scaled.at(i, j) = (*src.at(i * 2, j * 2)
                             + *src.at(i * 2, j * 2 + 1)
                             + *src.at(i * 2 + 1, j * 2)
                             + *src.at(i * 2 + 1, j * 2 +1)) / 4.f;
        }
    }

    src = std::move(scaled);
}
