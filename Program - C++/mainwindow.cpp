#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <qmath.h>
#include <QFile>

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

    Mat bwFrame = extractLipsAsBWImg(frame);

    QImage image  = QImage((uchar*)bwFrame.data, bwFrame.cols, bwFrame.rows, bwFrame.step, QImage::Format_Grayscale8);
    QPixmap pixmap = QPixmap::fromImage(image).scaled(width, height);

    ui->bwImage->setPixmap(pixmap);

    QImage finalImg  = QImage((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    QPixmap finalPixmap = QPixmap::fromImage(finalImg).scaled(width, height);
    ui->finalImage->setPixmap(finalPixmap);
}

Mat MainWindow::extractLipsAsBWImg(Mat &frame)
{
    Mat formattedFrame(frame.rows, frame.cols, CV_32FC3);
    frame.convertTo(formattedFrame, CV_32FC3, 1.0/255.0);

    // Split the image into different channels
    std::vector<Mat> rgbChannels;
    cv::split(formattedFrame, rgbChannels);

    Mat redChannel = rgbChannels[0];
    Mat greenChannel = rgbChannels[1];

    Mat bwFrame(frame.rows, frame.cols, CV_32FC1);
    cv::add(greenChannel, 0.00000001, bwFrame);
    cv::divide(redChannel, bwFrame, bwFrame);
    cv::log(bwFrame, bwFrame);

    Mat frameVect;
    bwFrame.reshape(0, 1).copyTo(frameVect);
    cv::sort(frameVect, frameVect, CV_SORT_ASCENDING);

    double thres_coeff = 0.20;
    int threshIdx = (frameVect.cols - 1) - qFloor(frameVect.cols * thres_coeff);
    float thresVal = frameVect.at<float>(0, threshIdx);

    qDebug() << "Threshold = " << thresVal;

    Mat bwFrameProc = bwFrame > thresVal;

    printMatFloat(bwFrame, "bwFrame.txt");

    Mat connCompLabels;
    cv::connectedComponents(bwFrameProc, connCompLabels, 8, CV_16U);

    printMatFloat(connCompLabels, "connCompLabels.txt");

    return bwFrameProc;
}

QPixmap MainWindow::getPixmap(Mat &frame)
{
    QImage image  = QImage((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_Grayscale8);
    return QPixmap::fromImage(image).scaled(width, height);
}

void MainWindow::printMatFloat(Mat &frame, QString filename)
{
    QString filePath = "C:/Users/nsebkhi3/GitHub/Perso/Lip_Tracking/Data/" + filename;
    QFile outFile(filePath);
    QTextStream out(&outFile);
    outFile.open(QIODevice::WriteOnly | QIODevice::Text);

    for (int row = 0; row < frame.rows; row++) {

        for (int col = 0; col < frame.cols; col++) {

            if (frame.type() == CV_32FC1) {
                out << frame.at<float>(row, col) << " ";
            }

            else if (frame.type() == CV_16U) {
                out << frame.at<char16_t>(row, col) << " ";
            }
        }
         out << "\n";
    }


    outFile.close();
}
