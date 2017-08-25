#include <iostream>

#include <image.h>
#include <utils.h>
#include <opencv/cv.hpp>

int main() {

    std::cout << "Hello, World!" << std::endl;

    auto image = pi::utils::loadImage("/home/alexander/test.png");

    /*cv::Mat gray;
    cv::cvtColor( image.pureData(), gray, CV_BGR2GRAY );

    cv::namedWindow("", CV_WINDOW_AUTOSIZE);
    cv::imshow("", gray);
    cv::waitKey(0);*/

    auto img = image.pureData();

    cv::Mat m(img.rows, img.cols, CV_8UC1);
    auto it1 = m.begin<uchar>();
    int t = 0;
    double max = 0;
    double min = 999999;
    for (auto it = img.begin<cv::Vec3b>(),
                 end = img.end<cv::Vec3b>(); it != end; ++it) {
        auto val = (uchar)(.299 * (*it)[2] + .587 * (*it)[1] + .114 * (*it)[0]);
        *it1 = val;

        if(val > max) max = val;
        if(val < min) min = val;

        it1++;
        t++;
    }


    //auto vec = gray.at<uchar>(2,2);
    //vec = 255;
    //std::cout << m.channels() << std::endl;
    //std::cout << t << std::endl;
    //std::cout << image.height() * image.width() << std::endl;
    //std::cout << image.pureData().channels() << std::endl;

    std::cout << max << " " << min << std::endl;

    cv::namedWindow("", CV_WINDOW_AUTOSIZE);
    cv::imshow("", m);
    cv::waitKey(0);

    pi::utils::saveImage(image, "/home/alexander/test01.png");
    //pi::utils::renderImage(pi::Image(m), "original image");

    return 0;
}