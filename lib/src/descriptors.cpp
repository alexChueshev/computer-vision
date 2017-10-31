#include <descriptors.h>

using namespace pi;

descriptors::Descriptor::Descriptor(detectors::Point point, int size)
    : point(std::move(point))
    , size(size)
    , data(std::make_unique<float[]>(size))
{
    std::fill(data.get(), data.get() + size, 0);
}

descriptors::Descriptor::Descriptor(const descriptors::Descriptor& descriptor)
    : point(descriptor.point)
    , size(descriptor.size)
    , data(std::make_unique<float[]>(size))
{
    std::copy(descriptor.data.get(), descriptor.data.get() + size, data.get());
}

descriptors::Descriptor& descriptors::Descriptor::operator=(const descriptors::Descriptor& descriptor) {
    if(this != &descriptor) {
        point = descriptor.point;
        size = descriptor.size;

        data = std::make_unique<float[]>(size);
        std::copy(descriptor.data.get(), descriptor.data.get() + size, data.get());
    }
    return *this;
}

descriptors::Descriptor descriptors::hog(const detectors::Point& point, const std::pair<Img, Img>& sobel,
                                         int histoSize, int blockSize, int bins, borders::BorderTypes border) {
    assert(blockSize % histoSize == 0);

    auto bandwidth = 2 * M_PI / bins;
    auto histoNums = blockSize / histoSize;
    Descriptor descriptor(point, histoNums * histoNums * bins);

    auto fBorder = borders::get(border);
    auto gaussian = kernels::gaussian2d(std::log10(blockSize), blockSize);

    for(auto row = 0; row < blockSize; row++) {
        for(auto col = 0; col < blockSize; col++) {
            auto rI = point.row + row - blockSize / 2;
            auto rC = point.col + col - blockSize / 2;

            auto magnitudeVal = *gaussian.at(row, col) * filters::magnitudeVal(
                        fBorder(rI, rC, sobel.first),
                        fBorder(rI, rC, sobel.second));
            auto phi = filters::phiVal(
                        fBorder(rI, rC, sobel.first),
                        fBorder(rI, rC, sobel.second)) + M_PI;

            auto histoNum = row / histoSize * histoNums + col / histoSize;
            auto lBin = ((int) (phi / bandwidth)) % bins;
            auto rProportion = (phi - bandwidth * lBin) / bandwidth;

            descriptor.data[histoNum * bins + lBin] += (1 - rProportion) * magnitudeVal;
            descriptor.data[histoNum * bins + (lBin + 1) % bins] += rProportion * magnitudeVal;
        }
    }

    return descriptor;
}

template<typename Functor, typename ...Args>
std::vector<descriptors::Descriptor> descriptors::asDescriptors(const std::vector<detectors::Point>& points,
                                                                Functor&& func, Args&&... args) {
    std::vector<Descriptor> descriptors(points.size());

    for(const auto &point : points) {
        func(point, std::forward(args)...);
    }

    return descriptors;
}
