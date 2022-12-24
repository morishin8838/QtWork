#include "widget.h"
#include <QApplication>
#include <QThread>
#include "camerathread.h"

QThread *ImageThread = new QThread;                         /** カメラ データ スレッドを取得する */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;

    CameraThread *camera_worker = new CameraThread;

    camera_worker->moveToThread(ImageThread);               /** スレッドを作成 */
    QObject::connect(ImageThread, SIGNAL(started()), camera_worker, SLOT(dowork()));
    QObject::connect(ImageThread, SIGNAL(finished()),camera_worker, SLOT(deleteLater()));   /** スレッドが終了すると、リソースが解放されます */
    ImageThread->start();                                   /** スレッドを開始 */
    w.show();

    return a.exec();
}
