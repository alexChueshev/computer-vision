#ifndef COMPUTER_VISION_TRANSFORMS_TPP
#define COMPUTER_VISION_TRANSFORMS_TPP

#include <detectors.h>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

namespace pi::transforms {
    constexpr int T_SIZE = 3;

    typedef std::array<float, T_SIZE> Transform1d;
    typedef std::array<Transform1d, T_SIZE> Transform2d;

    template<typename T>
    using PPairs = std::pair<T, T>;

    template<typename T>
    Transform2d dltHomography(const std::vector<PPairs<T>>& pairs);

    template<typename T>
    Transform2d dltAffine(const std::vector<PPairs<T>>& pairs);

    template<typename T>
    std::vector<PPairs<T>> inliers(const Transform2d& h, const std::vector<PPairs<T>>& matches,
                                   float threshold);
}

template<typename T>
pi::transforms::Transform2d pi::transforms::dltHomography(const std::vector<PPairs<T>>& pairs) {
    assert(pairs.size() >= 4);

    auto size = pairs.size();
    auto width = T_SIZE * T_SIZE;

    double dataB[width * width];
    double dataU[width * width];

    auto* A = gsl_matrix_alloc(2 * size, width);
    auto B = gsl_matrix_view_array(dataB, width, width);
    auto U = gsl_matrix_view_array(dataU, width, width);

    double dataS[width];
    double dataP[width];

    auto S = gsl_vector_view_array(dataS, width);
    auto P = gsl_vector_view_array(dataP, width);

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

    gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, A, A, 0.0, &B.matrix);
    gsl_linalg_SV_decomp(&B.matrix, &U.matrix, &S.vector, &P.vector);

    Transform2d h;
    auto gslH = gsl_matrix_column(&U.matrix, gsl_vector_min_index(&S.vector)).vector;
    auto div = gsl_vector_get(&gslH, width - 1);

    for(auto i = 0; i < T_SIZE; i++) {
        for(auto j = 0; j < T_SIZE; j++) {
            h[i][j] = gsl_vector_get(&gslH, T_SIZE * i + j) / div;
        }
    }

    gsl_matrix_free(A);

    return h;
}

template<typename T>
pi::transforms::Transform2d pi::transforms::dltAffine(const std::vector<PPairs<T>>& pairs) {
    assert(pairs.size() >= 3);

    auto size = pairs.size();
    auto width = T_SIZE * 2;
    auto height = (int) (2 * size);

    auto* A = gsl_matrix_alloc(height, width);
    auto* B = gsl_vector_alloc(height);

    double dataX[width];
    auto* residual = gsl_vector_alloc(height);
    auto X = gsl_vector_view_array(dataX, width);
    auto* tau = gsl_vector_alloc(width);

    for(auto i = 0; i < size; i++) {
        const auto &pair = pairs[i];
        const auto &fPoint = pair.first;
        const auto &sPoint = pair.second;

        float rowA1[] { fPoint.col, fPoint.row, 1, 0, 0, 0 };
        float rowA2[] { 0, 0, 0, fPoint.col, fPoint.row, 1 };

        gsl_vector_set(B, i * 2, sPoint.col);
        gsl_vector_set(B, i * 2 + 1, sPoint.row);

        for(auto j = 0; j < width; j++) {
            gsl_matrix_set(A, i * 2, j, rowA1[j]);
            gsl_matrix_set(A, i * 2 + 1, j, rowA2[j]);
        }
    }

    gsl_linalg_QR_decomp(A, tau);
    gsl_linalg_QR_lssolve(A, tau, B, &X.vector, residual);

    Transform2d h;
    for(auto i = 0; i < T_SIZE - 1; i++) {
        for(auto j = 0; j < T_SIZE; j++) {
            h[i][j] = gsl_vector_get(&X.vector, T_SIZE * i + j);
        }
    }
    h[T_SIZE - 1] = Transform1d{0, 0, 1};

    gsl_vector_free(tau);
    gsl_vector_free(residual);
    gsl_vector_free(B);
    gsl_matrix_free(A);

    return h;
}

template<typename T>
std::vector<pi::transforms::PPairs<T>> pi::transforms::inliers(const Transform2d& h,
                                                               const std::vector<PPairs<T>>& matches,
                                                               float threshold) {
    std::vector<PPairs<T>> inliers;

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

#endif //COMPUTER_VISION_TRANSFORMS_TPP
