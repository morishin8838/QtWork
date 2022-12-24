#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QObject>
#include "v4l2.h"
#include <QImage>
#include "widget.h"
#include "ui_widget.h"
#include "opencv2/opencv.hpp"

class CameraThread : public QObject
{
    Q_OBJECT
public:
    CameraThread();
    ~CameraThread();
    V4L2 v4l;
    QImage currentimage;            /** カメラからの画像データのフレームを保存し、ラベルに表示します */

signals:

public slots:
    void dowork();                  /** カメラ データ スレッドを取得する */
};

#endif // CAMERATHREAD_H
