#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>
#include <math.h>
#include <opencv2/opencv.hpp>

/*!
 * \file asn2.cpp
 *
 * Use optical flow to approximate the next image in a sequence, this
 * information can be used to check the validity of a image flow algorithm.
 *
 * \date 17 Oct 2014
 * \author Kevin Brightwell
 */

using std::cout;
using std::endl;

/*!
 * \brief showImage is a convenience function for displaying an image.
 * \param title Title for the window
 * \param src Image to display
 */
void showImage(const std::string &title, const cv::Mat &src) {
    cv::namedWindow(title, CV_WINDOW_AUTOSIZE );
    cv::imshow(title, src);

   // cv::waitKey(0);
}

template <int width>
void calculateI(const std::vector<cv::Mat> &images, const cv::Vec<float, width> &deriv,
                cv::Mat &outI) {
    CV_Assert(width == images.size());

    const int WIDTHD2 = width / 2;
    cv::Mat mergedImg;
    cv::merge(images, mergedImg);
    const cv::Mat mImg = images[WIDTHD2 + 1]; // middle image

    cv::Mat I = cv::Mat(mImg.size(), CV_32FC3);

    for (int j = WIDTHD2; j < mergedImg.cols - WIDTHD2; ++j) {
        for (int i = WIDTHD2; i < mergedImg.rows - WIDTHD2; ++i) {
            std::vector<uchar> x_data(width), y_data(width);
            for (int k = 0; k < width; ++k) {
                x_data[k] = mImg.at<uchar>(i - 2 + k, j);
                y_data[k] = mImg.at<uchar>(i, j - 2 + k);
            }

            const cv::Vec<uchar, width> x(x_data.data());
            const cv::Vec<uchar, width> y(y_data.data());
            const cv::Vec<uchar, width> t = mergedImg.at<cv::Vec<uchar, width>>(i, j);

            I.at<cv::Vec3f>(i, j) = cv::Vec3f(deriv.dot(x),
                                          deriv.dot(y),
                                          deriv.dot(t));
        }
    }

    I.copyTo(outI);
}

template <int width>
void houghTransform(const cv::Mat &I, cv::Mat &out) {
    CV_Assert(width % 2 == 1);

    const int WIDTHD2 = width / 2;

    cv::Mat outMat = cv::Mat::zeros(I.size(), CV_32FC3);

    // load an intermediate buffer with all of the points computing -I_x / I_y, -I_t / I_y
    cv::Mat uvbuff = cv::Mat::zeros(I.size(), CV_32FC3);

    cv::Mat binImg = cv::Mat::zeros(I.size(), CV_8U);

    const float threshold = 5.;

//    cout << "i = " << WIDTHD2 << " -> " << I.rows - WIDTHD2 - 1 << endl;
//    cout << "j = " << WIDTHD2 << " -> " << I.cols - WIDTHD2 - 1 << endl;

    for (int i = WIDTHD2; i < I.rows - WIDTHD2 - 1; ++i ) {
        for ( int j = WIDTHD2; j < I.cols - WIDTHD2 - 1; ++j ) {
            const cv::Vec3f vI = I.at<cv::Vec3f>(i, j);
            const float ix = vI[0],
                  iy = vI[1],
                  it = vI[2];

            const float u = -1. * ix / iy;
            const float v = -1. * it / iy;

            if (std::abs(iy) > 0.01) {
                uvbuff.at<cv::Vec3f>(i, j) = cv::Vec3f(u, v, std::sqrt(u*u + v*v));
            } else {
                uvbuff.at<cv::Vec3f>(i, j) = cv::Vec3f(0, 0, 0);
            }

            if (std::abs(iy) > 0.01 && std::abs(u) > threshold && std::abs(v) > threshold) {
                binImg.at<uchar>(i, j) = 255;
            } else {
                binImg.at<uchar>(i, j) = 0;
            }

//            cout << "uvbuff[" << i << ", " << j << "] = " << uvbuff.at<cv::Vec2f>(i, j) << std::endl;
        }
    }

    cv::Mat uvRGB(uvbuff.size(), CV_8UC3);
    cv::cvtColor(uvbuff / uchar(255), uvRGB, CV_HSV2RGB);

    cv::Mat uvGrey(uvRGB.size(), CV_8U);
    cv::cvtColor(uvRGB, uvGrey, CV_RGB2GRAY);

    cout << uvGrey.depth() << endl;

    cv::Mat uvGrey8U;
    uvGrey.convertTo(uvGrey8U, CV_8U);

    cv::Mat uvCanny;
    cv::Canny(uvGrey8U, uvCanny, 1, 1);

//    uvbuff.copyTo(out);

    cv::Mat flow(uvCanny.size(), CV_32FC3);
    for (int i = WIDTHD2; i < I.rows - WIDTHD2 - 1; ++i ) {
        for ( int j = WIDTHD2; j < I.cols - WIDTHD2 - 1; ++j ) {
            const cv::Rect roiRect(j - WIDTHD2, i - WIDTHD2, width, width);
            const cv::Mat roi = binImg(roiRect);

            cv::Mat lines;
            cv::HoughLines(roi, lines, 1, CV_PI/180, width);

            cv::Vec3f maxLine(0, 0, std::numeric_limits<float>::min());
            if (lines.rows * lines.cols > 0) {
                float maxDot = std::numeric_limits<float>::min();

                for (int k = 0; k < lines.rows; ++k) {
                    const cv::Vec2f line = lines.at<cv::Vec2f>(k);
                    const float dot = line.dot(line);
                    if (maxDot < dot) {
                        maxDot = dot;
                        maxLine = cv::Vec3f(line[1], line[0], dot);
                    }
                }
            }

            if (maxLine[2] > std::numeric_limits<float>::min()) {
                // we found a value
                flow.at<cv::Vec3f>(i, j) = maxLine;
            } else {
                flow.at<cv::Vec3f>(i, j) = cv::Vec3f(0, 0, 0);
            }
        }
    }

    // flow mat has the flow vectors
    cv::blur(flow, out, cv::Size(3, 3));

//    for ( int j = WIDTHD2; j < I.cols - WIDTHD2; ++j ) {
//        for (int i = WIDTHD2; i < I.rows - WIDTHD2; ++i ) {

//            // for a neighbourhood, collect the u,v points for fitting
//            std::vector<cv::Vec2f> pts(WIDTHD2 * WIDTHD2);
//            for (int y = -WIDTHD2; y < WIDTHD2; ++y) {
//                for (int x = -WIDTHD2; x < WIDTHD2; ++x) {
//                    pts.push_back(uvbuff.at<cv::Vec2f>(i + x, j + y));
//                }
//            }

////            std::cout << "pts(" << i << ", " << j << ") = [";

////            for (cv::Vec2f v : pts) {
////                std::cout << v;

////                if (v != *pts.end()) {
////                    std::cout << ", ";
////                }
////            }
////            std::cout << std::endl;

//            // fit the line, use least squares
//            cv::Vec4f line;
//            cv::fitLine(pts, line, CV_DIST_L2, 0, 0.01, 0.01);

//            const float x = line[0] * line[2];
//            const float y = line[1] * line[3];

//            // we now have the transformed line as optical flow
//            outMat.at<cv::Vec3f>(i, j) = cv::Vec3f(x, y, std::sqrt(x*x + y*y));
//        }
//    }

//    outMat.copyTo(out);
}

