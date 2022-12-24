#include "widget.h"
#include "ui_widget.h"


QLabel *imagelabel = NULL;
QLabel *fps__label = NULL;
QLabel *unit_label = NULL;

extern QThread *ImageThread;    /** カメラ データ スレッドを取得する */

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    imagelabel = ui->label;        /** インターフェイス上でラベルオブジェクトを取得し、「カメラデータ取得スレッド」で取得した画像データをこのラベルに表示できるようにします */
    fps__label = ui->label_3;
    unit_label = ui->label_2;

    fps__label->setStyleSheet("QLabel {color: #518a87;}");
    unit_label->setStyleSheet("QLabel {color: #518a87;}");
}

Widget::~Widget()
{
    delete ui;
}

/** ウィンドウを閉じるときのハンドリング関数 */
void Widget::closeEvent(QCloseEvent *)
{
    ImageThread->requestInterruption(); /** 割り込み要求 */
    ImageThread->quit();                /** スレッドを閉じる */
    ImageThread->wait();                /** 同期終了 */
}
