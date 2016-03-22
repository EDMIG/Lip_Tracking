#ifndef WEBCAMREADER_H
#define WEBCAMREADER_H

#include <QObject>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace cv;

const int dontcare = qRegisterMetaType<Mat*>("Mat*");

class WebCamReader : public QObject
{
    Q_OBJECT

public:
    explicit WebCamReader(QObject *parent = 0);

signals:
    void newFrame(Mat* frame);

private:
    VideoCapture video;
    bool exec;

public slots:
    void run();
    void stop();
};

#endif // WEBCAMREADER_H
