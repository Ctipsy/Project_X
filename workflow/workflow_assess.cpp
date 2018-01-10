#include "workflow/workflow_assess.h"
#include "ui_workflow_assess.h"
#include <QLabel>
#include <QProcess>
#include <QThread>
#include <QDateTime>
#include <QStyleOptionProgressBar>
#include <QPainter>
#include <QSplashScreen>
#include <QGridLayout>
#include <QTableWidget>
#include <QDebug>


extern System_Informa* sys_inf;
extern WorkflowCom *pCurrrentWorkflow;


CWorkflowAssess::CWorkflowAssess(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWorkflowAssess)
{
    ui->setupUi(this);
    move(0,50);

    setAutoFillBackground(true);
    QPalette pale1=palette();
    pale1.setColor(QPalette::Window,QColor(255,255,255));
    setPalette(pale1);

    ui->stackedWidget->setCurrentIndex(0);
    ui->algorithmCcomboBox->insertItem(0, "布尔代数评估算法");
    ui->algorithmCcomboBox->insertItem(1, "贝叶斯评估算法");
    //ui->algorithmCcomboBox->insertItem(ui->algorithmCcomboBox->count(), "");
    ui->algorithmCcomboBox->setCurrentIndex(0);


    ui->deviceAvailTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->deviceAvailTableWidget->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空

    ui->infoAvailTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->infoAvailTableWidget->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空

    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_2->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空

    ui->taskResultTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->taskResultTableWidget->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空




    BoolAlgebraAssessInit();  //布尔代数评估所需要的数据
    BayesAssessInit();     //贝叶斯评估所需要的可用性数据

    m_poThread = new WorkThread();

    connect(m_poThread,SIGNAL(notify(int)),this,SLOT(OnNotify(int)));
}

void CWorkflowAssess::BoolAlgebraAssessInit()
{
    //ui->deviceAvailTableWidget->setColumnWidth(0, 75);
    //ui->deviceAvailTableWidget->setColumnWidth(1, 75);
    //ui->deviceAvailTableWidget->setColumnWidth(2, 85);
    //ui->deviceAvailTableWidget->verticalHeader()->setVisible(false);//隐藏左边垂直
    //ui->deviceAvailTableWidget->resizeColumnsToContents();


    //ui->infoAvailTableWidget->setColumnWidth(0, 75);
    //ui->infoAvailTableWidget->setColumnWidth(1, 75);
    //ui->infoAvailTableWidget->setColumnWidth(2, 85);
    //ui->infoAvailTableWidget->verticalHeader()->setVisible(false);//隐藏左边垂直

    BoolAlgebraAssessWidgetShow(false);    //隐藏布尔代数评估所需要的可用性数据
}

void CWorkflowAssess::BayesAssessInit()
{
    m_pBayesTable = new QTableWidget(this);
    m_pBayesTable->setGeometry(20, 40, 400, 200);
    BayesAssessWidgetShow(false);
}

CWorkflowAssess::~CWorkflowAssess()
{
    delete ui;
}

void CWorkflowAssess::MySleep(unsigned long ulMs)
{
    QDateTime last = QDateTime::currentDateTime();
    QDateTime now;
    while (1)
    {
        now = QDateTime::currentDateTime();
        if (last.msecsTo(now) >= ulMs)
        {
            break;
        }
    }
}


void CWorkflowAssess::UpdateTable()     //刷新静态输入表格
{
    pCurrrentWorkflow->GetInfoDeviceStaticAvail();
    while (ui->deviceAvailTableWidget->rowCount())
    {
        ui->deviceAvailTableWidget->removeRow(0);
    }
    for (QMap<uint, double>::iterator iter = pCurrrentWorkflow->m_oDeviceStaticAvail.begin(); iter != pCurrrentWorkflow->m_oDeviceStaticAvail.end(); ++iter)
    {
        if (iter.key() == -1)
            continue;

        int currentRow = ui->deviceAvailTableWidget->rowCount();
        ui->deviceAvailTableWidget->insertRow(currentRow);
        ui->deviceAvailTableWidget->setItem(currentRow, 0, new  QTableWidgetItem(QString::number(iter.key())));
        ui->deviceAvailTableWidget->setItem(currentRow, 1, new QTableWidgetItem(sys_inf->Get_SingleDevice(iter.key())._Get_DeviceName()));
        ui->deviceAvailTableWidget->setItem(currentRow, 2, new QTableWidgetItem(QString::number(iter.value())));
    }

    while (ui->infoAvailTableWidget->rowCount())
    {
        ui->infoAvailTableWidget->removeRow(0);
    }
    for (QMap<uint, double>::iterator iter = pCurrrentWorkflow->m_oInfoStaticAvail.begin(); iter != pCurrrentWorkflow->m_oInfoStaticAvail.end(); ++iter)
    {
        if (iter.key() == -1)
            continue;

        int currentRow = ui->infoAvailTableWidget->rowCount();
        ui->infoAvailTableWidget->insertRow(currentRow);
        ui->infoAvailTableWidget->setItem(currentRow, 0, new  QTableWidgetItem(QString::number(iter.key())));
        ui->infoAvailTableWidget->setItem(currentRow, 1, new QTableWidgetItem(sys_inf->Get_SingleMessage(iter.key())._Get_MessageName()));
        ui->infoAvailTableWidget->setItem(currentRow, 2, new QTableWidgetItem(QString::number(iter.value())));
    }
}

