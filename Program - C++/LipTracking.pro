#-------------------------------------------------
#
# Project created by QtCreator 2016-03-19T11:20:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LipTracking
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui


# OPEN CV
INCLUDEPATH += "C:/dev/OpenCV/OpenCV_3.0_Installer/opencv/build/include"

LIBS += -L"C:/dev/OpenCV/OpenCV_3.0_Installer/Build_Shared_32_bit/lib/Release" \
            -lopencv_flann300       \
            -lopencv_hal300         \
            -lopencv_highgui300     \
            -lopencv_imgcodecs300   \
            -lopencv_imgproc300     \
            -lopencv_ml300          \
            -lopencv_objdetect300   \
            -lopencv_photo300       \
            -lopencv_shape300       \
            -lopencv_stitching300   \
            -lopencv_superres300    \
            -lopencv_ts300          \
            -lopencv_video300       \
            -lopencv_videoio300     \
            -lopencv_videostab300   \
            -lopencv_calib3d300     \
            -lopencv_core300        \
            -lopencv_features2d300
