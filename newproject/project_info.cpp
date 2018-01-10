#include "project_info.h"
#include "ui_project_info.h"
#include <QDateTime>
#include "newproject/newproject.h"
#include <QDebug>
extern QString creatName;
extern QString creatPath;
extern QString creatUser;
extern int active_module;
extern QString creatTime;


project_info::project_info(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::project_info)
{
    ui->setupUi(this);
    move(190,40);
    ui->project_info_svg->installEventFilter(this);   //将svg图片显示在label上增加的代码
    m_renderer = new QSvgRenderer(QLatin1String("D:/Program Files (x86)/X_Ship/image/project_info.svg"));

    ui->lcdNumber->setDigitCount(22);
    ui->lcdNumber->setMode(QLCDNumber::Dec);
    ui->lcdNumber->setSegmentStyle(QLCDNumber::Flat);
    ui->lcd_start_time->setDigitCount(22);
    ui->lcd_start_time->setMode(QLCDNumber::Dec);
    ui->lcd_start_time->setSegmentStyle(QLCDNumber::Flat);

    ui->groupBox_2->setFocusPolicy(Qt::NoFocus);
    ui->wuli_Radio->setMouseTracking(false);

    ui->wuli_Radio->setChecked(false);
    ui->net_Radio->setChecked(false);
    ui->message_Radio->setChecked(false);
    ui->workflow_Radio->setChecked(false);
    ui->taishi_Radio->setChecked(false);

    lic = new Licence;
    timer = new QTimer(this);
//    QDateTime current_date_time = QDateTime::currentDateTime();

//    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
//    creatTime = current_date; // 2018.1.2   改由从创建项目确定 按钮按下时，记录时间，然后放到这里来显示

    ui->lcd_start_time->setStyleSheet("border: 1px solid #b1b1b1; color: white; background: #323232;");
    ui->lcd_start_time->display(creatTime);

    connect(timer, SIGNAL(timeout()), this, SLOT(Show_Time()));
    timer->start(1000);    
    ui->user->setText(creatUser);
    ui->Pro_name->setText(creatName);
    items = lic->Read_Date_Module("Module");
    ui->modules->setText(QString::number(items));
    itemsCount();

}

project_info::~project_info()
{
    delete ui;

}

void project_info::Show_Time()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
    ui->lcdNumber->setStyleSheet("border: 1px solid white; color: green; background: #323232;border-color: #2E8B57");
    ui->lcdNumber->display(current_date);
    //ui->lcdNumber->setAttribute(Qt::AlignCenter);
}

bool project_info::eventFilter(QObject *watched, QEvent *event)//将svg图片显示在label上增加的代码
{
    if(watched == ui->project_info_svg && event->type() == QEvent::Paint)
    {
        QPainter painter(ui->project_info_svg);
        m_renderer->render(&painter);
    }
    return QWidget::eventFilter(watched,event);
}

void project_info::itemsCount()
{
    int item=active_module;
    qDebug()<<"激活模块"<<items<<"用户模块"<<item;
    if(item==1)
    {
        itemsCount_2();
    }
    else if(item==2)
    {
        itemsCount_2();
    }
    else if(item==3)
    {
        itemsCount_2();
    }
    else if(item==4)
    {
        itemsCount_2();
    }
    else
    {
        itemsCount_2();
    }
    ui->wuli_Radio->setEnabled(false);
    ui->net_Radio->setEnabled(false);
    ui->message_Radio->setEnabled(false);
    ui->workflow_Radio->setEnabled(false);
    ui->taishi_Radio->setEnabled(false);
}

void project_info::itemsCount_2()
{

    if(items>active_module)
        items=active_module;  //如果激活模块比用户模块多，那么以少的那个为准
    if(items==1)
        ui->wuli_Radio->setChecked(true);
    if(items==2)
    {
        ui->wuli_Radio->setChecked(true);
        ui->net_Radio->setChecked(true);
    }
    if(items==3)
    {
        ui->wuli_Radio->setChecked(true);
        ui->message_Radio->setChecked(true);
    }
    if(items==4)
    {
        ui->wuli_Radio->setChecked(true);
        ui->net_Radio->setChecked(true);
        ui->message_Radio->setChecked(true);
    }
    if(items==5)
    {
        ui->wuli_Radio->setChecked(true);
        ui->net_Radio->setChecked(true);
        ui->message_Radio->setChecked(true);
        ui->workflow_Radio->setChecked(true);
    }
    if(items==6)
    {
         ui->wuli_Radio->setChecked(true);
         ui->net_Radio->setChecked(true);
         ui->message_Radio->setChecked(true);
         ui->workflow_Radio->setChecked(true);
         ui->taishi_Radio->setChecked(true);
    }

}

