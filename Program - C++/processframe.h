#ifndef PROCESSFRAME_H
#define PROCESSFRAME_H

#include <QThread>

// OpenCV
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace cv;

class ProcessFrame : public QThread
{
    Q_OBJECT

private:
    Mat frame;

public:
    ProcessFrame(Mat frame,  QObject *parent = 0);
    void run();

signals:
    void binaryImg(Mat frame);
    void lipsPos(Mat frame, QVector<QPoint> pos);

private:
    Mat extractLipsAsBWImg(Mat &frame);
    QVector<QPoint> extractPointsOnLipsEdge(Mat &binaryImg);
};

#endif // PROCESSFRAME_H
