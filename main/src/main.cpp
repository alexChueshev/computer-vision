#include <imageprocessing.h>
#include <operations.h>

#include <utils.h>

using namespace pi;
using namespace std::placeholders;

int main() {
    ImageProcessing imageProcessing("/home/alexander/Lenna.png", utils::load);

    filters::Sobel sobel;
    auto sobelFunction = std::bind(&filters::Sobel::apply, &sobel, _1, _2);

    filters::Gaussian gaussian(2.5);
    auto gaussianFunction = std::bind(&filters::Gaussian::apply, &gaussian, _1, _2);

    imageProcessing.opts({opts::grayscale, opts::normalize})
                   .filters({std::make_pair(borders::BORDER_REPLICATE, gaussianFunction),
                             std::make_pair(borders::BORDER_REPLICATE, sobelFunction)})
                   .render("result", utils::render);

    return 0;
}
