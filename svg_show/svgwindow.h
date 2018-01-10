/*
 * 实现一个带滚动条显示区域的函数所在的文件.
 * 使图片在放大到超过主窗口大小时, 能通过拖曳
 */
#ifndef SVGWINDOW_H
#define SVGWINDOW_H

#include <QScrollArea>
#include "svgwidget.h"

class SvgWindow : public QScrollArea
{
    Q_OBJECT
public:
    SvgWindow(QWidget *parent = 0);
    void setFile(QString);
    void mousePressEvent (QMouseEvent *);
    void mouseMoveEvent (QMouseEvent *);
private:
    SvgWidget *svgWidget;
    QPoint mousePressPos;
    QPoint scrollBarValueOnMousePress;
};

#endif // SVGWINDOW_H
