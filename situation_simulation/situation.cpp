#include "situation.h"
#include "ui_situation.h"
//#include <QtCharts/QChartView>
//#include <QtCharts/QLineSeries>
//#include <QChart>
#include <QtCharts>

QT_CHARTS_USE_NAMESPACE

extern System_Informa* sys_inf;
QStringList workflow_device;
WorkflowCom *tai_curWorkflow;

QMap<uint,uint> m_is_fault_device;   //故障设备的标志位
QVector<virusFunctionality>  m_availability_virus;
QVector<virusConfidentiality> m_confidentiality_virus;
QSet<pair<uint,uint>> m_secu_gateway;

extern QMap<uint,uint> fault_device;
extern QSet<uint> infect_device;
extern QSet<uint> infect_info;


Situation::Situation(WFlow *m_poWFlowDlg,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Situation),out_poWFlowDlg(m_poWFlowDlg)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    QPalette pale1=palette();
    pale1.setColor(QPalette::Window,QColor(255,255,255));
    setPalette(pale1);
    move(190,40);//子窗口的偏移量	
    svgWindow = new SvgWindow;

    ui->topo_situ->installEventFilter(this);
    m_renderer = new QSvgRenderer(QLatin1String("D:/Program Files (x86)/X_Ship/image/SituDot.svg"));

    /*ui->tableWidget->verticalHeader()->setVisible(false); //隐藏行序号
    //ui->wuli_tableWidget->horizontalHeader()->setVisible(false); //隐藏列序号
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//表格均匀分布
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空*/

    device_list_signallist1=new QSignalMapper(this);//
    QObject::connect(device_list_signallist1,SIGNAL(mapped(const QString &)),this,SLOT(Set_fault_Device(const QString&)));

    device_list_signallist2=new QSignalMapper(this);
    QObject::connect(device_list_signallist2,SIGNAL(mapped(const QString &)),this,SLOT(Set_virusFunction_Device(const QString&)));

    device_list_signallist3=new QSignalMapper(this);
    QObject::connect(device_list_signallist3,SIGNAL(mapped(const QString &)),this,SLOT(Set_virusConfidentiality_Device(const QString&)));

    device_list_signallist4=new QSignalMapper(this);
    QObject::connect(device_list_signallist4,SIGNAL(mapped(const QString &)),this,SLOT(Set_gataway_Device(const QString&)));

    addworkflow();
    //lineChart();
    //barChart();
    updatepic();
}

Situation::~Situation()
{
    delete ui;
}


bool Situation::eventFilter(QObject *watched, QEvent *event)//将svg图片显示在label上增加的代码
{
    if(watched == ui->topo_situ && event->type() == QEvent::Paint)
    {
        QPainter painter(ui->topo_situ);
        m_renderer->render(&painter);
    }
    return QWidget::eventFilter(watched,event);
}

void Situation::addworkflow()
{
    QList<uint> businessNum = sys_inf->Get_BusinessId_Vector();
    for (uint i = 0; i < businessNum.size(); ++i)
    {
        Business_Informa oTemp = sys_inf->Get_SingleBusiness(businessNum[i]);
        ui->comboBox->addItem(oTemp._Get_BusinessName());      //添加QListWidget中的工作流记录
    }

}


void Situation::res_show()
{
    ui->listWidget->clear();
    QList<QString> par1;
    QList<QList<double>> par2;
    QList<double> par21,par22;
    for(QMap<uint, double>::iterator i=tai_curWorkflow->m_oDeviceDynamicAvail.begin(),j=tai_curWorkflow->m_T_oDeviceDynamicAvail.begin()
        ;i!=tai_curWorkflow->m_oDeviceDynamicAvail.end();i++,j++)
    {
        par1.push_back(sys_inf->Get_SingleDevice(i.key())._Get_DeviceName());
        par21.push_back(i.value());
        par22.push_back(j.value());
        if(j.value()==0) ui->listWidget->addItem(sys_inf->Get_SingleDevice(i.key())._Get_DeviceName());
    }
    par2.push_back(par21);
    par2.push_back(par22);
    QList<QString> par3;
    par3.push_back("正常情况下的安全状况");
    par3.push_back("添加激励后的安全状况");
    QString par4;
    par4.push_back("添加激励前后安全状况对比");
    CResultShow().BarDisplay(par1,par2,par3,par4,ui->barChart);

    /*QString line_title("业务流安全趋势图");
    CResultShow().LineChartDisplay(par1,par2[1],line_title,ui->lineChart);*/
}


