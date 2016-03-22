#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_frameSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    VideoCapture video;
    Mat frame;

    int bwHeight, bwWidth;
    int finalHeight, finalWidth ;

private:
    void startLipTracking(QString videoFilePath);
    Mat extractLipsAsBWImg(Mat &frame);
    void printMat(Mat &frame, QString filename);
};

#endif // MAINWINDOW_H