#define PYR_COUNT 0

int main(int argc, char** argv )
{
    if ( argc != 6 )
    {
        cout << "Usage: ./asn3 <Image0> ... <Image4>" << endl;
        return -1;
    }

     // Load images for processing
    std::vector<cv::Mat> imgs(5);
    for (int i = 0; i < 5; ++i) {
        cv::Mat m = cv::imread(argv[i + 1], CV_LOAD_IMAGE_GRAYSCALE);

        cv::blur(m, imgs[i], cv::Size(3,3));
//        imgs[i] = m;
    }

    // confirm all sizes are the same
    const cv::Size fsize = imgs[0].size();
    for (int i = 1; i < 5; ++i) {
        assert(fsize == imgs[i].size());
    }

    // 5 point central difference
    const cv::Vec<float, 5> deriv = cv::Vec<float, 5>(1., -8., 0., 8., -1.) / 12.;


    cv::Mat I;
    calculateI<5>(imgs, deriv, I);

    cv::Mat output;
    houghTransform<5>(I, output);


    cv::Mat mImg = imgs[2];
    cv::Mat hsvImg = output;

//    showImage("HSV Output", output);

//    std::cout << "output = " << cv::format(output, "python") << std::endl;

    // convert hsv
    cv::Mat rgbOutput, greyOutput;
    cv::cvtColor(hsvImg, rgbOutput, CV_HSV2BGR);
    rgbOutput.convertTo(rgbOutput, CV_8U);

    cv::cvtColor(rgbOutput, greyOutput, CV_BGR2GRAY, 1);
    greyOutput.convertTo(greyOutput, CV_8U);

    // create mask and inverted mask
    cv::Mat rgbMask, greyMask;
    cv::threshold(rgbOutput, rgbMask, 1, 255, cv::THRESH_BINARY);
    cv::threshold(greyOutput, greyMask, 1, 255, cv::THRESH_BINARY_INV);

    showImage("mImg", mImg);
    showImage("mask", rgbMask);

    cv::Mat mNoHSVPixels = greyMask & mImg; // all hsv pixels are now 0

    showImage("and", mNoHSVPixels);

    // convert the mNoHSVPixels to be in BGR and float like the
    cv::Mat mNoHSVPixelsRGB;
    cv::cvtColor(mNoHSVPixels, mNoHSVPixelsRGB, CV_GRAY2BGR, 3);
    showImage("maskedRGB", mNoHSVPixelsRGB);

    cv::Mat mOutImg = mNoHSVPixelsRGB + (rgbOutput & rgbMask) ;

    showImage("out?", mOutImg);

//    cv::Mat out;
//    cv::bitwise_or(rgbOutput, mImg, out, mask);
//    showImage("and", cv::bitwise_and(mask, mImg));
//    showImage("or", cv::bitwise_or(rgbOutput,  cv::bitwise_and(mask & mImg)));

//    showImage("and", ~(mImg & bwMask));
//    cv::Mat out = rgbOutputF | (mImg & bwMask);

//    showImage("Output", rgbOutputF);

//    showImage("Merged?", out);

    cv::waitKey(0);


    // I_y v = -I_t - I_x u
    // v = - I_t / I_y - I_x / I_y * u
    // watch for I_y being small, if it is too small (threshold it?) then set u, v to 0, 0
    //

    return 0;
}
