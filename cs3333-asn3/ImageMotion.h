
#ifndef IMAGE_MOTION_H
#define IMAGE_MOTION_H

#include <opencv2/opencv.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <string>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <unordered_set>

/*!
 * \file ImageMotion.h
 *
 * Declarations for functions used in Assignment 3. 
 *
 * \date 17 Nov 2014
 * \author Kevin Brightwell
 */

// Forwards: 

namespace _kbright2 {

/*!
 * Convert a line to Polar coordinates (including magnitude of line).
 * \param line Line with two points P1(line[0], line[1]), P2(line[2], line[3]).
 * \return cv::Vec3f made up of (theta, rho, ||line||).
 */
inline
cv::Vec3f toPolar(const cv::Vec4i line) {
    const int dX = std::abs(line[0] - line[2]);
    const int dY = std::abs(line[1] - line[3]);
    const int mX = (line[0] + line[2]) / 2;
    const int mY = (line[1] + line[3]) / 2;
    
    const float theta = std::fmod((180 / CV_PI) * std::atan2(mY, mX), 360.); // degrees
    const float rho = std::sqrt(mX * mX + mY * mY);
    const float mag = std::sqrt(dX * dX + dY * dY);
    return cv::Vec3f(theta, rho, mag); 
}

/*!
 * \brief Calculate the spatial temporal derivaitve from a set of `width` images. 
 * \param images Images to compute with. 
 * \param deriv Derivative kernal to use. 
 * \param width Width of the Kernel.
 */
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

/*!
 * \brief Computes the Image Motion using the motion constraint equations to create u,v lines based on the 
 * spatial-temporal derivative, I. 
 * \param dWidth Derivative width
 * \param nWidth Neighbourhood width for use with HoughTransform
 * \param I Image motion values
 * \param out Output matrix, will be allocated as necessary, size(I) == size(out) but with depth CV_32FC3. 
 */
template <int dWidth, int nWidth>
void houghTransform(const cv::Mat &I, cv::Mat &out) {
    CV_Assert(dWidth % 2 == 1 && nWidth % 2 == 1);

    const int D_WIDTHD2 = dWidth / 2;
    const int N_WIDTHD2 = nWidth / 2;

    // Compute the u, v values per x, y pixel and store them at a buffer in x, y
    // this acts as a "map" from (x, y) -> (u, v). 
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

    // allocate a matrix that can hold all values of u, v computed s.t. we can create a binary mapping of what points 
    // are neighbours for the HoughTransform
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

    const cv::Point2i xval(std::floor(minMax[0][0] - N_WIDTHD2), std::ceil(minMax[0][1] + N_WIDTHD2 + 1.)),
                  yval(std::floor(minMax[1][0] - N_WIDTHD2), std::ceil(minMax[1][1] + N_WIDTHD2 + 1.));
                  
    auto uvConv = [xval, yval](const float x, const float y) -> cv::Point {
        return cv::Point2i(std::floor(x - xval.x), std::floor(y - yval.x));      
    };

    // allocate the matrix itself
    cv::Mat uvSpace = cv::Mat::zeros(cv::Size(uvConv(xval.y, yval.y)), CV_8UC1);

    // fill the uvSpace with the u, v mappings as a binary image w/ 255 instead of 1
    for (int i = D_WIDTHD2; i < I.rows - D_WIDTHD2 - 1; ++i ) {
        for ( int j = D_WIDTHD2; j < I.cols - D_WIDTHD2 - 1; ++j ) {
            const cv::Point2f uv = uvbuff.at<cv::Point2f>(i, j);
            
            if (uv.x != 0. && uv.y != 0.) {
                uvSpace.at<uchar>(uvConv(uv.x, uv.y)) = 255;
            }
        }
    }
    
    // uvSpace holds a binary image with a mapping of (x, y) -> (u, v)

    // for each (x, y) value (inside the derivative boundary) compute the HoughTransform of the 
    // points that are in u, v and then take the largest line (magnititude) and write it to the 
    // output matrix with it's values as: (theta, rho, mag). 
    cv::Mat outMat = cv::Mat::zeros(I.size(), CV_32FC3);
    unsigned long linesFound = 0;
    for (int i = D_WIDTHD2; i < I.rows - D_WIDTHD2 - 1; ++i ) {
        for ( int j = D_WIDTHD2; j < I.cols - D_WIDTHD2 - 1; ++j ) {

            const cv::Point2f uv = uvbuff.at<cv::Point2f>(i, j);

            if (uv.x == 0 && uv.y == 0) {
                // ignore these pts
                continue ;
            }
  
            const cv::Rect roiRect(uvConv(uv.x - N_WIDTHD2, uv.y - N_WIDTHD2), cv::Point(nWidth, nWidth));      
            const cv::Mat roi = uvSpace(roiRect);
             
            vector<cv::Vec4i> lines;
            cv::HoughLinesP(roi, lines, 1, CV_PI/30, 5, 5);

            // if there are lines
            if (lines.size() > 0) {                
                // we found a value
                // get the largest magnitude line, 
                cv::Vec3f maxOut = toPolar(lines[0]);
                
                for (int i = 1; i < lines.size(); ++i) {
                    const cv::Vec4i line = lines[i];
                    const cv::Vec3f polar = toPolar(line);
                    
                    if (polar[2] > maxOut[2]) {
                        maxOut = polar;
                    }
                }
                
                outMat.at<cv::Vec3f>(i, j) = maxOut;
                linesFound++;
            }
        }
    }  

    outMat.copyTo(out);
}

};


