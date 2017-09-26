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

    //find max, min values
    auto* data = src.data();
    auto min = data[0], max = data[0];
    for(auto i = 1, size = src.dataSize(); i < size; i++) {
        if(data[i] > max) max = data[i];
        if(data[i] < min) min = data[i];
    }

    //normalize
    for(auto i = 0, size = src.dataSize(); i < size; i++) {
        data[i] = (data[i] - min) / (max - min);
    }
}
