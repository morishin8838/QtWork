#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QThread>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    Ui::Widget *ui;
    void closeEvent(QCloseEvent *);                 /** ウィンドウを閉じるときのハンドリング関数 */
private:

};

#endif // WIDGET_H
