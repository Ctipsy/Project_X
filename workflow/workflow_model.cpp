#include "workflow/workflow_model.h"
#include "ui_cworkflow_model.h"
#include "graphviz/graphviz.h"

#include <QLabel>
#include <QProcess>
#include <QPainter>
#include <QPixmap>
#include <QPaintEvent>
#include <QDebug>
#include<Windows.h>
#include<WinNT.h>
#include<WinBase.h>

CWorkflowModel::CWorkflowModel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWorkflowModel)
{
    ui->setupUi(this);
    move(0,50);
    //设置窗口背景颜色
    setAutoFillBackground(true);
    QPalette pale1=palette();
    pale1.setColor(QPalette::Window,QColor(255,255,255));
    setPalette(pale1);
    char tmp[255];
    GetPrivateProfileStringA("image", "workflowModelImagePath", "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());
    label = new QLabel(this);
    label->setGeometry(20,20,this->width() - 40,this->height() - 40);
    label->setStyleSheet("background-color: rgb(72, 72, 72);");
    label->installEventFilter(this);
    m_renderer = new QSvgRenderer(QLatin1String("D:/Program Files (x86)/X_Ship/image/workflow.svg"));

}
bool CWorkflowModel::eventFilter(QObject *watched, QEvent *event)//将svg图片显示在label上增加的代码
{
    if(watched == label && event->type() == QEvent::Paint)
    {
        QPainter painter(label);
        m_renderer->render(&painter);
    }
    return QWidget::eventFilter(watched,event);
}
CWorkflowModel::~CWorkflowModel()
{
    delete ui;
}
void CWorkflowModel::UpdateModel()
{
    m_renderer = new QSvgRenderer(QLatin1String("D:/Program Files (x86)/X_Ship/image/workflow.svg"));
    label->setFocus();//必须加上这一句，将鼠标光标强制挪到label上，触发事件过滤器
}
