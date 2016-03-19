#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QVector>

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
    frame.convertTo(formattedFrame, CV_32FC3);

    // Split the image into different channels
    std::vector<Mat> rgbChannels;
    cv::split(formattedFrame, rgbChannels);

    Mat redChannel = rgbChannels[0];
    Mat greenChannel = rgbChannels[1];


    Mat bwFrame(frame.rows, frame.cols, CV_32FC1);
    cv::add(greenChannel, 0.00000001, bwFrame);
    cv::divide(redChannel, bwFrame, bwFrame);
    cv::log(bwFrame, bwFrame);

//    qDebug() << "BwFrame value: " << ;

    bwFrame.convertTo(bwFrame, CV_8UC1);
    cv::threshold(bwFrame, bwFrame, 0, 1, THRESH_OTSU);

    return bwFrame;
}
