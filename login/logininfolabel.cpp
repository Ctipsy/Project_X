#include "login/logininfolabel.h"


LoginInfoLabel::LoginInfoLabel()
{
    //设置面板基本属性
    this ->resize(301, 182);//设置大小
    //去掉窗体边框和总是显示在屏幕最前面,但要和showNormal一起用
    this ->setWindowFlags(Qt::FramelessWindowHint);
    this ->setAttribute(Qt::WA_TranslucentBackground);//设置背景透明

    //设置登录成功显示位置
//    double width = QApplication::desktop() ->width();
//    double height = QApplication::desktop() ->height();
//    this ->move(QPoint(width / this ->x(), height - this ->y()));
}

//重画面板
void LoginInfoLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap backgroundImage;
    backgroundImage.load(":/image/login/loginLabel.png");

    this ->setMask(backgroundImage.mask());
    painter.drawPixmap(0, 0, 301, 182, backgroundImage);
}