void Situation::on_pb_additem1_clicked()
{
    if(m_is_fault_device[m_device_num[ui->device_list->currentIndex()]]==1)
        return;
    ui->falut_device_list->addItem(ui->device_list->currentText());
    m_is_fault_device[m_device_num[ui->device_list->currentIndex()]]=1;

    updatepic();
}

void Situation::updatepic()
{
    tai_curWorkflow->T_inital();
    ui->topo_situ->clear();
    m_renderer = new QSvgRenderer(QLatin1String("D:/Program Files (x86)/X_Ship/image/SituDot.svg"));
    res_show();
}

void Situation::on_comboBox_currentIndexChanged(int index)
{
    tai_curWorkflow=out_poWFlowDlg->m_pWorkflowList[index];
    tai_curWorkflow->GetInfoDeviceStaticAvail();
    workflow_device.clear();
    ui->device_list->clear();
    ui->falut_device_list->clear();
    m_availability_virus.clear();
    m_confidentiality_virus.clear();
    m_secu_gateway.clear();
    ui->tableWidget_2->clearContents();
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_3->setRowCount(0);
    ui->tableWidget_4->setRowCount(0);
    m_device_num.clear();
    m_is_fault_device.clear();
    for(QMap<uint, double>::iterator tem_dev=tai_curWorkflow->m_oDeviceStaticAvail.begin();
        tem_dev!=tai_curWorkflow->m_oDeviceStaticAvail.end();tem_dev++)
    {
        workflow_device.append(sys_inf->Get_SingleDevice(tem_dev.key())._Get_DeviceName());
        m_device_num.push_back(tem_dev.key());
        m_is_fault_device.insert(tem_dev.key(),0);
    }
    ui->device_list->addItems(workflow_device);
    updatepic();
}

void Situation::my_updata()
{
    //tai_curWorkflow=out_poWFlowDlg->m_pWorkflowList[index];
    tai_curWorkflow->GetInfoDeviceStaticAvail();
    updatepic();
}


void Situation::Set_fault_Device(const QString& tem_fault_device)
{

}

void Situation::Set_virusFunction_Device(const QString& tem_fault_device)
{
    m_availability_virus.clear();
    for(int row=0;row<ui->tableWidget_2->rowCount();row++)
    {
        virusFunctionality virus;
        //用于判断是否存在病毒名称、注入节点、攻击节点和攻击强度
        bool flag[4]={0};
        //存在病毒名称
        if(ui->tableWidget_2->item(row,0)!=NULL && ui->tableWidget_2->item(row,0)->text()!=tr(""))
        {
            QString temp=ui->tableWidget_2->item(row,0)->text();
            virus.virusName=temp;
            flag[0]=true;
        }
        //存在病毒注入点
        QWidget * deviceList1=ui->tableWidget_2->cellWidget(row,1);
        QComboBox *combox1=(QComboBox*)deviceList1;//强制转化为QComboBox
        if(combox1->currentIndex()!=-1)
        {
            int count=0;
            QList<uint>::const_iterator iter;
            for(iter=m_device_num.begin(); iter!=m_device_num.end() && count!=combox1->currentIndex(); ++iter)
                count++;
            virus.sourceId=*iter;
            flag[1]=true;
        }
        //存在病毒攻击目标点
        QWidget * deviceList2=ui->tableWidget_2->cellWidget(row,2);
        QComboBox *combox2=(QComboBox*)deviceList2;//强制转化为QComboBox
        if(combox2->currentIndex()!=-1)
        {
            int count=0;
            QList<uint>::const_iterator iter;
            for(iter=m_device_num.begin(); iter!=m_device_num.end() && count!=combox2->currentIndex(); ++iter)
                count++;
            virus.destination=*iter;
            flag[2]=true;
        }
        //存在病毒的攻击能力、渗透能力
        if(ui->tableWidget_2->item(row,3)!=NULL && ui->tableWidget_2->item(row,3)->text()!=tr(""))
        {
            QString temp=ui->tableWidget_2->item(row,3)->text();
            if(temp.toFloat()>0 && temp.toFloat()<1)
            {
                virus.influence=temp.toFloat();
                flag[3]=true;
            }
        }
        if(flag[0]&&flag[1]&&flag[2]&&flag[3])
            m_availability_virus.push_back(virus);
    }
    updatepic();
}

