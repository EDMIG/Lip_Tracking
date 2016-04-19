#ifndef WEBCAMREADER_H
#define WEBCAMREADER_H

#include <QObject>
#include <QMutex>
#include <QThread>

// Include OpenCV neeeded headers
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace cv;

class WebCamReader : public QThread
{
    Q_OBJECT

public:
    explicit WebCamReader(QObject *parent = 0);
    void run();

signals:
    void newFrame(Mat* frame);

private:
    VideoCapture video;
    bool exec;
    QMutex mutex;

public slots:
    void stop();
};

#endif // WEBCAMREADER_H
