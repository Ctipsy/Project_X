#ifndef MYWIDGET_H
#define MYWIDGET_H
#include <QWidget>
#include <QPainter>
#include <QPen>
namespace Ui {
class mywidget;
}

class mywidget : public QWidget
{
    Q_OBJECT
protected:
    void paintEvent(QPaintEvent *);
};

#endif // MYWIDGET_H


