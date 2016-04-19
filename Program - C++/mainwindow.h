#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qcustomplot.h>
#include <webcamreader.h>
#include <processframe.h>
#include <QThread>

// Include OpenCV neeeded headers
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace cv;

// Register OpenCV and custom classes to be used in Signal/Slot connection
// Note: cannot compile if returned values are not assigned to a constant int
const int dontcare1 = qRegisterMetaType<Mat>("Mat");
const int dontcare2  = qRegisterMetaType<Mat*>("Mat*");
const int dontcare3 = qRegisterMetaType< QVector<QPoint> >("QVector<QPoint>");

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    

private slots:
    void on_selectVideoButton_clicked();
    void on_frameSlider_valueChanged(int value);
    void on_webcamButton_clicked(bool checked);
    void startLipTracking(Mat* frame);
    void updateBinaryImage(Mat frame);
    void updateFinalImage(Mat frame, QVector<QPoint> lipsPos);

private:
    Ui::MainWindow *ui;

    // Frame dimensions
    Mat frame;
    int bwHeight, bwWidth;
    int finalHeight, finalWidth;

    VideoCapture video;

    QCPCurve *lipsCurve     = 0;

    // Webcam reader thread
    QThread *webcamThread   = 0;
    WebCamReader *webcam    = 0;

private:
    void setLipsCurve();
    void printMat(Mat &frame, QString filename);

signals:
    void stopWebcam();
    void newFrameAvail(Mat frame);
};

#endif // MAINWINDOW_H