void CWorkflowAssess::BoolAlgebraAssessWidgetShow(bool flag)   //修改评估算法对界面的影响
{
    if (flag)
    {
        ui->deviceAvailTableWidget->show();
        ui->infoAvailTableWidget->show();
        ui->label->show();
        ui->label_2->show();
    }
    else
    {
        ui->deviceAvailTableWidget->hide();
        ui->infoAvailTableWidget->hide();
        ui->label->hide();
        ui->label_2->hide();
    }
}

void CWorkflowAssess::BayesAssessWidgetShow(bool flag)
{
    if (m_pBayesTable)
    {
        if (flag)
        {
            m_pBayesTable->show();
        }
        else
        {
            m_pBayesTable->hide();
        }
    }
}

void CWorkflowAssess::on_algorithmCcomboBox_currentTextChanged(const QString &arg1)  //改变评估算法
{
    if (arg1 == "布尔代数评估算法")
    {

        //BayesAssessWidgetShow(false);
        if (pCurrrentWorkflow)
        {
            UpdateTable();
            BoolAlgebraAssessWidgetShow(true);
        }
        else
            qDebug() << "pCurrrentWorkflow is null" << endl;
    }
    else if (arg1 == "贝叶斯评估算法")
    {
        //BoolAlgebraAssessWidgetShow(false);
        //BayesAssessWidgetShow(true);

    }
}

void CWorkflowAssess::UpdateDeviceResultTable()   //更新设备评估结果表（动态安全）
{
    while (ui->tableWidget_2->rowCount())
    {
        ui->tableWidget_2->removeRow(0);
    }
    for (QMap<uint, double>::iterator iter = pCurrrentWorkflow->m_oDeviceDynamicAvail.begin();
         iter != pCurrrentWorkflow->m_oDeviceDynamicAvail.end(); ++iter)
    {
        int currentRow = ui->tableWidget_2->rowCount();
        ui->tableWidget_2->insertRow(currentRow);
        ui->tableWidget_2->setItem(currentRow, 0, new QTableWidgetItem(QString::number(currentRow + 1)));
        ui->tableWidget_2->setItem(currentRow, 1, new  QTableWidgetItem(sys_inf->Get_SingleDevice(iter.key())._Get_DeviceName()));
        ui->tableWidget_2->setItem(currentRow, 2, new QTableWidgetItem(QString::number(iter.value())));
    }
}

void CWorkflowAssess::ShowDeviceResult()     //以图形的方式显示设备评估结果
{
    QString sr = ui->lineEdit->text();
    double dVal = sr.toDouble();
    if (sr.isEmpty())
        dVal = 0.8;
    ui->lineEdit->setText(QString::number(dVal));

    int nNum = 0;
    for (QMap<uint, double>::iterator iter = pCurrrentWorkflow->m_oDeviceDynamicAvail.begin();
         iter != pCurrrentWorkflow->m_oDeviceDynamicAvail.end(); ++iter)
    {
        if (iter.value() > dVal)
        {
            nNum++;
        }
    }
    int nTotal = pCurrrentWorkflow->m_oDeviceDynamicAvail.size();

    QList<QString> strKeyName;
    strKeyName.push_back("合格");
    strKeyName.push_back("不合格");

    QList<double> dValue;
    dValue.push_back(nNum);
    dValue.push_back(nTotal - nNum);

    m_pResultShow->PieChartDisplay(strKeyName, dValue, "设备动态可用性结果", ui->deviceGraphicsView);

    //ui->lineEdit_2->setText(QString::number(100.0 * nNum/nTotal) + "%");
}

void CWorkflowAssess::on_assessResultBtn_clicked()   //刷新设备评估结果页面
{
    ui->resultStackedWidget->setCurrentIndex(0);
    //UpdateDeviceResultTable();
    //ShowDeviceResult();
}

void CWorkflowAssess::UpdateTaskResultTable()
{
    while (ui->taskResultTableWidget->rowCount())
    {
        ui->taskResultTableWidget->removeRow(0);
    }

     for (QMap<uint, double>::iterator iter = pCurrrentWorkflow->m_oTaskAvail.begin();
          iter != pCurrrentWorkflow->m_oTaskAvail.end(); ++iter)
     {
         int currentRow = ui->taskResultTableWidget->rowCount();
         ui->taskResultTableWidget->insertRow(currentRow);
         ui->taskResultTableWidget->setItem(currentRow, 0, new QTableWidgetItem(QString::number(currentRow + 1)));
         ui->taskResultTableWidget->setItem(currentRow, 1, new  QTableWidgetItem(pCurrrentWorkflow->m_oWorkflowTask[iter.key()].GetTaskName()));
         ui->taskResultTableWidget->setItem(currentRow, 2, new QTableWidgetItem(QString::number(iter.value())));
     }
}

