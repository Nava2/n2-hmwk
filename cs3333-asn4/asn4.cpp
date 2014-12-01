#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <iostream>

#include <opencv2/opencv.hpp>

#include <opencv2/nonfree/features2d.hpp>


/*!
 * \file asn3.cpp
 *
 * Use spatial temporal derivatives to compute the optical flow through the Hough Transform. 
 *
 * \date 17 Nov 2014
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
}

// Width of the neighbourhood
static const int NEIGHBOURHOOD = 45;

int main(int argc, char** argv )
{
    if ( argc < 2)
    {
        cout << "Usage: ./asn4 Image[0] [ Image[1] ... Image[N] ]" << endl;
        return -1;
    }

     // Load images for processing
    cv::Mat midImg;
    std::vector<cv::Mat> imgs(argc - 1);
    for (int i = 0; i < imgs.size(); ++i) {
        imgs[i] = cv::imread(argv[i + 1], CV_LOAD_IMAGE_COLOR);
    }

    // confirm all sizes are the same
    const cv::Size fsize = imgs[0].size();
    for (const cv::Mat img : imgs) {
        assert(fsize == img.size());
    }

    cv::Mat img1 = imgs[0], img2 = imgs[1];

    cv::Ptr<cv::Feature2D> sift = cv::Feature2D::create("SIFT");
    std::vector<std::string> names;
    sift->getParams(names);

    for (std::string name : names) {
        cout << name <<  ", ";
    }
    cout << endl;
    sift->setInt("nFeatures", 100);

    std::vector<cv::KeyPoint> pts1, pts2;
    cv::Mat desc1, desc2;
    sift->detect(img1, pts1);
    sift->compute(img1, pts1, desc1);

    sift->detect(img2, pts2);
    sift->compute(img2, pts2, desc2);

    cv::FlannBasedMatcher matcher;
    std::vector< cv::DMatch > matches;
    matcher.match( desc1, desc2, matches );

    cv::Mat display;
    cv::drawMatches(img1, pts1, img2, pts2, matches, display);

//    cv::Mat descriptors;

//    siftOp(img, cv::Mat::ones(img.size(), CV_8U), pts, descriptors);

//    cv::Mat display;
//    cv::drawKeypoints(img, pts, display, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    cv::Mat out;
    cv::resize(display, out, cv::Size(), 0.25, 0.25);

    showImage("Points", out);


//    // declare 5 point central difference
//    const cv::Vec<float, 5> deriv = cv::Vec<float, 5>(1., -8., 0., 8., -1.) / 12.;

//    // compute the image motion
//    cv::Mat motionImg;
//    imageMotion<NEIGHBOURHOOD>(imgs, deriv, motionImg);
    
//    // the magnitudes are all in full value, normalize them:
//    cv::Mat normalizedMotion;
//    normalizeChannels<3>(motionImg, {1, 2}, normalizedMotion);

//    // display the image, treat theta as hue, rho as sat, and mag as variance:
//    cv::Mat bgrImg;
//    cv::cvtColor(normalizedMotion, bgrImg, CV_HSV2BGR);

//    cv::Mat bgrMidImg;
//    cv::cvtColor(midImg, bgrMidImg, CV_GRAY2BGR, 3);
    
//    // the bgrImg is in 0.0..1.0 RGB, convert it to 0..255 and add it to bgrMidImg
//    cv::add(bgrMidImg, 255. * bgrImg, bgrMidImg, cv::noArray(), CV_8UC3);
    
//    showImage("Motion Output", bgrImg);
//    showImage("Composite Image", bgrMidImg);
    cv::waitKey(0);

    return 0;
}
