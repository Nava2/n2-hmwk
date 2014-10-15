#include <stdio.h>
#include <string>
#include <sstream>
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

    cv::waitKey(0);
}

/*!
 * \brief squareImage Squares out an image, padding the right or bottom with
 * black pixels
 * \param inputArr Input to read
 * \param outputArr Output image written to
 */
inline
void squareImage(const cv::Mat inputArr, cv::Mat &outputArr) {
    const cv::Size srcSize = inputArr.size();
    const int sqrSize = std::max(srcSize.height, srcSize.width);

    cv::copyMakeBorder(inputArr, outputArr, 0, sqrSize - srcSize.height,
                       0, sqrSize - srcSize.width, cv::BORDER_CONSTANT, 0);
}

struct MotionParams {

    //!< parameter, specifying the image scale (<1) to build pyramids for each
    //!< image; pyr_scale=0.5 means a classical pyramid, where each next layer
    //!< is twice smaller than the previous one.
    double pyr_scale;

    //!< number of pyramid layers including the initial image; levels=1 means
    //!< that no extra layers are created and only the original images are
    //!< used.
    int levels;

    //!< averaging window size; larger values increase the algorithm
    //!< robustness to image noise and give more chances for fast motion
    //!< detection, but yield more blurred motion field.
    int winsize;

    //!< number of iterations the algorithm does at each pyramid level.
    int iterations;

    //!< size of the pixel neighborhood used to find polynomial expansion in
    //!< each pixel; larger values mean that the image will be approximated
    //!< with smoother surfaces, yielding more robust algorithm and more
    //!< blurred motion field, typically poly_n =5 or 7.
    int poly_n;

    //!< standard deviation of the Gaussian that is used to smooth derivatives
    //!< used as a basis for the polynomial expansion; for poly_n=5, you can
    //!< set poly_sigma=1.1, for poly_n=7, a good value would be poly_sigma=1.5
    double poly_sigma;

    /*!
     * \brief run Run &cv::calcOpticalFlowFarneback()
     * \param first First frame, `n`
     * \param next Next frame, `n + 1`
     * \param flow Flow computed
     */
    void run(const cv::Mat &first, const cv::Mat &next, cv::Mat &flow) const {
        cv::calcOpticalFlowFarneback(first, next, flow, this->pyr_scale,
                                     this->levels, this->winsize,
                                     this->iterations, this->poly_n,
                                     this->poly_sigma,
                                     cv::OPTFLOW_FARNEBACK_GAUSSIAN);
    }


};

/// Write %MotionParams to an %std::ostream.
std::ostream& operator<<(std::ostream& os, const MotionParams& obj)
{
    os << "MotionParams {" <<
          "pyr_scale="  << obj.pyr_scale    << ", levels="      << obj.levels <<
          ", winsize="  << obj.winsize      << ", iterations="  << obj.iterations <<
          ", poly_n="   << obj.poly_n       << ", poly_sigma="  << obj.poly_sigma <<
          "}";
    return os;
}

/*!
 * \brief predictNextFrame Runs optical flow calculations, then creates a
 * predicted frame following \c first
 *
 * \param first first frame, `n`
 * \param next Next frame, `n + 1`
 * \param params Parameters for %cv::calcOpticalFlowFarneback
 * \return %cv::Mat with predicted frame
 */
const cv::Mat predictNextFrame(const cv::Mat &first, const cv::Mat &next, const MotionParams &params) {
    // square out input images to properly compute optical flow
    cv::Mat sqrFirst, sqrNext;
    squareImage(first, sqrFirst);
    squareImage(next, sqrNext);

    cv::Mat flowMat = cv::Mat::zeros(first.size(), CV_32FC2);

    std::stringstream ss;
    ss << params;
    cout << "Params:\t" << ss.str() << endl;

    params.run(sqrFirst, sqrNext, flowMat);

    // allocate a buffer to write flow contributions to
    cv::Mat buff = cv::Mat::zeros(flowMat.size(), CV_8U);

    // create a size to work with, this value is to ignore a 1-pixel border
    // due to applying a 3x3 operation
    const cv::Size adj(flowMat.size().width - 1, flowMat.size().height - 1);

    // manually apply a 3x3 kernel, creating a image with only valid pixels
    for (size_t j = 1; j < adj.height; ++j) {
        for (size_t i = 1; i < adj.width; ++i) {

            const cv::Rect cRect(i - 1, j - 1, 3, 3);

            uchar comb = 0;
            for (size_t x = i - 1; x < i + 2; ++x) {
                for (size_t y = j - 1; y < j + 2; ++y) {
                    const cv::Vec2f flow = flowMat.at<cv::Vec2f>(x, y);
                    const cv::Point2i ctrib(x + flow[0], y + flow[1]);

                    if (ctrib.inside(cRect)) {
                        comb += sqrFirst.at<uchar>(x, y);
                    }
                }
            }

            if (comb > 0) {
                buff.at<uchar>(i, j) = comb / 9;
            }
        }
    }

    // copy the buffer into an output Mat
    cv::Mat out;
    buff(cv::Rect(0, 0, first.size().width, first.size().height)).copyTo(out);

    return out;
}

int main(int argc, char** argv )
{
    if ( argc != 3 )
    {
        cout << "Usage: ./asn1 <First Image> <Next Image>" << endl;
        return -1;
    }

    // Load images for processing
    const cv::Mat firstImg = cv::imread( argv[1], CV_LOAD_IMAGE_GRAYSCALE );
    const cv::Mat nextImg = cv::imread( argv[2], CV_LOAD_IMAGE_GRAYSCALE );

    // verify the files were valid
    if ( firstImg.empty() || nextImg.empty() )
    {
        printf("No image data \n");
        return -1;
    }

    assert(firstImg.size() == nextImg.size());

    MotionParams params = { 0.5,
            3,      // levels
            5,      // win size
            10,     // iterations
            5,      // poly_n
            1.2 };  // poly_sigma

    const cv::Mat out = predictNextFrame(firstImg, nextImg, params);

    // compute the std deviation and average between the actual next frame and
    // the predicted
    cv::Mat diffMat;
    cv::absdiff(nextImg, out, diffMat);

    cv::Scalar mean, dev;
    cv::meanStdDev(diffMat, mean, dev, out);

    cout << "Mean: " << mean[0] << endl;
    cout << "Dev:  " << dev[0] << endl;

    return 0;
}