void CWorkflowAssess::ShowTaskResult()
{
    QList<QString> taskName;
    QList<double> taskAvail;

    for (QMap<uint, double>::iterator iter = pCurrrentWorkflow->m_oTaskAvail.begin();
         iter != pCurrrentWorkflow->m_oTaskAvail.end(); ++iter)
    {
        taskName.push_back(pCurrrentWorkflow->m_oWorkflowTask[iter.key()].GetTaskName());
        taskAvail.push_back(iter.value());
    }

    m_pResultShow->BarDisplay(taskName, taskAvail, "任务可用性评估结果", ui->taskResultGraphicsView);

}

void CWorkflowAssess::on_statisticalAnalysisBtn_clicked()  //任务评估结果页面
{
    ui->resultStackedWidget->setCurrentIndex(1);

    UpdateTaskResultTable();

    ShowTaskResult();
}

void CWorkflowAssess::ShowWorkflowResult()
{
    pair<QString, pair<double, double>> workflowAvail = pCurrrentWorkflow->GetWorkflowAvail();
    //qDebug()<<"end:"<<workflowAvail.first<<" "<<workflowAvail.second<<endl;
    //m_lstWorkflowAvail.push_back(workflowAvail);

    if(m_lstWorkflowAvail.find(workflowAvail.first)==m_lstWorkflowAvail.end())
        m_lstWorkflowAvail.insert(workflowAvail.first,workflowAvail.second);
    else m_lstWorkflowAvail[workflowAvail.first]=workflowAvail.second;
    QList<QString> workflowName;
    QList<double> workflowReqAvail;
    QList<double> workflowActualAvail;
    int nNumMeetReq = 0;
    int nTotalWorkflow = 0;

    for (auto i = m_lstWorkflowAvail.begin(); i != m_lstWorkflowAvail.end(); ++i)
    {
        if (i.value().second != -1)
        {
            ++nTotalWorkflow;
            workflowName.push_back(i.key());
            workflowReqAvail.push_back(i.value().first);  //可用性需求
            workflowActualAvail.push_back(i.value().second);  //可用性评估结果
            if (i.value().first <= i.value().second)
            {
                ++nNumMeetReq;
            }
        }
    }

    QList<QList<double>> barData;
    barData.push_back(workflowReqAvail);
    barData.push_back(workflowActualAvail);

    QList<QString> lstStrGroupName;
    lstStrGroupName.push_back("工作流可用性需求");
    lstStrGroupName.push_back("工作流评估结果");

    m_pResultShow->BarDisplay(workflowName, barData, lstStrGroupName,
                              "工作流可用性需求与评估结果对比图", ui->workflowResultGraphicsView);

    QList<QString> lstStrPieChartRecordName;
    lstStrPieChartRecordName.push_back("满足需求");
    lstStrPieChartRecordName.push_back("不满足需求");

    QList<double> lstDPieChartRecordData;
    lstDPieChartRecordData.push_back(nNumMeetReq);
    lstDPieChartRecordData.push_back(nTotalWorkflow - nNumMeetReq);

    m_pResultShow->PieChartDisplay(lstStrPieChartRecordName, lstDPieChartRecordData,
                                   "工作流可用性需求符合性分析图", ui->pieChartGraphicsView);

}


void CWorkflowAssess::on_multiWorkflowCompareBtn_clicked()
{
    ui->resultStackedWidget->setCurrentIndex(2);
    //ShowWorkflowResult();
}

void CWorkflowAssess::on_AvailResultBtn_clicked()  //进行评估结果界面
{
    pCurrrentWorkflow->GetInfoDeviceStaticAvail();
    pCurrrentWorkflow->AssessWorkflowCom();
    UpdateDeviceResultTable();
    ShowDeviceResult();
    UpdateTaskResultTable();
    ShowTaskResult();
    ShowWorkflowResult();
    total_reset();


    ui->stackedWidget->setCurrentIndex(1);
}

void CWorkflowAssess::on_returnBtn_clicked()  //返回输入参数界面
{
    ui->stackedWidget->setCurrentIndex(0);
}


void CWorkflowAssess::total_reset()
{
    on_algorithmCcomboBox_currentTextChanged(ui->algorithmCcomboBox->currentText());
    pCurrrentWorkflow->AssessWorkflowCom();
    UpdateDeviceResultTable();
    ShowDeviceResult();
    UpdateTaskResultTable();
    ShowTaskResult();
    ShowWorkflowResult();
    ui->stackedWidget->setCurrentIndex(0);
    ui->resultStackedWidget->setCurrentIndex(0);
}

void CWorkflowAssess::on_lineEdit_textChanged(const QString &arg1)
{
    pCurrrentWorkflow->SetAvailRequirement(arg1.toFloat());
    ShowWorkflowResult();
}
