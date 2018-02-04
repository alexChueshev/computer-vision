#ifndef COMPUTER_VISION_HOMOGRAPHY_H
#define COMPUTER_VISION_HOMOGRAPHY_H

#include <transforms.tpp>

#include <random>

namespace pi::transforms::t_homography {
    constexpr float H_THRESHOLD = 4.5f;
    constexpr int H_ITERS = 1200;
}

namespace pi::transforms {
    template<typename T>
    Transform2d homography(const std::vector<PPairs<T>>& matches,
                           float threshold = t_homography::H_THRESHOLD,
                           int iters = t_homography::H_ITERS);
}

template<typename T>
pi::transforms::Transform2d pi::transforms::homography(const std::vector<PPairs<T>>& matches,
                                                       float threshold, int iters) {
    assert(matches.size() >= 4);

    std::vector<PPairs<T>> pairs;
    std::vector<PPairs<T>> inliers;
    auto bestInliersCount = inliers.size();
    std::mt19937 rand{std::random_device{}()};

    for(auto i = 0; i < iters; i++) {
        pairs.clear();
        std::sample(std::begin(matches), std::end(matches), std::back_inserter(pairs), 4, rand);

        auto tmp = transforms::inliers(dltHomography(pairs), matches, threshold);
        auto tmpSize = tmp.size();

        if(tmpSize > bestInliersCount) {
            bestInliersCount = tmpSize;
            inliers.swap(tmp);
        }
    }

    return dltHomography(inliers);
}

#endif //COMPUTER_VISION_HOMOGRAPHY_H
