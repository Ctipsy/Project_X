#include <QImage>
#include <QProcess>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QPaintEvent>
#include <Windows.h>
#include <WinNT.h>
#include <WinBase.h>
#include "workflow/wflow.h"
#include "ui_wflow.h"


extern System_Informa* sys_inf;

WorkflowCom *pCurrrentWorkflow = NULL;  //当前正在操作的工作流指针

WFlow::WFlow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WFlow)
{
    ui->setupUi(this);
    move(130,40);
    setAutoFillBackground(true);
    QPalette pale1=palette();
    pale1.setColor(QPalette::Window,QColor(176,224,230));   //(176,224,230)浅蓝色
    setPalette(pale1);
    m_poWorkflowAssessDlg = new CWorkflowAssess(this);
    m_poWorkflowAssessDlg->hide();
    m_poWorkflowConfAssessDlg = new CConfAssess(this);
    m_poWorkflowConfAssessDlg->hide();
    ui->WfResultBtn->setFlat(true);
    ui->WfAssessBtn->setFlat(true);
    ui->label->setStyleSheet("background-color: rgb(72, 72, 72);");
    ui->label->installEventFilter(this);
    svgWindow = new SvgWindow; //8.6添加

    InitWorkflowSet();
}

WFlow::~WFlow()
{
    delete ui;
}

void WFlow::InitWorkflowSet()
{
    QList<uint> businessNum = sys_inf->Get_BusinessId_Vector();
    for (uint i = 0; i < businessNum.size(); ++i)
    {
        Business_Informa oTemp = sys_inf->Get_SingleBusiness(businessNum[i]);
        ui->listWidget->addItem(oTemp._Get_BusinessName());      //添加QListWidget中的工作流记录

        //创建一个具体的工作流对象，并添加任务数据
        qDebug()<<"atten "<<oTemp._Get_BusinessName();
        WorkflowCom *oWorkflowCom = new WorkflowCom(oTemp._Get_BusinessName(), oTemp._Get_BusinessId());
        //QList<uint> taskNum = sys_inf->Get_TaskId_Vector();
        QList<uint> taskNum = oTemp._Get_RelationTask();
        oWorkflowCom->CreateWorkflowCom(taskNum);    //根据任务编号创建工作流记录
        oWorkflowCom->SetAvailRequirement(oTemp._Get_BusinessReqSecurity());

        m_pWorkflowList.push_back(oWorkflowCom);
    }
    for (uint i = 0; i < businessNum.size(); ++i)
    {
        on_listWidget_currentRowChanged(i);
    }
    on_listWidget_currentRowChanged(0);
    qDebug()<<"工作流列表的初始化"<<m_pWorkflowList;
}


void WFlow::Updateinforma()
{
    m_poWorkflowAssessDlg->total_reset();
    m_poWorkflowConfAssessDlg->Updateinfor();
}
//工作流可用性评估
void WFlow::on_WfAssessBtn_clicked()
{
    if (pCurrrentWorkflow == NULL)
        return;
    //pCurrrentWorkflow->GetInfoDeviceStaticAvail();
    //m_poWorkflowAssessDlg->UpdateTable();
    //pCurrrentWorkflow->AssessWorkflowCom();

    m_poWorkflowConfAssessDlg->hide();
    ui->label->close();
    ui->verticalLayout_4->removeItem(ui->verticalSpacer_2);
    ui->verticalLayout_4->addWidget(m_poWorkflowAssessDlg);
    m_poWorkflowAssessDlg->show();
}



void WFlow::on_WfResultBtn_clicked()
{

    if (pCurrrentWorkflow == NULL)
        return;
    m_poWorkflowAssessDlg->hide();
    ui->label->close();
    ui->verticalLayout_4->removeItem(ui->verticalSpacer_2);
    ui->verticalLayout_4->addWidget(m_poWorkflowConfAssessDlg);
    m_poWorkflowConfAssessDlg->show();
}



void WFlow::on_listWidget_currentRowChanged(int currentRow)
{
    qDebug()<<"是不是所有的工作流"<<m_pWorkflowList;
    if (currentRow != -1)
    {
        pCurrrentWorkflow = m_pWorkflowList[currentRow];
    }
    pCurrrentWorkflow->GraphViz();
    char tmp[255];
    GetPrivateProfileStringA("projectInfo", "projectAddr", "", tmp, sizeof(tmp),INI_PATH.toStdString().c_str());

    QProcess p;
    p.start("cmd.exe", QStringList() << "/c" << "D:\\Program Files (x86)\\X_Ship\\generateJpgBatch.bat");
    if (p.waitForStarted())
    {
       p.waitForFinished();
       qDebug() << p.readAllStandardOutput();
    }
    else
       qDebug() << "Failed to start";

    UpdateModel();

    ui->label->show();

    m_poWorkflowAssessDlg->total_reset();

    m_poWorkflowConfAssessDlg->Updateinfor();

    m_poWorkflowAssessDlg->hide();
    m_poWorkflowConfAssessDlg->hide();    //替换为保密性评估界面
}


bool WFlow::eventFilter(QObject *watched, QEvent *event)//将svg图片显示在label上增加的代码
{
    if(watched == ui->label && event->type() == QEvent::Paint)
    {
        QPainter painter(ui->label);
        m_renderer->render(&painter);
    }
    return QWidget::eventFilter(watched,event);
}

void WFlow::UpdateModel()
{
    char tmp[255];
    GetPrivateProfileStringA("image", "workflowModelImagePath", "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());
    m_renderer = new QSvgRenderer(QLatin1String("D:/Program Files (x86)/X_Ship/image/workflow.svg"));
    ui->label->setFocus();//必须加上这一句，将鼠标光标强制挪到label上，触发事件过滤器
}

void WFlow::on_radioButton_clicked(bool checked)
{
    svgWindow->setFile ("D:/Program Files (x86)/X_Ship/image/workflow.svg");    //8.6添加
    if(checked)
        svgWindow->show();
    else
        svgWindow->close();


}
