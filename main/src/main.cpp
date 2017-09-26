#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <imageprocessing.h>
#include <operations.h>

using namespace pi;
using namespace std::placeholders;

int main() {
    auto loadFunction = [](const std::string& path) {
        cv::Mat src = cv::imread(path, cv::IMREAD_COLOR);
        assert(src.type() == CV_8UC3);

        Img img(src.rows, src.cols, src.channels());

        auto* data = img.data();
        auto step = img.step();

        for(auto i = 0, rows = src.rows; i < rows; i++) {
            auto* ptr = src.ptr<uchar>(i);
            auto row = i * step;

            for(auto j = 0; j < step; j++) {
                data[row + j] = ptr[j];
            }
        }

        return img;
    };

    auto renderFunction = [](const std::string& window, const Img& img) {
        cv::namedWindow(window, CV_WINDOW_AUTOSIZE);
        cv::imshow(window, cv::Mat(img.height(), img.width(),
                                   CV_32FC1, const_cast<float*>(img.data())));
        cv::waitKey(0);
    };

    ImageProcessing imageProcessing("/home/alexander/Lenna.png", loadFunction);

    filters::Sobel sobel;
    auto sobelFunction = std::bind(&filters::Sobel::apply, &sobel, _1, _2);

    filters::Gaussian gaussian(1.5);
    auto gaussianFunction = std::bind(&filters::Gaussian::apply, &gaussian, _1, _2);

    imageProcessing.opts({opts::grayscale, opts::normalize})
                   .filters({std::make_pair(borders::BORDER_REPLICATE, gaussianFunction),
                             std::make_pair(borders::BORDER_REPLICATE, sobelFunction)})
                   .render("result", renderFunction);

    return 0;
}
