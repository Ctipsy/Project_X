/***
 * 实现显示SVG图片的函数所在的文件.
 */
#ifndef SVGWIDGET_H
#define SVGWIDGET_H

#include <QtSvg/QtSvg>
#include <QtSvg/QSvgWidget>
#include <QtSvg/QSvgRenderer>

class SvgWidget : public QSvgWidget
{
    Q_OBJECT
public:
    SvgWidget(QWidget *parent = 0);
    //响应鼠标的滚动事件,使SVG图片能够通过鼠标滚轮的 滚动 进行 缩放
    void wheelEvent (QWheelEvent *);
private:
    //渲染器
    QSvgRenderer *render;           //用于图片显示尺寸 的 确定
};

#endif // SVGWIDGET_H