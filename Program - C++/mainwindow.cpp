#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <qmath.h>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bwHeight    = ui->bwImage->height();
    bwWidth     = ui->bwImage->width();

    finalHeight = ui->finalImage->height();
    finalWidth  = ui->finalImage->width();
}


void MainWindow::on_selectVideoButton_clicked()
{
    /* Get a video filepath by prompting a file explorer window
     * and set default folde to Data */

    QString defaultPath = "C:/Users/nsebkhi3/GitHub/Perso/Lip_Tracking/Data";

    QString videoFilePath = QFileDialog::getOpenFileName(this,
                                                    tr("Open Video"), defaultPath, tr("Video Files (*.avi)"));
    ui->videoFilePathText->setText(videoFilePath);
    startLipTracking(videoFilePath);
}

void MainWindow::startLipTracking(QString videoFilePath)
{
    /* Open the video and set the slider length to the num of frames
     * Return if video cannot be opened */

    bool videoOpen = video.open(videoFilePath.toStdString());

    if (!videoOpen) {
        QMessageBox msgBox;
        msgBox.setText("The video cannot be opened.");
        msgBox.exec();
        return;
    }

    int numFrames = static_cast<int>(video.get(CV_CAP_PROP_FRAME_COUNT));
    ui->frameSlider->setRange(0, numFrames - 1);
    on_frameSlider_valueChanged(0);     // Force the first frame to be processed after loading video
}


void MainWindow::on_frameSlider_valueChanged(int value)
{
    // Get the frame associated to the slider value
    video.set(CAP_PROP_POS_FRAMES, value);
    video >> frame;

    // Lower frame resolution to reduce execution time
    cv::resize(frame, frame, Size(320, 240), 0, 0, INTER_AREA);

    // OpenCV frame color format is by default BGR. Invert color to RGB for display
    cv::cvtColor(frame, frame, CV_BGR2RGB);

    // Process frame to extract a lips into a binary image
    Mat bwFrame = extractLipsAsBWImg(frame);

    QImage bwImg  = QImage((uchar*)bwFrame.data, bwFrame.cols, bwFrame.rows, bwFrame.step, QImage::Format_Grayscale8);
    QPixmap bwPixmap = QPixmap::fromImage(bwImg).scaled(bwWidth, bwHeight);
    ui->bwImage->setPixmap(bwPixmap);

}

/**
 * @brief Construct a binary image with lips pixels as white (255) and others as black (0)
 * @param frame RGB image
 * @return binary image
 */
Mat MainWindow::extractLipsAsBWImg(Mat &frame)
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
    cv::add(greenChannel, 0.00000001, bwFrame);
    cv::divide(redChannel, bwFrame, bwFrame);
    cv::log(bwFrame, bwFrame);

    // Compute the threshold to render lips-like area to white and other areas to black
    Mat frameVect;
    bwFrame.reshape(0, 1).copyTo(frameVect);            // Flatten out the frame into a row vector
    cv::sort(frameVect, frameVect, CV_SORT_ASCENDING);
    double thres_coeff = 0.20;                          // Variable that sets strength of discrimination (lower = more discrimination)
    int threshIdx = (frameVect.cols - 1) - qFloor(frameVect.cols * thres_coeff);
    float thresVal = frameVect.at<float>(0, threshIdx);

    // Create the binary image
    Mat bwFrameProc = bwFrame > thresVal;
    printMat(bwFrame, "bwFrame.txt");

    // Keep only the biggest agglomerate of white pixels as more likely related to lips
    Mat connCompLabels, connCompStats, connCompCentroids;
    cv::connectedComponentsWithStats(bwFrameProc, connCompLabels, connCompStats, connCompCentroids, 8, CV_16U);
    printMat(connCompLabels, "connCompLabels.txt");
    printMat(connCompStats, "connCompStats.txt");

    int widerConnComp[2] = {0 , 0};                     // Format: (label , numPixels)

    for (int i = 1; i < connCompStats.rows; i++) {      // Start from 1 to ignore background (black pixels)

        int numPixels = static_cast<int>(connCompStats.at<char32_t>(i, 4));

        if (numPixels >= widerConnComp[1]) {
            widerConnComp[0] = i;
            widerConnComp[1] = numPixels;
        }
    }

    Mat bwFrameFiltered = (connCompLabels == widerConnComp[0]);
    printMat(bwFrameFiltered, "bwFrameFiltered.txt");

    // Return a binary image with only the lips as white pixels
    return bwFrameFiltered;
}


void MainWindow::printMat(Mat &frame, QString filename)
{
    QString filePath = "C:/Users/nsebkhi3/GitHub/Perso/Lip_Tracking/Data/" + filename;
    QFile outFile(filePath);
    QTextStream out(&outFile);
    outFile.open(QIODevice::WriteOnly | QIODevice::Text);

    for (int row = 0; row < frame.rows; row++) {

        for (int col = 0; col < frame.cols; col++) {

            switch(frame.type()) {

            case CV_8U:
                out << frame.at<uchar>(row, col) << " ";
                break;

            case CV_16U:
                out << frame.at<char16_t>(row, col) << " ";
                break;

            case CV_32S:
                out << frame.at<char32_t>(row, col) << " ";
                break;

            case CV_32FC1:
                out << frame.at<float>(row, col) << " ";
                break;

            default:
                break;
            }
        }

        out << "\n";
    }

    outFile.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}
