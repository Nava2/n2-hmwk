#include <stdio.h>
#include <string>
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

using std::string;
using std::cout;
using std::endl;

/*!
 * \brief DEFAULT_OUTPUT Default path for output image
 */
const static string DEFAULT_OUTPUT = "output.png";

/*!
 * \brief showImage is a convenience function for displaying an image.
 * \param title Title for the window
 * \param src Image to display
 */
void showImage(const string &title, const cv::Mat &src) {
    cv::namedWindow(title, CV_WINDOW_AUTOSIZE );
    cv::imshow(title, src);

    cv::waitKey(0);
}

inline
void squareImage(const cv::Mat inputArr, cv::Mat outputArr) {
    const cv::Size srcSize = inputArr.size();
    const int sqrSize = std::max(srcSize.height, srcSize.width);

    cv::copyMakeBorder(inputArr, outputArr, 0, sqrSize - srcSize.height,
                       0, sqrSize - srcSize.width, cv::BORDER_CONSTANT, 0);
}


using std::cout;
using std::endl;

int main(int argc, char** argv )
{
    if ( argc != 3 && argc != 4 )
    {
        cout << "Usage: ./asn1 <Image_Path1> <Image_Path2> [<Output_Path>]" << endl;
        return -1;
    }

    // check if optional param was specified, take if it's available.
    const string OUTPUT_PATH = (argc == 4 ? argv[3] : DEFAULT_OUTPUT);

    // Load images for processing
    const cv::Mat srcImage1 = cv::imread( argv[1], CV_LOAD_IMAGE_GRAYSCALE );
    const cv::Mat srcImage2 = cv::imread( argv[2], CV_LOAD_IMAGE_GRAYSCALE );

    // verify the files were valid
    if ( srcImage1.empty() || srcImage2.empty() )
    {
        printf("No image data \n");
        return -1;
    }

    assert(srcImage1.size() == srcImage2.size());

    // square out input images to properly compute optical flow
    cv::Mat sqrImg1, sqrImg2;
    squareImage(srcImage1, sqrImg1);
    squareImage(srcImage2, sqrImg2);

    cv::Mat flowMat = cv::Mat::zeros(sqrImg1.size(), CV_32FC2);
    cv::calcOpticalFlowFarneback(sqrImg1, sqrImg2, flowMat,
                                 0.5, 3, 3, 3, 5, 1.2, cv::OPTFLOW_FARNEBACK_GAUSSIAN);

    // allocate a buffer to write flow contributions to
    cv::Mat buff = cv::Mat::zeros(flowMat.size(), CV_8U);
    cv::Size adj(flowMat.size().width - 1, flowMat.size().height - 1);


    for (size_t j = 1; j < adj.height; ++j) {
        for (size_t i = 1; i < adj.width; ++i) {

            const cv::Rect cRect(i - 1, j - 1, 3, 3);

            uchar comb = 0;
            for (size_t x = i - 1; x < i + 2; ++x) {
                for (size_t y = j - 1; y < j + 2; ++y) {
                    const cv::Vec2f flow = flowMat.at<cv::Vec2f>(x, y);
                    const cv::Point2i ctrib(x + flow[0], y + flow[1]);

                    if (ctrib.inside(cRect)) {
                        comb += sqrImg1.at<uchar>(x, y);
                    }
                }
            }

            if (comb > 0) {
                buff.at<uchar>(i, j) = comb / 9;
            }
        }
    }

    cv::Mat out;
    buff(cv::Rect(0, 0, srcImage1.size().width, srcImage1.size().height)).copyTo(out);

    cout << "Output Size: " << out.size() << endl;
    showImage("out", out);

    cv::Mat diffMat;
    cv::absdiff(srcImage2, out, diffMat);

    cv::Scalar mean, dev;
    cv::meanStdDev(diffMat, mean, dev, out);

    cout << "Mean: " << mean[0] << endl;
    cout << "Dev:  " << dev[0] << endl;

    cout << "Output written to: " << OUTPUT_PATH << endl;

    return 0;
}
