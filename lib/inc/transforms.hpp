#ifdef COMPUTER_VISION_TRANSFORMS_H

using namespace pi;

template<typename T>
transforms::Transform2d transforms::dltHomography(const std::vector<PPairs<T>>& pairs) {
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
transforms::Transform2d transforms::dltAffine(const std::vector<PPairs<T>>& pairs) {
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
std::vector<transforms::PPairs<T>> transforms::inliers(const Transform2d& h,
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

template<typename T>
std::pair<transforms::Hypothesis<T>, float> transforms::verify(const transforms::Hypotheses<T>& hypotheses,
                                                               int matches, float threshold) {
    auto max = .0f;
    auto best = -1;

    for(auto it = std::begin(hypotheses), end = std::end(hypotheses); it != end; it++) {
        auto probability = ((float) it->second.size()) / matches;
        if(probability > max) {
            max = probability;
            best = std::distance(std::begin(hypotheses), it);
        }
    }

    if(best != -1 && max >= threshold) {
        return { hypotheses[best], max};
    }

    return {{}, 0};
}

template<typename T>
transforms::Transform2d transforms::homography(const std::vector<PPairs<T>>& matches,
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

#endif //COMPUTER_VISION_TRANSFORMS_H
