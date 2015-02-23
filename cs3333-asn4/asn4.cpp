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

/*!
 * \brief showImage is a convenience function for displaying an image.
 * \param title Title for the window
 * \param src Image to display
 */
void showImage(const std::string &title, const cv::Mat &src, float sx, float sy = 0) {
    if (sy <= 0) {
        sy = sx;
    }

    cv::Mat out;
    cv::resize(src, out, cv::Size(), sx, sy);

    cv::namedWindow(title, CV_WINDOW_AUTOSIZE );
    cv::imshow(title, out);
}

/**
 * @brief cropImage Crops an image to remove the outside black borders. This completes this by finding the external
 * contour and cropping along the bounding rectangle.
 * @param in Input image
 * @param out ROI of the input image, cropped.
 */
void cropImage(const cv::Mat &in, cv::Mat &out) {

    cv::Mat in_grey, threshold;
    cv::cvtColor(in, in_grey, CV_RGB2GRAY, 1);
    cv::threshold(in_grey, threshold, 5, 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point2i> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(threshold, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );

    cv::Rect border;
    for(const std::vector<cv::Point2i> contour : contours) {
        std::vector<cv::Point2i> poly;
        approxPolyDP( cv::Mat(contour), poly, 3, true );
        cv::Rect cborder = cv::boundingRect(cv::Mat(poly));
        if (cborder.area() > border.area()) {
            border = cborder;
        }
    }

    if (border.area() > 0) {
        out = in(border);
    } else {
        out = in;
    }
}


void stitchImages(const cv::Ptr<cv::Feature2D> &feature2D, const cv::DescriptorMatcher &matcher,
                  const cv::Mat &imgA, const cv::Mat &imgB,
                  cv::Mat &out, std::vector<cv::KeyPoint> &outPts) {

    // use the feature detectors passed:
    std::vector<cv::KeyPoint> keyPtsA, keyPtsB;
    cv::Mat descA, descB;

    // per img
//    if (outPts.size() > 0) {
//        keyPtsA = outPts;
//    } else {
        feature2D->detect(imgA, keyPtsA);
//    }

    feature2D->compute(imgA, keyPtsA, descA);

    feature2D->detect(imgB, keyPtsB);
    feature2D->compute(imgB, keyPtsB, descB);

    // matches betwen the two images
    std::vector< cv::DMatch > matches;
    matcher.match( descA, descB, matches);

    // filter the matches base don finding the minimum of 3
    double min = std::numeric_limits<double>::max();
    for (cv::DMatch match : matches) {
        if (match.distance < min) {
            min = match.distance;
        }
    }

    const double threshold = 3. * min;
    std::vector<cv::DMatch> goodMatches;
    for (cv::DMatch match : matches) {
        // TODO 3?
        if (match.distance < threshold) {
            goodMatches.push_back(match);
        }
    }

    std::vector<cv::KeyPoint> kpBuff;
    std::vector<cv::Point2f> pts4HomoA, pts4HomoB;
    for (cv::DMatch match : goodMatches) {
        // Get the keypoints from the good matches
        cv::KeyPoint aPt = keyPtsA[match.queryIdx];
        kpBuff.push_back(aPt);

        pts4HomoA.push_back(aPt.pt);
        pts4HomoB.push_back(keyPtsB[match.trainIdx].pt);
    }

    // compute the homography:
    const cv::Mat H = cv::findHomography(pts4HomoA, pts4HomoB, CV_RANSAC);

    // Now that we have the homography find the boundaries of the (eventually) warped image
    std::vector<cv::Point2f> cornersInA(4);
    cornersInA[0] = cv::Point2f(0,0); cornersInA[1] = cv::Point2f( imgA.cols, 0 );
    cornersInA[2] = cv::Point2f( imgA.cols, imgA.rows ); cornersInA[3] = cv::Point2f( 0, imgA.rows );

    std::vector<cv::Point2f> cornersInB(4);
    cv::perspectiveTransform(cornersInA, cornersInB, H);

    // since the transform will translate values to negative co-ordinates, we need to find the origin and use this to
    // shift the transform to only live in positive space.
    cv::Point2f origin(0, 0);
    cv::Point2f maxXY(imgB.cols, imgB.rows);
    for (cv::Point2f pt : cornersInB) {
        origin.x = std::min(pt.x, origin.x);
        origin.y = std::min(pt.y, origin.y);

        maxXY.x = std::max(pt.x, maxXY.x);
        maxXY.y = std::max(pt.y, maxXY.y);
    }
    origin = cv::Point2f(std::abs(origin.x), std::abs(origin.y));

    // now have 'origin' which is the x,y shift required, use matrix multiplication to make it work
    const cv::Mat pTrans = (cv::Mat_<double>(3,3) << 1, 0, origin.x,
                                                     0, 1, origin.y,
                                                     0, 0, 1) * H;

    // now pTrans will not translate values into the negative space when used with warpPerspective
    cv::Mat warped;
    cv::warpPerspective(imgA, warped, pTrans, cv::Size(origin.x + maxXY.x, origin.y + maxXY.y));

    // `warped` now has the warped image drawn, there are massive borders, but they are cleaned later

//    showImage("warped..", warped, 0.5);
//    cout << "origin = " << origin << ", maxXY = " << maxXY << endl;

    // now we stitch the two images together
    cv::Size totalSize(std::round(maxXY.x + origin.x), std::round(maxXY.y + origin.y));

    cv::Mat stitched = cv::Mat::zeros(totalSize, CV_8UC3);

    // for the first image, pull the ROI from the output image, and copy it
    cv::Rect rROI = cv::Rect(origin.x, origin.y, imgB.cols, imgB.rows);
    cout << "roiRect = " << rROI << endl;
    cv::Mat roi = stitched(rROI);
    imgB.copyTo(roi);

    // now put the warped image onto the output plane
    cv::Mat roi2 = stitched(cv::Rect(cv::Point2i(0, 0), warped.size()));
    cv::Mat warpedMask; // create a mask for copying
    cv::cvtColor(warped, warpedMask, CV_RGB2GRAY, 1);
    warped.copyTo(roi2, warpedMask);

    cropImage(stitched.clone(), out);
    outPts = kpBuff;
}

int main(int argc, char** argv )
{
    if ( argc < 2)
    {
        cout << "Usage: ./asn4 Image[0] [ Image[1] ... Image[N] ]" << endl;
        return -1;
    }

     // Load images for processing

    std::vector<cv::Mat> fullImgs(argc - 1);
    for (int i = 0; i < fullImgs.size(); ++i) {
        fullImgs[i] = cv::imread(argv[i + 1], CV_LOAD_IMAGE_COLOR);
    }

    std::vector<const cv::Mat> imgs;
    for (int i = 0; i < fullImgs.size(); ++i) {
        cv::Mat m;
        cv::pyrDown(fullImgs[i], m);
        imgs.push_back(m);
    }

    // confirm all sizes are the same
    const cv::Size fsize = imgs[0].size();
    for (const cv::Mat img : imgs) {
        assert(fsize == img.size());
    }

    cv::Ptr<cv::Feature2D> feature2D = cv::Feature2D::create("SIFT");
    feature2D->setInt("nFeatures", 200);

    const cv::FlannBasedMatcher matcher;

    cv::Mat buff;
    std::vector<cv::KeyPoint> keyPts;
    stitchImages(feature2D, matcher, imgs[0], imgs[1], buff, keyPts);
    for (int i = 2; i < imgs.size(); ++i) {
        stitchImages(feature2D, matcher, buff, imgs[i], buff, keyPts);
    }

    showImage("stitched (12.5% scale)", buff, 0.25);

    cv::Mat output;
    cv::pyrUp(buff, output);
    cv::imwrite("stitched.png", output);

//    std::vector<cv::Point2f> coords(imgA.rows * imgA.cols);
//    for (int i = 0; i < imgA.rows; ++i) {
//        for (int j = 0; j < imgA.cols; ++j) {
//            coords.push_back(cv::Point2f(i, j));
//        }
//    }

    double min = std::numeric_limits< double >::max();
    for (cv::DMatch match : matches) {
        if (match.distance < min) {
            min = match.distance;
        }
    }
    
    const double thresh = min * 3.;
    
    std::vector< cv::DMatch > goodMatches;
    for (cv::DMatch match: matches) {
        if (match.distance < thresh) {
            goodMatches.push_back(match);
        }
    }
    
    // we have good matches now based on a thresholding
    cv::Mat display;
    cv::drawMatches(img1, pts1, img2, pts2, goodMatches, display);
    
    std::vector<cv::Point2f> obj;
    std::vector<cv::Point2f> scene;

    for( cv::DMatch match : goodMatches )
    {
        //-- Get the keypoints from the good matches
        obj.push_back( pts2[ match.queryIdx ].pt );
        scene.push_back( pts2[ match.trainIdx ].pt );
    }
    
    cv::Mat H = findHomography( obj, scene, CV_RANSAC );

//    cv::Mat display;
//    cv::drawMatches(imgA, ptsA, imgB, ptsB, goodMatches, display);

//    cv::line( display, scene_corners[0] + cv::Point2f( imgA.cols, 0), scene_corners[1] + cv::Point2f( imgA.cols, 0), cv::Scalar(0, 255, 0), 4 );
//    cv::line( display, scene_corners[1] + cv::Point2f( imgA.cols, 0), scene_corners[2] + cv::Point2f( imgA.cols, 0), cv::Scalar( 0, 255, 0), 4 );
//    cv::line( display, scene_corners[2] + cv::Point2f( imgA.cols, 0), scene_corners[3] + cv::Point2f( imgA.cols, 0), cv::Scalar( 0, 255, 0), 4 );
//    cv::line( display, scene_corners[3] + cv::Point2f( imgA.cols, 0), scene_corners[0] + cv::Point2f( imgA.cols, 0), cv::Scalar( 0, 255, 0), 4 );

//    showImage("Points", display, 0.5);


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
