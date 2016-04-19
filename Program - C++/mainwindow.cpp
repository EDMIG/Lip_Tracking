/* *******************************************************************
 * This class is the main GUI manager.
 * It obtains the frames to be processed and creates the processing threads.
 * It also displays the processing results (binary image and lips boundary).
 *
 * Author: Nordine Sebkhi
 * *******************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <qmath.h>
#include <QFile>


/**
 * @brief Constructor that sets desired frame dimensions and output display
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set dimensions according to widget size
    // This will help resizing frames to fit widgets
    bwHeight    = ui->bwImage->height();
    bwWidth     = ui->bwImage->width();

    finalHeight = ui->finalImage->height();
    finalWidth  = ui->finalImage->width();

    // Set the output graph where lips boundary will be shown
    setLipsCurve();
}


/**
 * @brief Render the output graph that shows lips boundary
 */
void MainWindow::setLipsCurve()
{
    /* Set the QCP curve where lips boundaries are identified */
    QCPAxisRect *pixelAxis = ui->finalImage->axisRect();
    lipsCurve = new QCPCurve(pixelAxis->axis(QCPAxis::atBottom), pixelAxis->axis(QCPAxis::atLeft));
    ui->finalImage->addPlottable(lipsCurve);

    // TODO: The ranges should depend on the downsampled image resolution, not hard-coded
    pixelAxis->axis(QCPAxis::atBottom)->setRange(0, 319);
    pixelAxis->axis(QCPAxis::atLeft)->setRange(0, 239);
    pixelAxis->axis(QCPAxis::atLeft)->setRangeReversed(true);   // Image indexing starts from the upper left corner

    lipsCurve->setPen(QPen(Qt::green));
    lipsCurve->setLineStyle(QCPCurve::lsLine);
    lipsCurve->setScatterStyle(QCPScatterStyle::ssCircle);
}


/**
 * @brief Process video frames from a file
 */
void MainWindow::on_selectVideoButton_clicked()
{
    ui->frameSlider->setEnabled(true);

    /* Get a video filepath by prompting a file explorer window
     * and set default folde to Data */
    QString defaultPath     = "C:/Users/nsebkhi3/GitHub/Perso/Lip_Tracking/Data";
    QString videoFilePath   = QFileDialog::getOpenFileName(this, "Open Video", defaultPath, "Video Files (*.avi)");
    ui->videoFilePathText->setText(videoFilePath);

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

/**
 * @brief Process the frame selected by the slider
 * @param value position of the slider which represents a frame index
 */
void MainWindow::on_frameSlider_valueChanged(int value)
{
    // Get the frame associated to the slider value
    video.set(CAP_PROP_POS_FRAMES, value);

    Mat frame;
    video >> frame;

    startLipTracking(&frame);
}


/**
 * @brief Process video frames from a live webcam feed
 * @param checked
 */
void MainWindow::on_webcamButton_clicked(bool checked)
{
    if (checked) {

        ui->frameSlider->setEnabled(false);

        // Start a new thread to acquire frames from a webcam
        webcam = new WebCamReader();

        connect(webcam, SIGNAL(newFrame(Mat*)), this, SLOT(startLipTracking(Mat*)));
        connect(webcam, SIGNAL(finished()), webcam, SLOT(deleteLater()));

        webcam->start();

        ui->webcamButton->setText("Stop Webcam");
    }

    else {
        webcam->stop();
        ui->webcamButton->setText("Start Webcam");
    }
}


/**
 * @brief Find lip boundary in a video frame
 * @param framePtr frame to be processed
 */
void MainWindow::startLipTracking(Mat *framePtr)
{
    // Create a new thread to process the frame
    ProcessFrame *procFrame = new ProcessFrame(*framePtr, this);

    connect(procFrame, SIGNAL(binaryImg(Mat)), this, SLOT(updateBinaryImage(Mat)));
    connect(procFrame, SIGNAL(lipsPos(Mat,QVector<QPoint>)), this, SLOT(updateFinalImage(Mat,QVector<QPoint>)));
    connect(procFrame, SIGNAL(finished()), procFrame, SLOT(deleteLater()));

    procFrame->start();
}


/**
 * @brief Display the black & white (aka binary) frame
 * @param frame binary frame
 */
void MainWindow::updateBinaryImage(Mat frame) {
    QImage bwImg        = QImage((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_Grayscale8);
    QPixmap bwPixmap    = QPixmap::fromImage(bwImg).scaled(bwWidth, bwHeight);
    ui->bwImage->setPixmap(bwPixmap);
}


/**
 * @brief Display the lips boundary
 * @param frame     original frame
 * @param lipsPos   points that form the lips boundary
 */
void MainWindow::updateFinalImage(Mat frame, QVector<QPoint> lipsPos) {

    // Clear previous lips boundary points if any
    lipsCurve->clearData();

    // Add each point to the lips curve
    foreach (QPoint point, lipsPos) {
        lipsCurve->addData(point.x(), point.y());
    }

    // Set original frame as background of the ploy
    QImage finalImg        = QImage((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    QPixmap finalPixmap    = QPixmap::fromImage(finalImg).scaled(finalWidth, finalHeight);

    ui->finalImage->axisRect()->setBackground(finalPixmap);
    ui->finalImage->replot();
}


/**
 * @brief Helper method that saves a video frame values into a file
 * @param frame     frame whose values will be saved
 * @param filename  file location to save frame values
 */
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


/**
 * @brief Destructor
 */
MainWindow::~MainWindow()
{
    delete ui;
}




