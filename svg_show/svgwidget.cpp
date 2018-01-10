#include "svgwidget.h"

SvgWidget::SvgWidget(QWidget *parent)
    : QSvgWidget(parent)
{
    //获得本窗体的QSvgRenderer对象
    render = renderer ();
}

//滚轮响应事件，使SVG图片能够鼠标滚动滑轮进行缩放
void SvgWidget::wheelEvent (QWheelEvent *e)
{
    //diff的值表示每次滚轮滑动一定的值,图片大小改变的比例
    const double diff = 0.1;
    /***
     * 下面三行代码: 用于获取图片显示区的尺寸大小,以便进行下一步的缩放操作
     */
    QSize size = render->defaultSize ();
    int width = size.width ();
    int height = size.height ();
    /***
     * 利用QWheelEvent的delta()函数获得滚轮的距离值,通过此值来判断滚轮滚动的方向
     * delta() > 0 , 则表示滚轮向前(远离用户方向); delta() < 0, 则表示向后滚动
     * (Ps: 鼠标滚动事件，滚轮每滚动1°, 相当于移动8°;而常见的滚轮鼠标拨动一下滚动角度为15°
     * 因此，滚轮拨动一下相当于移动了 120(=15*8)
     */
    if (e->delta () > 0)
    {
        //对图片的长, 宽值进行处理, 放大一定的比例
        width = int(this->width () + this->width ()*diff);
        height = int(this->height () + this->height ()*diff);
    }
    else
    {
        //对图片的长,宽进行处理,缩小一定的比例
        width = int(this->width () - this->width ()*diff);
        height = int(this->height () - this->height ()*diff);
    }
    //利用新的长,宽值对图片进行resize()操作
    resize (width, height);

}