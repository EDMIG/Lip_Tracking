#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <qmath.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    height = ui->bwImage->height();
    width = ui->bwImage->width();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectVideoButton_clicked()
{
//    QString defaultPath = QCoreApplication::applicationDirPath() + "/../../Data";
    QString defaultPath = "C:/Users/nsebkhi3/GitHub/Perso/Lip_Tracking/Data";

    qDebug() << "Default path: " << defaultPath;

    videoFilePath = QFileDialog::getOpenFileName(this,
                                                    tr("Open Video"), defaultPath, tr("Video Files (*.avi)"));

    ui->videoFilePathText->setText(videoFilePath);

    startLipTracking(videoFilePath);
}

void MainWindow::startLipTracking(QString videoFilePath)
{

    bool videoOpen = video.open(videoFilePath.toStdString());

    if (!videoOpen) {
        QMessageBox msgBox;
        msgBox.setText("The video cannot be opened.");
        msgBox.exec();

        return;
    }


    int numFrames = static_cast<int>(video.get(CV_CAP_PROP_FRAME_COUNT));
    ui->frameSlider->setRange(0, numFrames - 1);
    on_frameSlider_valueChanged(0);
}


void MainWindow::on_frameSlider_valueChanged(int value)
{
    video.set(CAP_PROP_POS_FRAMES, value);
    video >> frame;

    cv::resize(frame, frame, Size(320, 240), 0, 0, INTER_AREA);
    cv::cvtColor(frame, frame, CV_BGR2RGB);       // invert BGR to RGB

    Mat bwImage = extractLipsAsBWImg(frame);

    QImage image  = QImage((uchar*)bwImage.data, bwImage.cols, bwImage.rows, bwImage.step, QImage::Format_Grayscale8);
    QPixmap pixmap = QPixmap::fromImage(image).scaled(width, height);

    ui->bwImage->setPixmap(pixmap);
}

Mat MainWindow::extractLipsAsBWImg(Mat &frame)
{
    Mat formattedFrame(frame.rows, frame.cols, CV_32FC3);
    frame.convertTo(formattedFrame, CV_32FC3, 1.0/255.0);

//    Vec3b frameIntensity = frame.at<Vec3b>(0, 0);
//    uchar red  = frameIntensity.val[0];
//    uchar green = frameIntensity.val[1];
//    uchar blue   = frameIntensity.val[2];

//    Vec3f formatIntensity = formattedFrame.at<Vec3f>(0, 0);
//    float redFormat  = formatIntensity.val[0];
//    float greenFormat = formatIntensity.val[1];
//    float blueFormat   = formatIntensity.val[2];

//    qDebug() << QString("Raw: R = %1  G = %2  B = %3").arg(red).arg(green).arg(blue);
//    qDebug() << QString("Formatted: R = %1  G = %2  B = %3").arg(redFormat).arg(greenFormat).arg(blueFormat);

    // Split the image into different channels
    std::vector<Mat> rgbChannels;
    cv::split(formattedFrame, rgbChannels);

//    qDebug() << "Size RGB Channles: " << rgbChannels.size();

    Mat redChannel = rgbChannels[0];
    Mat greenChannel = rgbChannels[1];

//    Vec3f redIntensity = redChannel.at<Vec3f>(0, 0);
//    float redPixel  = redIntensity.val[0];

//    Vec3f greenIntensity = greenChannel.at<Vec3f>(0, 0);
//    float greenPixel  = greenIntensity.val[0];

//    qDebug() << QString("Intensity: R = %1  G = %2").arg(redPixel).arg(greenPixel);

    Mat bwFrame(frame.rows, frame.cols, CV_32FC1);
    cv::add(greenChannel, 0.00000001, bwFrame);
    cv::divide(redChannel, bwFrame, bwFrame);
    cv::log(bwFrame, bwFrame);

//    qDebug() << QString("Intensity: %1 %2 %3").arg(bwFrame.at<float>(235, 0)).arg(bwFrame.at<float>(235,1)).arg(bwFrame.at<float>(235,2));

    Mat frameVect = bwFrame.reshape(0, 1);
    qDebug() << QString("Size frameVect: row = %1 -  cols = %2").arg(frameVect.rows).arg(frameVect.cols);

    qDebug() << QString("Pre-frameVect: 1st = %1 - last = %2 ").arg(frameVect.at<float>(0,0)).arg(frameVect.at<float>(0,76799));
    cv::sort(frameVect, frameVect, CV_SORT_ASCENDING);
    qDebug() << QString("Sorted frameVect: 1st = %1 - last = %2 ").arg(frameVect.at<float>(0,0)).arg(frameVect.at<float>(0,76799));

    double thres_coeff = 0.14;
    int threshIdx = (frameVect.cols - 1) - qFloor(frameVect.cols * thres_coeff);
    float threshold = frameVect.at<float>(0, threshIdx);


////    bwFrame.convertTo(bwFrame, CV_8UC1, 255);
//    cv::threshold(bwFrame, bwFrame, 0, 1, CV_THRESH_BINARY | CV_THRESH_OTSU);

    return bwFrame;
}

QPixmap MainWindow::getPixmap(Mat &frame)
{
    QImage image  = QImage((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_Grayscale8);
    return QPixmap::fromImage(image).scaled(width, height);
}

void MainWindow::printMat(Mat &frame)
{
//    qDebug() << QString("First elt: R = %1  G = %2  B = %3").arg(frame.at<>(0,0,1)).arg(frame(0,0,2)).arg(frame(0,0,3));
//    qDebug() << QString("First elt: R = %1  G = %2  B = %3").arg(5).arg(7).arg(1);
}
