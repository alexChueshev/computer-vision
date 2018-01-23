#include <transforms.h>

using namespace pi;

namespace {
    std::vector<float> _dltHomography(const std::vector<transforms::PPairs>& pairs, int width = 9) {
        assert(pairs.size() >= 4);

        auto size = pairs.size();
        std::vector<float> h;
        h.reserve(width);

        auto* A = gsl_matrix_alloc(2 * size, width);
        auto* B = gsl_matrix_alloc(width, 2 * size);
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

        auto gslH = gsl_matrix_column(U, gsl_vector_min_index(S)).vector;
        auto div = gsl_vector_get(&gslH, width - 1);
        for(auto i = 0; i < width; i++) {
            h.push_back(gsl_vector_get(&gslH, i) / div);
        }

        gsl_matrix_free(A);
        gsl_matrix_free(B);
        gsl_matrix_free(U);

        gsl_vector_free(S);
        gsl_vector_free(T);

        return h;
    }
}

void transforms::homography(const std::vector<PPairs>& matches, int iters) {
    assert(matches.size() >= 4);

    std::mt19937 rand{std::random_device{}()};

    for(auto i = 0; i < iters; i++) {
        std::vector<PPairs> pairs;
        std::sample(std::begin(matches), std::end(matches), std::back_inserter(pairs), 4, rand);

    }
}
