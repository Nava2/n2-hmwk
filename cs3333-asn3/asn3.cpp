#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

#include "ImageMotion.h"


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

   // cv::waitKey(0);
}

int main(int argc, char** argv )
{
    if ( argc != 6 )
    {
        cout << "Usage: ./asn3 <Image0> ... <Image4>" << endl;
        return -1;
    }

     // Load images for processing
    cv::Mat midImg;
    std::vector<cv::Mat> imgs(5);
    for (int i = 0; i < 5; ++i) {
        imgs[i] = cv::imread(argv[i + 1], CV_LOAD_IMAGE_GRAYSCALE);

        if (i == 2) {
            imgs[i].copyTo(midImg);
        }
    }

    // confirm all sizes are the same
    const cv::Size fsize = imgs[0].size();
    for (int i = 1; i < 5; ++i) {
        assert(fsize == imgs[i].size());
    }

    // 5 point central difference
    const cv::Vec<float, 5> deriv = cv::Vec<float, 5>(1., -8., 0., 8., -1.) / 12.;

    // compute the image motion
    cv::Mat motionImg;
    imageMotion<5, 45>(imgs, deriv, motionImg);
    
    // the magnitudes are all in full value, normalize them: 
    cv::Mat normalizedMotion;
    normalizeChannels<3>(motionImg, {1, 2}, normalizedMotion);

    // display the image, treat theta as hue, rho as sat, and mag as variance:
    cv::Mat bgrImg;
    cv::cvtColor(normalizedMotion, bgrImg, CV_HSV2BGR);

    cv::Mat bgrMidImg;
    cv::cvtColor(midImg, bgrMidImg, CV_GRAY2BGR, 3);
    
    for (int i = 0; i < midImg.rows; ++i) {
        for (int j = 0; j < midImg.cols; ++j) {
            const cv::Vec3f &bgr = bgrImg.at<cv::Vec3f>(i, j);
            if (bgr != cv::Vec3f::zeros()) {
                bgrMidImg.at<cv::Vec3b>(i, j) += cv::Vec3b(std::round(255. * bgr[0]), 
                                                          std::round(255. * bgr[1]), 
                                                          std::round(255. * bgr[2]));
            }  
        }
    }
    
    showImage("Motion Output", bgrImg);
    showImage("Composite Image", bgrMidImg);
    cv::waitKey(0);

    return 0;
}