/*!
 * \brief Normalize the magnitude of the output from the houghTransform function. 
 * \param input Input matrix of type CV_32FC3.
 * \param channels Channels to normalize
 * \param output Matrix with values of Input but with magnitiudes normalized, i.e. 0 <= mag <= 1.0.
 */
template <int cn>
void normalizeChannels(const cv::Mat &input, const std::unordered_set<int> channels, cv::Mat &output) {
    CV_Assert(cn >= channels.size());
    
    using std::vector;
    
    std::unordered_map<int, float> maxs;
    for (int k : channels) {
        CV_Assert(k < cn);
        maxs[k] = std::numeric_limits< float >::min();
    }

    for (int i = 0; i < input.rows; ++i ) {
        for ( int j = 0; j < input.cols; ++j ) {
            const cv::Vec<float, cn> &vals = input.at<cv::Vec<float, cn> >(i, j);
            for (int chan : channels) {
                maxs[chan] = std::max(vals[chan], maxs[chan]);
            }
        }
    }
    
    input.copyTo(output);
    
    // normalize all of the lengths
    for (int i = 0; i < output.rows; ++i ) {
        for ( int j = 0; j < output.cols; ++j ) {
            cv::Vec<float, cn> &vals = output.at<cv::Vec<float, cn> >(i, j);
           
            for (int chan : channels) {
                float &v = vals[chan];
                v = v / maxs[chan];
            }
        }
    }
}

/*!
 * \brief Computes the image motion of an image by computing the spatial temporal derivative followed by computing the 
 * Hough Transform on the derivative space and returning lines of motion. 
 * \param images Images used as input for spatial-temporal derivative calculation
 * \param deriv A linear kernel for computing the derivative
 * \param outut Output matrix
 * \param derivWidth Size of the derivative vector and the images vector. 
 * \param neighbourhoodWidth Width of the neighbourhood to consider for the HoughTransform. 
 */
template <int derivWidth, int neighbourhoodWidth>
void imageMotion(const std::vector<cv::Mat> &images, const cv::Vec<float, derivWidth> &deriv, cv::Mat &output) {
    CV_Assert(images.size() == derivWidth);
    
    // lower the resolution of the images, scale down the time
    std::vector<cv::Mat> pyrImgs;
    for (cv::Mat m : images) {
        cv::Mat pm;
        cv::pyrDown(m, pm);
        
        pyrImgs.push_back(pm);
    }
    
    cv::Mat I;
    _kbright2::calculateI<derivWidth>(pyrImgs, deriv, I);

    cv::Mat pyrOut;
    _kbright2::houghTransform<derivWidth, neighbourhoodWidth>(I, pyrOut);

    cv::pyrUp(pyrOut, output);
}

#endif