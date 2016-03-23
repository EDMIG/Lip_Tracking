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

    setLipsCurve();

//    webcamThread = new QThread(this);
//    connect(this, SIGNAL(stopWebcam()), webcamThread, SLOT(quit()));
}


void MainWindow::on_selectVideoButton_clicked()
{
    /* Get a video filepath by prompting a file explorer window
     * and set default folde to Data */


    ui->frameSlider->setEnabled(true);

    QString defaultPath = "C:/Users/nsebkhi3/GitHub/Perso/Lip_Tracking/Data";

    QString videoFilePath = QFileDialog::getOpenFileName(this, "Open Video", defaultPath, "Video Files (*.avi)");
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

void MainWindow::on_frameSlider_valueChanged(int value)
{
    // Get the frame associated to the slider value
    video.set(CAP_PROP_POS_FRAMES, value);

    Mat frame;
    video >> frame;

    startLipTracking(&frame);
}


void MainWindow::startLipTracking(Mat *framePtr)
{
    ProcessFrame *procFrame = new ProcessFrame(*framePtr, this);

    connect(procFrame, SIGNAL(binaryImg(Mat)), this, SLOT(updateBinaryImage(Mat)));
    connect(procFrame, SIGNAL(lipsPos(Mat,QVector<QPoint>)), this, SLOT(updateFinalImage(Mat,QVector<QPoint>)));
    connect(procFrame, SIGNAL(finished()), procFrame, SLOT(deleteLater()));
    procFrame->start();
}

void MainWindow::updateBinaryImage(Mat frame) {
    QImage bwImg        = QImage((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_Grayscale8);
    QPixmap bwPixmap    = QPixmap::fromImage(bwImg).scaled(bwWidth, bwHeight);
    ui->bwImage->setPixmap(bwPixmap);
}

void MainWindow::updateFinalImage(Mat frame, QVector<QPoint> lipsPos) {

    lipsCurve->clearData();

    foreach (QPoint point, lipsPos) {
        lipsCurve->addData(point.x(), point.y());
    }

    QImage finalImg        = QImage((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    QPixmap finalPixmap    = QPixmap::fromImage(finalImg).scaled(finalWidth, finalHeight);

    ui->finalImage->axisRect()->setBackground(finalPixmap);
    ui->finalImage->replot();
}

void MainWindow::setLipsCurve()
{
    /* Set the QCP curve where lips boundaries are identified */

    QCPAxisRect *pixelAxis = ui->finalImage->axisRect();
    lipsCurve = new QCPCurve(pixelAxis->axis(QCPAxis::atBottom), pixelAxis->axis(QCPAxis::atLeft));
    ui->finalImage->addPlottable(lipsCurve);

    pixelAxis->axis(QCPAxis::atBottom)->setRange(0, 319);
    pixelAxis->axis(QCPAxis::atLeft)->setRange(0, 239);
    pixelAxis->axis(QCPAxis::atLeft)->setRangeReversed(true);

    lipsCurve->setPen(QPen(Qt::green));
    lipsCurve->setLineStyle(QCPCurve::lsLine);
    lipsCurve->setScatterStyle(QCPScatterStyle::ssCircle);

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



void MainWindow::on_webcamButton_clicked(bool checked)
{
    if (checked) {

        ui->frameSlider->setEnabled(false);

        webcam = new WebCamReader();

        connect(webcam, SIGNAL(newFrame(Mat*)), this, SLOT(startLipTracking(Mat*)));
//        connect(this, SIGNAL(stopWebcam()), webcam, SLOT(stop()), Qt::DirectConnection);
        connect(webcam, SIGNAL(finished()), webcam, SLOT(deleteLater()));

        webcam->start();

        ui->webcamButton->setText("Stop Webcam");
    }

    else {

        webcam->stop();
        ui->webcamButton->setText("Start Webcam");
    }
}
