#include "webcamreader.h"
#include <QCameraInfo>
#include <QMutexLocker>

WebCamReader::WebCamReader(QObject *parent) : QThread(parent)
{
    // Find index of TTS supported camera in the list of connected cameras
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    int cameraIdx = 0;

    for (int i = 0; i < cameras.size(); i++) {
        // Get the name of the camera
        QString cameraName = cameras.at(i).description();

        // Search if name matches one of TTS camera
        if (cameraName.contains("LifeCam") || cameraName.contains("PC CAMERA")) {
            cameraIdx = i;
            break;
        }
    }

    video.open(cameraIdx);
    qDebug() << "OK: Camera found as " << cameras.at(cameraIdx).description() << "\n";

    exec = true;
}


void WebCamReader::run(){

    Mat frame;

    while (exec) {

        bool readSuccessful = video.read(frame);

        if (readSuccessful) {
            emit newFrame(&frame);
        }
    }

    video.release();
}

void WebCamReader::stop() {
    mutex.lock();
    exec = false;
    mutex.unlock();
}
