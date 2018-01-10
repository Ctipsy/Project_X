#include "svgwindow.h"

SvgWindow::SvgWindow(QWidget *parent)
    : QScrollArea(parent)
{
    svgWidget = new SvgWidget;
    setWidget (svgWidget);       //设置滚动条
    setWindowTitle("指标体系树");  //窗口大小
    this->resize( QSize( 1000, 260));  //默认的弹出窗口的大小
    //this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    //设置关闭按钮隐藏
}

//当主窗口对文件进行了选择或修改时, 将调用setFile()函数设置新的文件
void SvgWindow::setFile (QString fileName)
{
    //将新的SVG文件加载到svgWidget中显示
    svgWidget->load (fileName);
    //返回渲染器,去显示控件的内容
    QSvgRenderer *render = svgWidget->renderer ();
    //使svgWidget窗体按SVG图片的默认尺寸进行显示
    svgWidget->resize (render->defaultSize ());

}
/***
 * 鼠标键按下时,mousePressPos和scrollBarValuesOnMousePress进行初始化,QScrollArea的horizonalScrollBar()
 * 和verticalScrollBar()函数可以分别获得svgWindow的水平滚动条和垂直滚动条
 */
void SvgWindow::mousePressEvent (QMouseEvent *event)
{
    mousePressPos = event->pos ();         //记下当前事件位置
    scrollBarValueOnMousePress.rx () = horizontalScrollBar ()->value ();
    scrollBarValueOnMousePress.ry () = verticalScrollBar ()->value ();
    //设置事件对象的接受信号,表明事件接受器希望事件，意外事件传到父部件
    event->accept ();
}

/***
 * 当鼠标按下并拖曳鼠标时, 触发mouseMoveEvent()函数,通过滑动条的位置设置实现图片图片拖曳的效果
 */
void SvgWindow::mouseMoveEvent (QMouseEvent *event)
{
    //对水平滑动条的新位置进行设置
    //新水平滑动条位置 = (滑块当前位置 - 鼠标即时返回的位置)(即:滑块移动位置) + 鼠标之前记下的事件位置
    horizontalScrollBar ()->setValue (scrollBarValueOnMousePress.x () - event->pos ().x () + mousePressPos.x ());
    //对垂直滑动条的新位置进行设置
    verticalScrollBar ()->setValue (scrollBarValueOnMousePress.y () - event->pos ().y () + mousePressPos.y ());
    horizontalScrollBar ()->update ();
    verticalScrollBar ()->update ();
    event->accept ();
}
