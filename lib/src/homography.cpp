#include <homography.h>

using namespace pi;

namespace {
    using namespace transforms;
    using namespace transforms::t_homography;

    Transform2d _dltHomography(const std::vector<PPairs>& pairs) {
        assert(pairs.size() >= 4);

        auto size = pairs.size();
        auto width = H_SIZE * H_SIZE;

        auto* A = gsl_matrix_alloc(2 * size, width);
        auto* B = gsl_matrix_alloc(width, width);
        auto* U = gsl_matrix_alloc(width, width);

        auto* S = gsl_vector_alloc(width);
        auto* T = gsl_vector_alloc(width);

        for(auto i = 0; i < size; i++) {
            const auto &pair = pairs[i];
            const auto &fPoint = pair.first;
            const auto &sPoint = pair.second;

            float row1[] { fPoint.col, fPoint.row
                                       , 1, 0, 0, 0
                                       , -sPoint.col * fPoint.col
                                       , -sPoint.col * fPoint.row
                                       , -sPoint.col };
            float row2[] { 0, 0, 0, fPoint.col, fPoint.row, 1
                                       , -sPoint.row * fPoint.col
                                       , -sPoint.row * fPoint.row
                                       , -sPoint.row };

            for(auto j = 0; j < width; j++) {
                gsl_matrix_set(A, i * 2, j, row1[j]);
                gsl_matrix_set(A, i * 2 + 1, j, row2[j]);
            }
        }

        gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, A, A, 0.0, B);
        gsl_linalg_SV_decomp(B, U, S, T);

        Transform2d h;
        auto gslH = gsl_matrix_column(U, gsl_vector_min_index(S)).vector;
        auto div = gsl_vector_get(&gslH, width - 1);

        for(auto i = 0; i < H_SIZE; i++) {
            for(auto j = 0; j < H_SIZE; j++) {
                h[i][j] = gsl_vector_get(&gslH, H_SIZE * i + j) / div;
            }
        }

        gsl_matrix_free(A);
        gsl_matrix_free(B);
        gsl_matrix_free(U);

        gsl_vector_free(S);
        gsl_vector_free(T);

        return h;
    }

    std::vector<PPairs> _inliers(const Transform2d& h, const std::vector<PPairs>& matches, float threshold) {
        std::vector<PPairs> inliers;

        auto mul = [](const Transform1d& h, int x, int y, int d) {
            return h[0] * x + h[1] * y + h[2] * d;
        };

        for(const auto &pair : matches) {
            auto &fPoint = pair.first;
            auto &sPoint = pair.second;

            auto div = mul(h[2], fPoint.col, fPoint.row, 1);
            auto sX = mul(h[0], fPoint.col, fPoint.row, 1) / div;
            auto sY = mul(h[1], fPoint.col, fPoint.row, 1) / div;

            if(std::hypot(sX - sPoint.col, sY - sPoint.row) < threshold) {
                inliers.push_back(pair);
            }
        }

        return inliers;
    }
}

transforms::Transform2d transforms::homography(const std::vector<PPairs>& matches, float threshold, int iters) {
    assert(matches.size() >= 4);

    std::vector<PPairs> pairs;
    std::vector<PPairs> inliers;
    auto bestInliersCount = inliers.size();
    std::mt19937 rand{std::random_device{}()};

    for(auto i = 0; i < iters; i++) {
        pairs.clear();
        std::sample(std::begin(matches), std::end(matches), std::back_inserter(pairs), 4, rand);

        auto tmp = _inliers(_dltHomography(pairs), matches, threshold);
        auto tmpSize = tmp.size();

        if(tmpSize > bestInliersCount) {
            bestInliersCount = tmpSize;
            inliers.swap(tmp);
        }
    }

    return _dltHomography(inliers);
}
