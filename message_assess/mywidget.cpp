#include "mywidget.h"


void mywidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawLine(QPointF(40,60),QPointF(610,60));
}