void Situation::Set_virusConfidentiality_Device(const QString& tem_fault_device)
{
    m_confidentiality_virus.clear();
    for(int row=0;row<ui->tableWidget_3->rowCount();row++)
    {
        virusConfidentiality virus;
        //用于判断是否存在病毒名称、注入节点、攻击节点和攻击强度
        bool flag[4]={0};
        //存在病毒名称
        if(ui->tableWidget_3->item(row,0)!=NULL && ui->tableWidget_3->item(row,0)->text()!=tr(""))
        {
            QString temp=ui->tableWidget_3->item(row,0)->text();
            virus.virusName=temp;
            flag[0]=true;
        }
        //存在病毒注入点
        QWidget * deviceList1=ui->tableWidget_3->cellWidget(row,1);
        QComboBox *combox1=(QComboBox*)deviceList1;//强制转化为QComboBox
        if(combox1->currentIndex()!=-1)
        {
            int count=0;
            QList<uint>::const_iterator iter;
            for(iter=m_device_num.begin(); iter!=m_device_num.end() && count!=combox1->currentIndex(); ++iter)
                count++;
            virus.sourceId=*iter;
            flag[1]=true;
        }
        //存在病毒攻击目标点
        QWidget * deviceList2=ui->tableWidget_3->cellWidget(row,2);
        QComboBox *combox2=(QComboBox*)deviceList2;//强制转化为QComboBox
        if(combox2->currentIndex()!=-1)
        {
            int count=0;
            QList<uint>::const_iterator iter;
            for(iter=m_device_num.begin(); iter!=m_device_num.end() && count!=combox2->currentIndex(); ++iter)
                count++;
            virus.destination=*iter;
            flag[2]=true;
        }
        //存在病毒的攻击能力、渗透能力
        if(ui->tableWidget_3->item(row,3)!=NULL && ui->tableWidget_3->item(row,3)->text()!=tr(""))
        {
            QString temp=ui->tableWidget_3->item(row,3)->text();
            if(temp.toFloat()>0 && temp.toFloat()<1)
            {
                virus.influence=temp.toFloat();
                flag[3]=true;
            }
        }
        if(flag[0]&&flag[1]&&flag[2]&&flag[3])
            m_confidentiality_virus.push_back(virus);
    }
    updatepic();
}
void Situation::Set_gataway_Device(const QString& tem_fault_device)
{
    m_secu_gateway.clear();
    for(int row=0;row<ui->tableWidget_4->rowCount();row++)
    {
        pair<uint,uint> gataway;
        bool flag[3]={0};
        QWidget * deviceList1=ui->tableWidget_4->cellWidget(row,0);
        QComboBox *combox1=(QComboBox*)deviceList1;//强制转化为QComboBox
        if(combox1->currentIndex()!=-1)
        {
            int count=0;
            QList<uint>::const_iterator iter;
            for(iter=m_device_num.begin(); iter!=m_device_num.end() && count!=combox1->currentIndex(); ++iter)
                count++;
            gataway.first=*iter;
            flag[0]=true;
        }
        QWidget * deviceList2=ui->tableWidget_4->cellWidget(row,1);
        QComboBox *combox2=(QComboBox*)deviceList2;//强制转化为QComboBox
        if(combox2->currentIndex()!=-1)
        {
            int count=0;
            QList<uint>::const_iterator iter;
            for(iter=m_device_num.begin(); iter!=m_device_num.end() && count!=combox2->currentIndex(); ++iter)
                count++;
            gataway.second=*iter;
            flag[1]=true;
        }
        QWidget * deviceList3=ui->tableWidget_4->cellWidget(row,2);
        QComboBox *combox3=(QComboBox*)deviceList3;//强制转化为QComboBox
        if(combox3->currentIndex()!=-1)
        {
            flag[2]=true;
        }
        if(flag[0] && flag[1] && flag[2])
        {
            if(combox3->currentIndex()==0)//是双向网关控制
            {
                m_secu_gateway.insert(gataway);
                pair<uint,uint> gataway0;
                gataway0.first=gataway.second;
                gataway0.second=gataway.first;
                m_secu_gateway.insert(gataway0);
            }
            else//单向官网控制
            {
                m_secu_gateway.insert(gataway);
            }
        }
    }
    updatepic();
}

