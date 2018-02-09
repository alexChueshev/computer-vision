#ifdef COMPUTER_VISION_UTILS_H

template<typename T> utils::MatchResolvedType<pi::descriptors::BDescriptor, T>
utils::drawMatches(const cv::Mat& src1, const cv::Mat& src2, const std::vector<std::pair<T, T>>& matches) {
    assert(src1.type() == CV_32FC3);
    assert(src2.type() == CV_32FC3);

    cv::Mat dst(std::max(src1.rows, src2.rows), src1.cols + src2.cols, CV_32FC3);

    //concat images
    src1.copyTo(cv::Mat(dst, cv::Rect(0, 0, src1.cols, src1.rows)));
    src2.copyTo(cv::Mat(dst, cv::Rect(src1.cols, 0, src2.cols, src2.rows)));

    //draw lines
    for(const auto& match : matches) {
        cv::line(dst,
                 cv::Point(match.first.point.col, match.first.point.row),
                 cv::Point(match.second.point.col + src1.cols, match.second.point.row),
                 cv::Scalar(.0, 1., 1.)); //yellow color
    }

    return dst;
}

template<typename T> utils::MatchResolvedType<pi::detectors::Point, T>
utils::drawMatches(const cv::Mat& src1, const cv::Mat& src2, const std::vector<std::pair<T, T>>& matches) {
    assert(src1.type() == CV_32FC3);
    assert(src2.type() == CV_32FC3);

    cv::Mat dst(std::max(src1.rows, src2.rows), src1.cols + src2.cols, CV_32FC3);

    //concat images
    src1.copyTo(cv::Mat(dst, cv::Rect(0, 0, src1.cols, src1.rows)));
    src2.copyTo(cv::Mat(dst, cv::Rect(src1.cols, 0, src2.cols, src2.rows)));

    //draw lines
    for(const auto& match : matches) {
        cv::line(dst,
                 cv::Point(match.first.col, match.first.row),
                 cv::Point(match.second.col + src1.cols, match.second.row),
                 cv::Scalar(.0, 1., 1.)); //yellow color
    }

    return dst;
}

template<typename T>
cv::Mat utils::drawMatches(const pi::Img& src1, const pi::Img& src2, const std::vector<std::pair<T, T>>& matches) {
    assert(src1.channels() == 1);
    assert(src2.channels() == 1);

    return utils::drawMatches(utils::convertToMat(utils::convertTo3Ch(src1)),
                              utils::convertToMat(utils::convertTo3Ch(src2)), matches);
}

#endif
