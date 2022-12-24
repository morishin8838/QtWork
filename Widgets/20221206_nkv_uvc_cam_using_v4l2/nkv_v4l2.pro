#-------------------------------------------------
# 大元のリポジトリはこちらです。
# https://github.com/jianfenggithub/V4L2_example
#-------------------------------------------------

QT += core gui
QT += widgets
#CONFIG += c++11
CONFIG += c++17

TARGET = nkv_v4l2
TEMPLATE = app
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

SOURCES += main.cpp\
        widget.cpp \
    v4l2.cpp \
    camerathread.cpp

HEADERS  += widget.h \
    v4l2.h \
    camerathread.h

FORMS    += widget.ui

#KV260向けにビルドする場合
MACHINE_PLATFORM = "KV260"
equals (MACHINE_PLATFORM, "KV260"){
    message("Compiling for KV260 evironment")
    KV260_ROOTFS="/home/a8838/Xilinx/LinuxForKV260/rootfs"
    OPEN_CV_DIR_INC =$$KV260_ROOTFS/usr/include/opencv4
    OPEN_CV_DIR_LIB =$$KV260_ROOTFS/usr/local/lib
}else{
    OPEN_CV_DIR_INC = "/home/a8838/Downloads/opencv3.4.9/include"
    OPEN_CV_DIR_LIB = "/home/a8838/Downloads/opencv3.4.9/lib"
}
#OpenCV
INCLUDEPATH += $$OPEN_CV_DIR_INC
DEPENDPATH += $$OPEN_CV_DIR_LIB
LIBS += -L$$OPEN_CV_DIR_LIB -lopencv_highgui -lopencv_core -lopencv_imgcodecs -lopencv_imgproc
