/* **************************************************************
 * This class extracts the boundary of the lips in a video frame
 *
 * Author: Nordine Sebkhi
 * **************************************************************/

#include "processframe.h"
#include <QVector>
#include <QPoint>
#include <qmath.h>


/**
 * @brief Constructor that sets the frame to be processed
 * @param frame a video frame (cv::Mat object)
 * @param parent
 */
ProcessFrame::ProcessFrame(Mat frame,  QObject *parent) : QThread(parent)
{
    this->frame = frame;
}


/**
 * @brief Process a video frame
 * Lower resolution to reduce processing time
 * Convert the color scheme to standard RGB (openCV saves frames in another color order)
 * Produce a black & white image with lips in white
 * Locate lips boundary
 */
void ProcessFrame::run()
{
    // Lower frame resolution to reduce execution time
    cv::resize(frame, frame, Size(320, 240), 0, 0, INTER_AREA);

    // OpenCV frame color format is by default BGR. Invert color to RGB for display
    cv::cvtColor(frame, frame, CV_BGR2RGB);

    // Process frame to extract a lips into a binary image
    Mat bwFrame         = extractLipsAsBWImg(frame);
    emit binaryImg(bwFrame);

    // Process binary image to localize points on the lip boundaries
    QVector<QPoint> lipsPoints  = extractPointsOnLipsEdge(bwFrame);
    emit lipsPos(frame, lipsPoints);
}


/**
 * @brief Construct a binary image with lips pixels as white (255) and others as black (0)
 * @param frame RGB image
 * @return binary image
 */
Mat ProcessFrame::extractLipsAsBWImg(Mat &frame)
{
    // Create a copy of frame with float as data type
    // Needed for Red color extraction algorithm
    Mat formattedFrame(frame.rows, frame.cols, CV_32FC3);
    frame.convertTo(formattedFrame, CV_32FC3, 1.0/255.0);

    // Split the image into different color channels
    std::vector<Mat> rgbChannels;
    cv::split(formattedFrame, rgbChannels);

    Mat redChannel = rgbChannels[0];
    Mat greenChannel = rgbChannels[1];

    // Apply the lips extraction filter based on Red pixels differentiation
    Mat bwFrame(frame.rows, frame.cols, CV_32FC1);
    cv::add(greenChannel, 0.000001, bwFrame);
    cv::divide(redChannel, bwFrame, bwFrame);
    cv::log(bwFrame, bwFrame);

    // Compute the threshold to render lips-like area to white and other areas to black
    Mat frameVect;
    bwFrame.reshape(0, 1).copyTo(frameVect);            // Flatten out the frame into a row vector
    cv::sort(frameVect, frameVect, CV_SORT_ASCENDING);
    double thres_coeff = 0.18;                          // Variable that sets strength of discrimination (lower = more discrimination)
    int threshIdx = (frameVect.cols - 1) - qFloor(frameVect.cols * thres_coeff);
    float thresVal = frameVect.at<float>(0, threshIdx);

    // Create the binary image
    Mat bwFrameProc = bwFrame > thresVal;

    // Keep only the biggest agglomerate of white pixels as more likely related to lips
    Mat connCompLabels, connCompStats, connCompCentroids;
    cv::connectedComponentsWithStats(bwFrameProc, connCompLabels, connCompStats, connCompCentroids, 8, CV_16U);

    int widerConnComp[2] = {0 , 0};                     // Format: (label , numPixels)

    for (int i = 1; i < connCompStats.rows; i++) {      // Start from 1 to ignore background (black pixels)

        int numPixels = static_cast<int>(connCompStats.at<char32_t>(i, 4));

        if (numPixels >= widerConnComp[1]) {
            widerConnComp[0] = i;
            widerConnComp[1] = numPixels;
        }
    }

    Mat bwFrameFiltered = (connCompLabels == widerConnComp[0]);

    // Return a binary image with only the lips as white pixels
    return bwFrameFiltered;
}


/**
 * @brief Identify points on the boundary of the lips from the lips binary image
 * @param binaryImg Binary image of the lips
 * @return Vector of points on the boundary of the lips
 */
QVector<QPoint> ProcessFrame::extractPointsOnLipsEdge(Mat &binaryImg)
{
    // Two data structures are needed as 2 points exists for a same column
    QVector<QPoint> upperLipPts;
    QVector<QPoint> lowerLipPts;

    // Skip columns to reduce execution time
    int colsDownSampling = 50;
    int numColsPerScan = binaryImg.cols / colsDownSampling;

    // Scan each selected columns
    for (int colIdx = 0; colIdx < binaryImg.cols; colIdx += numColsPerScan) {

        bool upperLipFound = false;
        bool lowerLipFound = false;
        QPoint lowerPoint;

        // Scan each row
        for (int rowIdx = 0; rowIdx < binaryImg.rows; rowIdx++) {

            int pixelIntensity = static_cast<int>(binaryImg.at<uchar>(rowIdx, colIdx));

            // Append first point where black pixel changes to white (upper lip)
            if ( pixelIntensity == 255 && !upperLipFound) {
                upperLipPts.append(QPoint(colIdx, rowIdx));
                upperLipFound = true;
            }

            // Create a point at the location where a white pixel changes to black (lower lip)
            else if ( pixelIntensity == 0 && upperLipFound && !lowerLipFound ) {
                lowerPoint.setX(colIdx);
                lowerPoint.setY(rowIdx);
                lowerLipFound = true;
            }

            // Manages cases where a black patch of pixels exists between upper and lower lips
            else if (lowerLipFound && pixelIntensity == 255) {
                lowerLipFound = false;
            }
        }

        // Add lower point if found
        if(!lowerPoint.isNull()) {
            lowerLipPts.push_front(lowerPoint); // Push to front to make line creation easier
        }

        // Add pixel of last row as lower lip if not found
        if (upperLipFound && !lowerLipFound) {
            lowerLipPts.push_front(QPoint(colIdx, binaryImg.rows - 1));
        }
    }


    QVector<QPoint> lipsPoints;

    foreach (QPoint point, upperLipPts) {
        lipsPoints.append(point);
    }

    foreach (QPoint point, lowerLipPts) {
        lipsPoints.append(point);
    }

    return lipsPoints;
}