//设备故障
void Situation::on_choose_event1_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
//影响功能安全的病毒
void Situation::on_choose_event2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
//影响机密性的病毒
void Situation::on_choose_event3_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}
//安全网关
void Situation::on_choose_event4_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void Situation::on_pb_additem2_clicked()
{
    int currentRow=ui->tableWidget_2->rowCount();
    ui->tableWidget_2->insertRow(currentRow);
    //插入攻击设备列表和目标设备列表
    for(int col=1;col<3;col++)
    {
        QComboBox* device_list=new QComboBox;
        device_list->addItems(workflow_device);
        device_list->setCurrentIndex(-1);
        ui->tableWidget_2->setCellWidget(currentRow,col,device_list);

        device_list_signallist2->setMapping(device_list,QString("%1-%2").arg(currentRow).arg(col));
        QObject::connect(device_list,SIGNAL(currentIndexChanged(int)),device_list_signallist2,SLOT(map()));
    }
    QTableWidget * item=ui->tableWidget_2;
    device_list_signallist2->setMapping(item,QString("%1-%2").arg(currentRow).arg(3));
    QObject::connect(item,SIGNAL(itemChanged(QTableWidgetItem * )),device_list_signallist2,SLOT(map()));
}

//添加影响机密性的病毒
void Situation::on_pb_additem3_clicked()
{
    int currentRow=ui->tableWidget_3->rowCount();
    ui->tableWidget_3->insertRow(currentRow);
    //插入攻击设备列表和目标设备列表
    for(int col=1;col<3;col++)
    {
        QComboBox* device_list=new QComboBox;
        device_list->addItems(workflow_device);
        device_list->setCurrentIndex(-1);
        ui->tableWidget_3->setCellWidget(currentRow,col,device_list);

        device_list_signallist3->setMapping(device_list,QString("%1-%2").arg(currentRow).arg(col));
        QObject::connect(device_list,SIGNAL(currentIndexChanged(int)),device_list_signallist3,SLOT(map()));
    }
    QTableWidget * item=ui->tableWidget_3;
    device_list_signallist3->setMapping(item,QString("%1-%2").arg(currentRow).arg(3));
    QObject::connect(item,SIGNAL(itemChanged(QTableWidgetItem * )),device_list_signallist3,SLOT(map()));
}

//添加网关
void Situation::on_pb_additem4_clicked()
{
    int currentRow=ui->tableWidget_4->rowCount();
    ui->tableWidget_4->insertRow(currentRow);
    //插入正方向设备列表和反方向设备列表
    for(int col=0;col<2;col++)
    {
        QComboBox* device_list=new QComboBox;
        device_list->addItems(workflow_device);
        device_list->setCurrentIndex(-1);
        ui->tableWidget_4->setCellWidget(currentRow,col,device_list);

        device_list_signallist4->setMapping(device_list,QString("%1-%2").arg(currentRow).arg(col));
        QObject::connect(device_list,SIGNAL(currentIndexChanged(int)),device_list_signallist4,SLOT(map()));
    }
    //插入是否双向控制QCombox
    QComboBox* bilateralControl=new QComboBox;
    bilateralControl->addItems(QStringList()<<"是"<<"否");
    bilateralControl->setCurrentIndex(-1);
    ui->tableWidget_4->setCellWidget(currentRow,2,bilateralControl);

    device_list_signallist4->setMapping(bilateralControl,QString("%1-%2").arg(currentRow).arg(2));
    QObject::connect(bilateralControl,SIGNAL(currentIndexChanged(int)),device_list_signallist4,SLOT(map()));
}

void Situation::on_radioButton_clicked(bool checked)
{

    svgWindow->setFile ("D:/Program Files (x86)/X_Ship/image/SituDot.svg");    //8.6添加

    if(checked)
        svgWindow->show();
    else
        svgWindow->close();


}
