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

template <int dWidth, int nWidth>
void houghTransform(const cv::Mat &I, cv::Mat &out) {
    CV_Assert(dWidth % 2 == 1 && nWidth % 2 == 1);

    const int D_WIDTHD2 = dWidth / 2;
    const int N_WIDTHD2 = nWidth / 2;

    cv::Mat uvbuff = cv::Mat::zeros(I.size(), CV_32FC2);
    for (int i = D_WIDTHD2; i < I.rows - D_WIDTHD2 - 1; ++i ) {
        for ( int j = D_WIDTHD2; j < I.cols - D_WIDTHD2 - 1; ++j ) {
            const cv::Vec3f vI = I.at<cv::Vec3f>(i, j);
            const float ix = vI[0],
                  iy = vI[1],
                  it = vI[2];

            const float u = -1. * ix / iy;
            const float v = -1. * it / iy;

            if (std::abs(iy) > 0.01) {
                uvbuff.at<cv::Vec2f>(i, j) = cv::Vec2f(u, v);
            }
        }
    }

    using std::vector;

    vector<vector<double>> minMax(2);
    vector<cv::Mat> uvbuffSpl(2);
    cv::split(uvbuff, uvbuffSpl);

    for (int i = 0; i < 2; ++i) {
        const cv::Mat m = uvbuffSpl[i];

        double min = std::numeric_limits<double>::max(),
                max = std::numeric_limits<double>::min();

        cv::minMaxIdx(m, &min, &max);
        minMax[i] = { min, max };
    }

    cout << "x = " << minMax[0][0] << ", " << minMax[0][1] << endl;
    cout << "y = " << minMax[1][0] << ", " << minMax[1][1]<< endl;

    const cv::Point2i xval(std::floor(minMax[0][0] - N_WIDTHD2), std::ceil(minMax[0][1] + N_WIDTHD2 + 1.)),
                  yval(std::floor(minMax[1][0] - N_WIDTHD2), std::ceil(minMax[1][1] + N_WIDTHD2 + 1.));

    cout << "xsize = " << xval << ", ysize = " << yval << endl;

    cv::Mat uvSpace = cv::Mat::zeros(cv::Size2i(xval.y - xval.x, yval.y - yval.x), CV_8UC1);

    for (int i = D_WIDTHD2; i < I.rows - D_WIDTHD2 - 1; ++i ) {
        for ( int j = D_WIDTHD2; j < I.cols - D_WIDTHD2 - 1; ++j ) {
            const cv::Point2f uv = uvbuff.at<cv::Point2f>(i, j);
            uvSpace.at<uchar>(int(uv.x - xval.x), int(uv.y - yval.x)) = 1;
        }
    }

    // uvSpace holds a binary image with a mapping of (x, y) -> (u, v)

    cv::Mat outMat(I.size(), CV_32FC2);
    for (int i = D_WIDTHD2; i < I.rows - D_WIDTHD2 - 1; ++i ) {
        for ( int j = D_WIDTHD2; j < I.cols - D_WIDTHD2 - 1; ++j ) {

            const cv::Point2f uv = uvbuff.at<cv::Point2f>(i, j);

            if (uv.x == 0 || uv.y == 0) {
                // ignore these pts
                continue ;
            }

            cv::Mat lines;
            cv::Rect roiRect(uv.x - N_WIDTHD2 - xval.x, uv.y - N_WIDTHD2 - yval.x, nWidth, nWidth);
            try {
                const cv::Mat roi = uvSpace(roiRect);

                cv::HoughLines(roi, lines, 1, CV_PI/180, nWidth);
            } catch (cv::Exception &e) {
                cout << "e = " << e.what() << endl;

                cout << "rect = " << roiRect << endl;
                cout << "uvSpace = " << uvSpace.size() << ": rows, cols = " << uvSpace.rows << ", " << uvSpace.cols << endl;

                throw e;
            }

            bool linesFound = lines.rows * lines.cols > 0;
            cv::Vec2f maxLine;
            // if there are lines
            if (linesFound) {
                float maxDot = std::numeric_limits<float>::min();

                for (int k = 0; k < lines.rows; ++k) {
                    const cv::Vec2f line = lines.at<cv::Vec2f>(k);
                    const float dot = line.dot(line);
                    if (maxDot < dot) {
                        maxDot = dot;
                        maxLine = line;
                    }
                }
            }

            if (linesFound) {
                // we found a value
                outMat.at<cv::Vec2f>(i, j) = maxLine;
            } else {
                outMat.at<cv::Vec2f>(i, j) = cv::Vec2f(0, 0);
            }
        }
    }

    outMat.copyTo(out);
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
    houghTransform<5, 45>(I, output);


    cv::Mat mImg = imgs[2];
    cv::Mat hsvImg = output;

    showImage("HSV Output", output);

    cv::waitKey(0);

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
