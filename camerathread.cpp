#include "camerathread.h"
#include <QThread>
#include <time.h>
#include <iostream>
#include <string>

extern QLabel *imagelabel;
extern QLabel *fps__label;

CameraThread::CameraThread()
{

}

CameraThread::~CameraThread()
{

}

/** カメラ データ スレッドを取得する */
void CameraThread::dowork()
{
//    v4l.V4l_Init("/dev/video0", V4L2_FPS);                          /** 初期化 */
    v4l.V4l_Init(USE_DEVIDE, V4L2_FPS);                             /** 初期化 */

    while(1){
        clock_t start_clock = clock();
        if((QThread::currentThread()->isInterruptionRequested()))   {/** このスレッドを中断するシグナル */
            v4l.Close_Camera();                                     /** カメラをオフにする */
            break;
        }
        currentimage = v4l.Get_image();                             /** 画像データのフレームを取得する */
        clock_t end_clock = clock();
        imagelabel->setPixmap(QPixmap::fromImage(currentimage));    /** ラベルに現在イメージを表示 */
        double process_sec = (double)(end_clock - start_clock) / (double)CLOCKS_PER_SEC;
        int fps = (int)1/process_sec;
        fps__label->setText(QString::number(fps));                  /** ラベルにFPSを表示 */
    }

}
