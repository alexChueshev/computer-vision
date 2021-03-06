#ifndef COMPUTER_VISION_DESCRIPTORS_TPP
#define COMPUTER_VISION_DESCRIPTORS_TPP

#include <detectors.h>

#include <type_traits>
#include <numeric>

namespace pi::descriptors {
    template<typename T, class Enable = void>
    struct Descriptor;

    template<typename T>
    Descriptor<T> normalize(Descriptor<T> descriptor);

    template<typename T>
    Descriptor<T> trim(Descriptor<T> descriptor, float threshold = .2f);

    template<typename T>
    float distance(const Descriptor<T>& descriptor1, const Descriptor<T>& descriptor2);

    template<typename T>
    std::vector<int> peaks(const Descriptor<T>& descriptor, float threshold = .8f, int nums = 2);

    template<typename T, typename U>
    std::vector<std::pair<T, T>> match(const std::vector<Descriptor<U>>& descriptors1, const std::vector<Descriptor<U>>& descriptors2,
                                       const std::function<std::pair<T, T>(Descriptor<U>, Descriptor<U>)>& op, float threshold = .7f);

    template<typename Base, typename Derived>
    using MatchResolvedType = typename std::enable_if_t<std::is_base_of<Base, Derived>::value, std::vector<std::pair<Derived, Derived>>>;

    template<typename T, typename U> MatchResolvedType<detectors::Point, T>
    match(const std::vector<Descriptor<U>>& descriptors1, const std::vector<Descriptor<U>>& descriptors2, float threshold = .7f);

    template<typename T, typename U> MatchResolvedType<Descriptor<U>, T>
    match(const std::vector<Descriptor<U>>& descriptors1, const std::vector<Descriptor<U>>& descriptors2, float threshold = .7f);
}

template<typename T>
struct pi::descriptors::Descriptor<T, typename std::enable_if_t<
       std::is_base_of<pi::detectors::Point, T>::value>> {
    int size;
    T point;
    std::unique_ptr<float[]> data;

    Descriptor() = default;

    Descriptor(T point, int size)
        : size(size)
        , point(std::move(point))
        , data(std::make_unique<float[]>(size))
    {
        std::fill(data.get(), data.get() + size, 0);
    }

    Descriptor(const Descriptor& descriptor)
        : size(descriptor.size)
        , point(descriptor.point)
        , data(std::make_unique<float[]>(size))
    {
        std::copy(descriptor.data.get(), descriptor.data.get() + size, data.get());
    }

    Descriptor(Descriptor&& descriptor) = default;

    Descriptor& operator=(const Descriptor& descriptor) {
        if(this != &descriptor) {
            point = descriptor.point;
            size = descriptor.size;

            data = std::make_unique<float[]>(size);
            std::copy(descriptor.data.get(), descriptor.data.get() + size, data.get());
        }
        return *this;
    }

    Descriptor& operator=(Descriptor&& descriptor) = default;

    Descriptor(T point, int size, std::unique_ptr<float[]> data)
        : size(size)
        , point(std::move(point))
        , data(std::move(data))
    {
    }

    ~Descriptor() = default;
};

template<typename T>
pi::descriptors::Descriptor<T> pi::descriptors::normalize(Descriptor<T> descriptor) {
    auto first = descriptor.data.get();
    auto last = descriptor.data.get() + descriptor.size;

    auto sumSq = std::accumulate(first, last, .0f, [] (auto accumulator, auto value) {
        return accumulator + value * value;
    });
    auto length = std::sqrt(sumSq);

    std::transform(first, last, first, [length] (auto value) {
        return value / length;
    });

    return descriptor;
}

template<typename T>
pi::descriptors::Descriptor<T> pi::descriptors::trim(Descriptor<T> descriptor, float threshold) {
    auto first = descriptor.data.get();
    auto last = descriptor.data.get() + descriptor.size;

    std::transform(first, last, first, [threshold] (auto value) {
        return std::min(value, threshold);
    });

    return descriptor;
}

template<typename T>
std::vector<int> pi::descriptors::peaks(const Descriptor<T>& descriptor, float threshold, int nums) {
    std::vector<int> positions;

    auto maxValue = std::max_element(descriptor.data.get(), descriptor.data.get() + descriptor.size);
    auto fIndex = std::distance(descriptor.data.get(), maxValue);
    positions.push_back(fIndex);

    for(auto i = 0; i < descriptor.size && positions.size() < nums; i++) {
        if(descriptor.data[i] >= *maxValue * threshold && i != fIndex) {
            positions.push_back(i);
        }
    }

    return positions;
}

template<typename T>
float pi::descriptors::distance(const Descriptor<T>& descriptor1, const Descriptor<T>& descriptor2) {
    assert(descriptor1.size == descriptor2.size);

    auto distance = .0f;
    for(auto i = 0; i < descriptor1.size; i++) {
        distance += std::pow(descriptor1.data[i] - descriptor2.data[i], 2);
    }

    return distance;
}

template<typename T, typename U>
std::vector<std::pair<T, T>> pi::descriptors::match(const std::vector<Descriptor<U>>& descriptors1,
                                                    const std::vector<Descriptor<U>>& descriptors2,
                                                    const std::function<std::pair<T, T>(Descriptor<U>, Descriptor<U>)>& op,
                                                    float threshold) {
    std::vector<std::pair<T, T>> matches;

    for(const auto &descriptor1 : descriptors1) {
        auto minDistance1 = FLT_MAX, minDistance2 = FLT_MAX;
        auto index = 0, counter = 0;

        for(const auto &descriptor2 : descriptors2) {
            auto distance = descriptors::distance(descriptor1, descriptor2);
            if(distance < minDistance1) {
                minDistance2 = minDistance1;
                minDistance1 = distance;
                index = counter;
            } else if(distance < minDistance2) {
                minDistance2 = distance;
            }
            counter++;
        }

        if(minDistance1 / minDistance2 <= threshold) {
            matches.push_back(op(descriptor1, descriptors2[index]));
        }
    }

    return matches;
}

template<typename T, typename U> pi::descriptors::MatchResolvedType<pi::detectors::Point, T>
pi::descriptors::match(const std::vector<Descriptor<U>>& descriptors1, const std::vector<Descriptor<U>>& descriptors2,
                       float threshold) {
    return match<T, U>(descriptors1, descriptors2, [](const auto& d1, const auto& d2) {
        return std::pair<T, T>(d1.point, d2.point);
    }, threshold);
}

template<typename T, typename U> pi::descriptors::MatchResolvedType<pi::descriptors::Descriptor<U>, T>
pi::descriptors::match(const std::vector<Descriptor<U>>& descriptors1, const std::vector<Descriptor<U>>& descriptors2,
                       float threshold) {
    return match<T, U>(descriptors1, descriptors2, [](auto d1, auto d2) {
        return std::pair<T, T>(std::move(d1), std::move(d2));
    }, threshold);
}

#endif // COMPUTER_VISION_DESCRIPTORS_TPP
