#include <operations.h>

using namespace pi;

Img opts::grayscale(const Img& src) {
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

    return graycale;
}

Img opts::normalize(const Img& src) {
    assert(src.channels() == 1);
    assert(src.height() > 0 && src.width() > 0);

    Img normalized(src.height(), src.width(), 1);

    auto* dataNormalized = normalized.data();
    auto* dataSrc = src.data();

    //find max, min values for one-channel image
    auto minmax = std::minmax_element(dataSrc, dataSrc + src.dataSize());
    auto min = *minmax.first;
    auto max = *minmax.second;

    //normalize
    for(auto i = 0, size = src.dataSize(); i < size; i++) {
        dataNormalized[i] = (dataSrc[i] - min) / (max - min);
    }

    return normalized;
}

Img opts::scale(const Img& src) {
    assert(src.channels() == 1);

    Img scaled(src.height() / 2, src.width() / 2, 1);

    for(auto i = 0, height = scaled.height(); i < height; i++) {
        for(auto j = 0, width = scaled.width(); j < width; j++) {
            *scaled.at(i, j) = (*src.at(i * 2, j * 2)
                             + *src.at(i * 2, j * 2 + 1)
                             + *src.at(i * 2 + 1, j * 2)
                             + *src.at(i * 2 + 1, j * 2 +1)) / 4.f;
        }
    }

    return scaled;
}

Img opts::difference(const Img& src1, const Img& src2) {
    assert(src1.width() == src2.width());
    assert(src1.height() == src2.height());

    Img img(src1.height(), src1.width(), src1.channels());

    auto* dataSrc1 = src1.data();
    auto* dataSrc2 = src2.data();
    auto* dataImg = img.data();
    std::transform(dataSrc1, dataSrc1 + src1.dataSize(), dataSrc2, dataImg, std::minus<float>());

    return img;
}
