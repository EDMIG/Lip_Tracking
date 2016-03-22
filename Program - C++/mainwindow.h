#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qcustomplot.h>
#include <webcamreader.h>
#include <QThread>

// OpenCV
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace cv;

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
    void on_webcamButton_clicked();
    void on_frameSlider_valueChanged(int value);
    void startLipTracking(Mat* frame);

private:
    Ui::MainWindow *ui;
    VideoCapture video;
    Mat frame;

    int bwHeight, bwWidth;
    int finalHeight, finalWidth;

    QCPCurve *lipsCurve;

    QThread *webcamThread = 0;
    WebCamReader *webcam = 0;

private:
    Mat extractLipsAsBWImg(Mat &frame);
    QVector<QPoint> extractPointsOnLipsEdge(Mat &binaryImg);
    void setLipsCurve();
    void printMat(Mat &frame, QString filename);
};

#endif // MAINWINDOW_H
