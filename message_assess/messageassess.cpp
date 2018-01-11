#include "messageassess.h"
#include "ui_messageassess.h"
#include <QSqlDatabase>
#include "questionnaire_analysis/questionnaire_analysis.h"
/*#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif*/

extern System_Informa* sys_inf;
QList<uint> devicegroup;
QList<uint> messagegroup;
QList<uint> channelgroup;
bool bflag1=1,bflag2=1;
QStringList protocal=QStringList()<<"CIP"<<"MODBUS"<<"DNP3"<<"PROFIBUS"<<"PROFINET"<<"POWERLINK ETHERNET"<<"OPC"<<"ETHERCAT";
QStringList message_type=QStringList()<<"舱室环境类传感"<<"航向类传感"<<"对敌探测类传感"<<"操控类数据"<<"报警类数据"<<"日志类数据"<<"娱乐类数据";
QList<uint> message_index={2,3,4};
extern QMap<QString,uint> subsystem;
extern QMap<QString,uint> domain;
Index* m_message_index;
const QString m_path = "D:/Program Files (x86)/X_Ship/DB_51aspx/message_assess_KnowledgeBase";


messageassess::messageassess(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::messageassess),m_Protocal_Text(Get_ProtocalText())
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    QPalette pale1=palette();
    pale1.setColor(QPalette::Window,QColor(255,255,255));
    setPalette(pale1);
    move(190,40);//子窗口的偏移量
    setAutoFillBackground(true);
    svgWindow = new SvgWindow;
    m_message_index=new Index("信息流指标树");

    devicegroup=sys_inf->Get_DeviceId_Vector();
    messagegroup=sys_inf->Get_MessageId_Vector();
    channelgroup=sys_inf->Get_ChannelId_Vector();
    set_protocal();
    QObject::connect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(Set_Modified_Informa(QTableWidgetItem *)));
    QObject::connect(ui->ChannelWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(Set_Modified_Channel(QTableWidgetItem*)));
    Set_ModelConstruct();
    Set_Result_Initial();
    move(190,40);
    ui->topo_visual->installEventFilter(this);
    m_renderer = new QSvgRenderer(QLatin1String("D:/Program Files (x86)/X_Ship/image/info.svg"));
    ui->tableWidget_2->verticalHeader()->setVisible(false); //隐藏行序号
    ui->tableWidget_2->horizontalHeader()->setVisible(false); //隐藏列序号
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_2->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空

    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //x先自适应宽度
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列

    ui->ChannelWidget->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空
    ui->ChannelWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->ChannelWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //x先自适应宽度
    ui->ChannelWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列


}

messageassess::~messageassess()
{
    delete ui;
}

/*           *****************************************                    */
/*           *************************信息流拓扑可视化***                    */
/*           *****************************************                    */


bool messageassess::eventFilter(QObject *watched, QEvent *event)//将svg图片显示在label上增加的代码
{
    if(watched == ui->topo_visual && event->type() == QEvent::Paint)
    {
        QPainter painter(ui->topo_visual);
        m_renderer->render(&painter);
    }
    return QWidget::eventFilter(watched,event);
}
void messageassess::Set_ModelConstruct()
{
    Display_Message1();
    //label = new QLabel(this);
    //label->setGeometry(20,20,this->width() - 40,this->height() - 40);
    //调整图片以适应窗口大小
}

QString chooseacolor(uint message_type)
{
    switch(message_type)
    {
    case 1:
        return "brown";
    case 2:
        return "lightblue";
    case 3:
        return "blue";
    case 4:
        return "skyblue";
    case 5:
        return "yellow";
    case 6:
        return "red";
    }
}
void messageassess::Display_Message1()
{
    QHash<pair<uint,uint>,pair<QString,QString>> channel;
    for(uint i_temp=0;i_temp!=messagegroup.size();i_temp++)
    {
        Message_Informa temp_message=sys_inf->Get_SingleMessage(messagegroup[i_temp]);
        uint temp_SendDevice=temp_message._Get_MessageSendDevice();
        uint temp_ReceiveDevice=temp_message._Get_MessageReceiveDevice();
        if(channel.find(pair<uint,uint>(temp_SendDevice,temp_ReceiveDevice))!=channel.end())
        {
            channel[pair<uint,uint>(temp_SendDevice,temp_ReceiveDevice)].first+="I";
            channel[pair<uint,uint>(temp_SendDevice,temp_ReceiveDevice)].first+=QString::number(temp_message._Get_Messageid());
            channel[pair<uint,uint>(temp_SendDevice,temp_ReceiveDevice)].first+=".";
        }
        else if(channel.find(pair<uint,uint>(temp_ReceiveDevice,temp_SendDevice))!=channel.end())
        {
            channel[pair<uint,uint>(temp_ReceiveDevice,temp_SendDevice)].second+="I";
            channel[pair<uint,uint>(temp_ReceiveDevice,temp_SendDevice)].second+=QString::number(temp_message._Get_Messageid());
            channel[pair<uint,uint>(temp_ReceiveDevice,temp_SendDevice)].second+=".";
        }
        else
        {
            pair<QString,QString> curr_message;
            curr_message.first+="I";
            curr_message.first+=QString::number(temp_message._Get_Messageid());
            curr_message.first+=".";
            channel.insert(pair<uint,uint>(temp_SendDevice,temp_ReceiveDevice),curr_message);
        }
    }
    QList<link_info> fin_channel;
    for(auto i=channel.begin();i!=channel.end();i++)
    {
       link_info temp_link;
       temp_link.link=i.key();
       temp_link.link_message=i.value();
       temp_link.style=make_pair<QString,uint>("black",1);
       fin_channel.push_back(temp_link);
    }
    CInfoInternetVizObj *a=new CInfoInternetVizObj();
    a->InfoGraphvizWithProperty(fin_channel,0);
    delete a;
}

void messageassess::Display_Message2()
{
    QList<link_info> channel;
    QHash<pair<uint,uint>,pair<float,float>> flow;
    for(uint i_temp=0;i_temp!=messagegroup.size();i_temp++)
    {
        Message_Informa temp_message=sys_inf->Get_SingleMessage(messagegroup[i_temp]);
        uint temp_SendDevice=temp_message._Get_MessageSendDevice();
        uint temp_ReceiveDevice=temp_message._Get_MessageReceiveDevice();
        float temp_flow=temp_message._Get_MessageRate()*temp_message._Get_MessageSize();
        if(flow.find(pair<uint,uint>(temp_SendDevice,temp_ReceiveDevice))!=flow.end())
        {
            flow[pair<uint,uint>(temp_SendDevice,temp_ReceiveDevice)].first+=temp_flow;
        }
        else if(flow.find(pair<uint,uint>(temp_ReceiveDevice,temp_SendDevice))!=flow.end())
        {
            flow[pair<uint,uint>(temp_ReceiveDevice,temp_SendDevice)].first+=temp_flow;
        }
        else
        {
            flow.insert(pair<uint,uint>(temp_SendDevice,temp_ReceiveDevice),pair<float,float>(temp_flow,
                                                                                              sys_inf->Get_ChannelCapacity(temp_ReceiveDevice,temp_SendDevice)));
        }
    }
    for(auto tem=flow.begin();tem!=flow.end();tem++)
    {
        link_info temp_link;
        temp_link.link=tem.key();
        temp_link.link_message=make_pair<QString,QString>(QString::number(tem.value().first),QString::number(tem.value().second));
        temp_link.style=make_pair<QString,uint>("black",tem.value().second/tem.value().first);
        channel.push_back(temp_link);
    }
    CInfoInternetVizObj *a=new CInfoInternetVizObj();
    a->InfoGraphvizWithProperty(channel,0);
    delete a;
}

void messageassess::Display_Message3()
{
    QList<link_info> channel;
    for(uint i_temp=0;i_temp!=messagegroup.size();i_temp++)
    {
        link_info temp;
        Message_Informa temp_message=sys_inf->Get_SingleMessage(messagegroup[i_temp]);
        uint temp_SendDevice=temp_message._Get_MessageSendDevice();
        uint temp_ReceiveDevice=temp_message._Get_MessageReceiveDevice();
        pair<QString,QString> current_inter;
        pair<QString,uint> current_style;
        current_inter.first+="I";
        current_inter.first+=QString::number(temp_message._Get_Messageid());
        current_inter.first+=" (";
        current_inter.first+=QString::number(temp_message._Get_MessageSize());
        current_inter.first+=",";
        current_inter.first+=QString::number(temp_message._Get_MessageRate());
        current_inter.first+=") ";
        current_inter.second+=QString::number(sys_inf->Get_ChannelCapacity(temp_SendDevice,temp_ReceiveDevice));
        current_style.first=chooseacolor(temp_message._Get_MessageType());
        current_style.second=1;
        temp.link=make_pair(temp_SendDevice,temp_ReceiveDevice);
        temp.link_message=current_inter;
        temp.style=current_style;
        channel.push_back(temp);
    }
    CInfoInternetVizObj *a=new CInfoInternetVizObj();
    a->InfoGraphvizWithProperty(channel,1);
    delete a;
}
/*           *********************************************                    */
/*           ************************评估对象编辑***********                    */
/*           *********************************************                    */
void messageassess::set_protocal()
{
    Initial_Read_Receive_Device();
    Set_ChannelTable(ui->Csend_device->currentIndex(),ui->Creceive_device->currentIndex());
    Set_MessageTable(ui->Csend_device->currentIndex(),ui->Creceive_device->currentIndex());
    init_total_message();
    set_message_index();
}

void messageassess::set_message_index()
{
    m_message_index->RebuildEvaluationTree(ui->infor_index);
    ui->infor_index->expandAll();
}

void messageassess::Initial_Read_Receive_Device()
{
    //设置发送设备、接收设备的下拉菜单
    //setTextAlignment(Qt::AlignHCenter,Qt::AlignVCenter);
    ui->keshihua->setCurrentIndex(2);
    ui->Csend_device->addItem(QString("不限"));
    ui->Creceive_device->addItem(QString("不限"));
    for(int i=0;i!=devicegroup.size();i++)
    {
        ui->Csend_device->addItem(sys_inf->Get_SingleDevice(devicegroup[i])._Get_DeviceName());
        ui->Creceive_device->addItem(sys_inf->Get_SingleDevice(devicegroup[i])._Get_DeviceName());
    }

    ui->Csubsystem->addItem("不限");
    for(auto i=subsystem.begin();i!=subsystem.end();i++)
    {
        ui->Csubsystem->addItem(i.key());
    }
    ui->Cdomain->addItem("不限");
    for(auto i=domain.begin();i!=domain.end();i++)
    {
        ui->Cdomain->addItem(i.key());
    }
}

bool check_channel(int i,int j,int i_temp,int j_temp)
{
    if(i==-1&&j==-1) return 1;
    else if(i==-1)
    {
        if(i_temp==j||j_temp==j) return 1;
    }
    else if(j==-1)
    {
        if(i_temp==i||j_temp==i) return 1;
    }
    else if((i==i_temp&&j==j_temp)||(i==j_temp&&j==i_temp))
        return 1;
    return 0;
}
void messageassess::Set_ChannelTable(int i,int j)
{
    bflag2=0;
    ui->ChannelWidget->clearContents();
    ui->ChannelWidget->setRowCount(0);
    QSignalMapper *channel_signallist=new QSignalMapper(this);
    if(devicegroup.size()==0) return;        //防止构件在初始化时出现错误
    for(int i_t=0;i_t!=channelgroup.size();i_t++)
    {
        Channel_Informa *CurrentChannel=&(sys_inf->Get_SingleChannel(channelgroup[i_t]));
        uint i_temp=CurrentChannel->_Get_Channel().first-1;
        uint j_temp=CurrentChannel->_Get_Channel().second-1;
        if(check_channel(i-1,j-1,i_temp,j_temp)==1&&(ui->Csubsystem->currentIndex()==0||(sys_inf->Get_SingleDevice(i_temp+1)._Get_SystemType()==ui->Csubsystem->currentIndex()
                &&sys_inf->Get_SingleDevice(j_temp+1)._Get_SystemType()==ui->Csubsystem->currentIndex()))
                &&(ui->Cdomain->currentIndex()==0||(sys_inf->Get_SingleDevice(i_temp+1)._Get_DomainType()==ui->Cdomain->currentIndex()
                &&sys_inf->Get_SingleDevice(j_temp+1)._Get_DomainType()==ui->Cdomain->currentIndex())))
        {
            int currentRow=ui->ChannelWidget->rowCount();
            ui->ChannelWidget->insertRow(currentRow);

            ui->ChannelWidget->setItem(currentRow,0,new QTableWidgetItem(QString::number(CurrentChannel->_Get_ChannelId())));
            ui->ChannelWidget->item(currentRow,0)->setFlags(ui->ChannelWidget->item(currentRow,0)->flags() & (~Qt::ItemIsEditable));

            ui->ChannelWidget->setItem(currentRow,1,new QTableWidgetItem(sys_inf->Get_SingleDevice(CurrentChannel->_Get_Channel().first)._Get_DeviceName()));
            ui->ChannelWidget->item(currentRow,1)->setFlags(ui->ChannelWidget->item(currentRow,1)->flags() & (~Qt::ItemIsEditable));

            ui->ChannelWidget->setItem(currentRow,2,new QTableWidgetItem(sys_inf->Get_SingleDevice(CurrentChannel->_Get_Channel().second)._Get_DeviceName()));
            ui->ChannelWidget->item(currentRow,2)->setFlags(ui->ChannelWidget->item(currentRow,2)->flags() & (~Qt::ItemIsEditable));

            QComboBox* first=new QComboBox;
            first->addItems(protocal);
            first->setCurrentIndex(CurrentChannel->_Get_ChannelProtocal()-1);
            ui->ChannelWidget->setCellWidget(currentRow,3,first);
            channel_signallist->setMapping(first,QString("%1-%2").arg(currentRow).arg(3));
            QObject::connect(first,SIGNAL(currentIndexChanged(int)),channel_signallist,SLOT(map()));

            QComboBox* second=new QComboBox();
            second->addItems(QStringList()<<"光纤"<<"同轴电缆"<<"双绞线"<<"无线");
            second->setCurrentIndex(CurrentChannel->_Get_ChannelMedia()-1);
            ui->ChannelWidget->setCellWidget(currentRow,4,second);
            channel_signallist->setMapping(second,QString("%1-%2").arg(currentRow).arg(4));
            QObject::connect(second,SIGNAL(currentIndexChanged(int)),channel_signallist,SLOT(map()));

            ui->ChannelWidget->setItem(currentRow,5,new QTableWidgetItem(QString::number(CurrentChannel->_Get_ChannelCapacity())));
        }
    }
    QObject::connect(channel_signallist,SIGNAL(mapped(const QString &)),this,SLOT(Set_Modified_ChannelType(const QString&)));
    bflag2=1;
}
void messageassess::Set_MessageTable(int i,int j)
{
    bflag1=0;
    //设置表格的信息
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    //qSort(messagegroup.begin(),messagegroup.end());
    QSignalMapper *message_type_signallist=new QSignalMapper(this);
    for(int i_temp=0;i_temp!=messagegroup.size();i_temp++)
    {
        if((i<=0||devicegroup[i-1]==sys_inf->Get_SingleMessage(messagegroup[i_temp])._Get_MessageSendDevice())&&
                (j<=0||devicegroup[j-1]==sys_inf->Get_SingleMessage(messagegroup[i_temp])._Get_MessageReceiveDevice()))
        {
            uint send_device=sys_inf->Get_SingleMessage(messagegroup[i_temp])._Get_MessageSendDevice();
            uint rece_device=sys_inf->Get_SingleMessage(messagegroup[i_temp])._Get_MessageReceiveDevice();
            if((ui->Csubsystem->currentIndex()==0||(sys_inf->Get_SingleDevice(send_device)._Get_SystemType()==ui->Csubsystem->currentIndex()
                            &&sys_inf->Get_SingleDevice(rece_device)._Get_SystemType()==ui->Csubsystem->currentIndex()))
                            &&(ui->Cdomain->currentIndex()==0||(sys_inf->Get_SingleDevice(send_device)._Get_DomainType()==ui->Cdomain->currentIndex()
                            &&sys_inf->Get_SingleDevice(rece_device)._Get_DomainType()==ui->Cdomain->currentIndex())))
            {
                int currentRow=ui->tableWidget->rowCount();
                ui->tableWidget->insertRow(currentRow);
                ui->tableWidget->setItem(currentRow,0,new QTableWidgetItem(QString::number(messagegroup[i_temp])));
                ui->tableWidget->item(currentRow,0)->setFlags(ui->tableWidget->item(currentRow,0)->flags() & (~Qt::ItemIsEditable));
                ui->tableWidget->setItem(currentRow,1,new QTableWidgetItem(sys_inf->Get_SingleMessage(messagegroup[i_temp])._Get_MessageName()));
                ui->tableWidget->item(currentRow,1)->setFlags(ui->tableWidget->item(currentRow,1)->flags() & (~Qt::ItemIsEditable));

                QComboBox* messagetype=new QComboBox;
                messagetype->addItems(message_type);
                messagetype->setCurrentIndex(sys_inf->Get_SingleMessage(messagegroup[i_temp])._Get_MessageType()-1);
                ui->tableWidget->setCellWidget(currentRow,2,messagetype);
                message_type_signallist->setMapping(messagetype,QString("%1-%2").arg(currentRow).arg(2));
                QObject::connect(messagetype,SIGNAL(currentIndexChanged(int)),message_type_signallist,SLOT(map()));

                ui->tableWidget->setItem(currentRow,3,new QTableWidgetItem(sys_inf->Get_SingleDevice(sys_inf->Get_SingleMessage
                                                                               (messagegroup[i_temp])._Get_MessageSendDevice())._Get_DeviceName()));
                ui->tableWidget->item(currentRow,3)->setFlags(ui->tableWidget->item(currentRow,3)->flags() & (~Qt::ItemIsEditable));
                ui->tableWidget->setItem(currentRow,4,new QTableWidgetItem(sys_inf->Get_SingleDevice(sys_inf->Get_SingleMessage
                                                                               (messagegroup[i_temp])._Get_MessageReceiveDevice())._Get_DeviceName()));
                ui->tableWidget->item(currentRow,4)->setFlags(ui->tableWidget->item(currentRow,4)->flags() & (~Qt::ItemIsEditable));
                ui->tableWidget->setItem(currentRow,5,new QTableWidgetItem(QString::number(sys_inf->Get_SingleMessage(messagegroup[i_temp])._Get_MessageRate())));
                ui->tableWidget->setItem(currentRow,6,new QTableWidgetItem(QString::number(sys_inf->Get_SingleMessage(messagegroup[i_temp])._Get_MessageSize())));
                ui->tableWidget->setItem(currentRow,7,new QTableWidgetItem(QString::number(sys_inf->Get_SingleMessage(messagegroup[i_temp])._Get_AimMessageReliability())));
                ui->tableWidget->setItem(currentRow,8,new QTableWidgetItem(QString::number(sys_inf->Get_SingleMessage(messagegroup[i_temp])._Get_AimMessageSecurity())));
            }
        }
    }
    QObject::connect(message_type_signallist,SIGNAL(mapped(const QString &)),this,SLOT(Set_Modified_InformaType(const QString&)));
    bflag1=1;
}

void messageassess::init_total_message()
{
    for(int i=0;i!=messagegroup.size();i++)
    {
        QString current_message=sys_inf->Get_SingleMessage(messagegroup[i])._Get_MessageName();
        ui->select_message->addItem(current_message);
        ui->message_selection->addItem(current_message);
    }
}


/*           *********************************************                    */
/*           *********************评估结果部分**************                    */
/*           *********************************************                    */
void messageassess::Set_Result_Initial()
{
    ui->pushButton->setFlat(true);
    ui->pushButton_2->setFlat(true);
    ui->pushButton_3->setFlat(true);
    ui->pushButton_4->setFlat(true);
    ui->stackedWidget->setCurrentIndex(0);
    current_page=0;
    Get_ProtocalGraph();
    ui->protocol_selection->addItems(protocal);
    ui->radioBreli->setChecked(1);    //设置当前根据安全需求确定关键设备为依据可靠性
    ui->analysis_S->setCurrentIndex(0);
    Set_CriticalDevice_S();
    ui->security_satify_stack->setCurrentIndex(1);
    //on_message_selection_currentIndexChanged(ui->message_selection->currentText());
    //View_TotalMess();
}

void messageassess::Get_ProtocalGraph()
{
    QMap<uint,uint> Protocal_Time;
    QList<QString> vecStrProtocalName;
    QStringList total_protocal=(QStringList()<<"CIP"<<"MODBUS"<<"DNP3"<<"PROFIBUS"<<"PROFINET"<<"POWERLINK ETHERNET"<<"OPC"<<"ETHERCAT"<<"ICCP");
    for(int i=0;i!=channelgroup.size();i++)
    {
        Channel_Informa currentchannel=sys_inf->Get_SingleChannel(channelgroup[i]);
        if(Protocal_Time.find(currentchannel._Get_ChannelProtocal())!=Protocal_Time.end())
            Protocal_Time[currentchannel._Get_ChannelProtocal()]++;
        else
        {
            Protocal_Time.insert(currentchannel._Get_ChannelProtocal(),1);
            vecStrProtocalName.push_back(total_protocal[currentchannel._Get_ChannelProtocal()-1]);
        }
    }
    QList<double> verStrProtocalId;
    QList<double> verStrTime;//用来求每个协议的百分比
    for(auto i_temp=Protocal_Time.begin();i_temp!=Protocal_Time.end();i_temp++)
    {
        verStrProtocalId.push_back(i_temp.key());
        verStrTime.push_back(i_temp.value());
    }
    CResultShow *protocal_pie=new CResultShow();
    QString title("显示系统中各种协议的使用情况");
    protocal_pie->PieChartDisplay(vecStrProtocalName,verStrTime,title,ui->widgetP);
}

QStringList* messageassess::Get_ProtocalText()
{
    QFile cXmlFile("D:/Program Files (x86)/X_Ship/DB_51aspx/protocal_suggestion.txt");
    if(!cXmlFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return NULL;
    }
    QTextStream floStream(&cXmlFile);
    QTextCodec *codec=QTextCodec::codecForName("UTF-8");
    floStream.setCodec(codec);
    QStringList *str_protocal=new QStringList(floStream.readAll().split("\n"));
    return str_protocal;
}

void data_analysis(uint unAimDeviceId,float& max_Device_Security,float& max_Device_reli,float& message_total,float& min_Device_Security)
{
    max_Device_Security=0;
    min_Device_Security=10;
    max_Device_reli=0;
    message_total=0;
    for(int itemp=0;itemp!=messagegroup.size();itemp++)
    {
        Message_Informa MItemp=sys_inf->Get_SingleMessage(messagegroup[itemp]);
        if(MItemp._Get_MessageSendDevice()==unAimDeviceId
                ||MItemp._Get_MessageReceiveDevice()==unAimDeviceId)
        {
            if(max_Device_Security<MItemp._Get_AimMessageSecurity())
                max_Device_Security=MItemp._Get_AimMessageSecurity();
            if(min_Device_Security>MItemp._Get_AimMessageSecurity())
                min_Device_Security=MItemp._Get_AimMessageSecurity();
            if(max_Device_reli<MItemp._Get_AimMessageReliability())
                max_Device_reli=MItemp._Get_AimMessageReliability();
            //if(MItemp._Get_MessageReceiveDevice()==unAimDeviceId)
                message_total=message_total+MItemp._Get_MessageRate()*MItemp._Get_MessageSize();
        }
    }
    return;
}


bool CompaByValue(pair<double,uint> &a1,pair<double,uint> &a2)
{
    return (a1.first)>(a2.first);
}

void set_sort(QList<double> &temp_reli,QListWidget *listw)
{
    listw->clear();
    QList<pair<double,uint>> sort_secu;
    for(int i=0;i!=devicegroup.size();i++)
    {
        sort_secu.push_back(pair<double,uint>(temp_reli[i],devicegroup[i]));
    }
    std::sort(sort_secu.begin(),sort_secu.end(),CompaByValue);
    for(int i=0;i!=devicegroup.size();i++)
    {
        listw->addItem(sys_inf->Get_SingleDevice(sort_secu[i].second)._Get_DeviceName());
    }
}

void messageassess::Set_CriticalDevice_S()
{
    QList<double> dDeviceId;
    QList<QString> vecStrDeviceName;
    QList<double> temp_secu,temp_reli,temp_message_total,temp_secu_min;
    for(int i=0;i!=devicegroup.size();i++)
    {
        dDeviceId.push_back(devicegroup[i]);
        vecStrDeviceName.push_back(sys_inf->Get_SingleDevice(devicegroup[i])._Get_DeviceName());
        float max_Device_Security=0,max_Device_reli=0,min_Device_Security=10;
        float message_single=0;
        data_analysis(devicegroup[i],max_Device_Security,
                             max_Device_reli,message_single,min_Device_Security);
        temp_secu.push_back(max_Device_Security);
        temp_reli.push_back(max_Device_reli);
        temp_message_total.push_back(message_single);
        temp_secu_min.push_back(min_Device_Security);
    }
    QList<QList<double>> CriticalDevice_S;
    QList<QString> vecStrGroupName_S;
    CriticalDevice_S.push_back(temp_reli);
    CriticalDevice_S.push_back(temp_secu);
    vecStrGroupName_S.push_back("根据相关报文最大可靠性需求");
    vecStrGroupName_S.push_back("由相关报文最大密级需求");
    QString Title_S("根据安全需求");
    CResultShow BarCriticalDevice;
    BarCriticalDevice.BarDisplay(vecStrDeviceName,CriticalDevice_S,vecStrGroupName_S,Title_S,ui->widgetS);
    /*m_CriticalDevice_S=new CResultDisplay(vecStrDeviceName, dDeviceId);
    m_CriticalDevice_S->AddOneGroupData(temp_reli,"根据相关报文最大可靠性需求");
    m_CriticalDevice_S->AddOneGroupData(temp_secu,"由相关报文最大密级需求");
    m_CriticalDevice_S->BarDisplay("根据安全需求",ui->widgetS);*/
    set_sort(temp_reli,ui->listWidget);
    set_sort(temp_secu,ui->listWidget_2);


    QString Title_C("根据流经设备的总流量");
    BarCriticalDevice.BarDisplay(vecStrDeviceName,temp_message_total,Title_C,ui->widgetC);
    /*m_CriticalDevice_C=new CResultDisplay(vecStrDeviceName, dDeviceId);
    m_CriticalDevice_C->AddOneGroupData(temp_message_total,"根据流经设备的总流量");
    m_CriticalDevice_C->BarDisplay("根据报文总大小",ui->widgetC);*/
    set_sort(temp_message_total,ui->listWidget_3);

    QList<QList<double>> CriticalDevice_B;
    QList<QString> vecStrGroupName_B;
    CriticalDevice_B.push_back(temp_secu);
    CriticalDevice_B.push_back(temp_secu_min);
    vecStrGroupName_B.push_back("提供最高密级服务");//根据高密级的等级来定，因为意味着后果非常严重；其次是根据低密级的等级来定，因为意味着相对容易入侵
    vecStrGroupName_B.push_back("提供最低密级服务");
    QString Title_B("由密级服务产生的边界安全");
    BarCriticalDevice.BarDisplay(vecStrDeviceName,CriticalDevice_B,vecStrGroupName_B,Title_B,ui->widgetB);
    /*m_CriticalDevice_B=new CResultDisplay(vecStrDeviceName, dDeviceId);
    m_CriticalDevice_B->AddOneGroupData(temp_secu,"提供最高密级服务");
    m_CriticalDevice_B->AddOneGroupData(temp_secu_min,"提供最低密级服务");
    m_CriticalDevice_B->BarDisplay("由密级服务产生的边界安全",ui->widgetB);*/

    QList<double> temp_secu_diff;
    for(int i=0;i!=devicegroup.size();i++)
        temp_secu_diff.push_back(temp_secu[i]-temp_secu_min[i]);
    set_sort(temp_secu_diff,ui->listWidget_4);

}

void messageassess::View_SingleMess(const QString& current_mess)
{
    if(m_message_index->m_ID_calculateResult.find(current_mess)==m_message_index->m_ID_calculateResult.end())
    {
        ui->total_score->setText("暂未评估");
    }
    else ui->total_score->setText(QString::number(m_message_index->m_ID_calculateResult[current_mess][1]));
    CResultShow Single_Mess_Secu;
    QList<QString> message_index_other;
    QList<double> message_index_value;
    for(int i_temp=0;i_temp!=message_index.size();i_temp++)
    {
        message_index_other.push_back(m_message_index->m_nodeMark[message_index[i_temp]]);
        if(m_message_index->m_ID_calculateResult.find(current_mess)==m_message_index->m_ID_calculateResult.end())
        {
            message_index_value.push_back(0);
        }
        else message_index_value.push_back(m_message_index->m_ID_calculateResult[current_mess][message_index[i_temp]]);
    }
    if(m_message_index->m_ID_calculateResult.find(current_mess)==m_message_index->m_ID_calculateResult.end())
    {
        //qDebug()<<"is zero"<<endl;
    }
    QString title("雷达图分析信息流安全各项指标");
    Single_Mess_Secu.PolarChartDisplay(message_index_other,message_index_value,title,ui->message_security);
}

void messageassess::View_TotalMess()
{
    CResultShow Total_Mess_Secu;
    QString title("所有信息的安全态势分析");
    QList<QString> message_total;
    QList<double> message_total_value;
    for(int i_temp=0;i_temp!=messagegroup.size();i_temp++)
    {
        QString current_mess=sys_inf->Get_SingleMessage(messagegroup[i_temp])._Get_MessageName();
        if(m_message_index->m_ID_calculateResult.find(current_mess)!=m_message_index->m_ID_calculateResult.end())
        {
            message_total.push_back(current_mess);
            message_total_value.push_back(m_message_index->m_ID_calculateResult[current_mess][1]);
        }
    }
    Total_Mess_Secu.BarDisplay(message_total,message_total_value,title,ui->total_security);
}


void messageassess::low_score_index(const QString &arg1)
{
    int table_location=0;
    QList<unsigned int> find;
    //QMap<QString, double>::iterator iter;
    QList<unsigned int> index = {NULL};
    int currentRow=0;
    QString node_name;
    double node_value;
    //设置分数较低的设备名
    ui->tableWidget_2->setRowCount(0);   //相当于重设表格，   用clear清空后，表格还在，只是内容没有
    ui->tableWidget_2->setColumnCount(4);
    find = m_message_index->m_nodeParentId.keys(1); //找到主根节点下的所有一级子指标的节点nodeid，组成列表
    ui->tableWidget_2->insertRow(currentRow);
    for(auto i=0;i<find.size();i++)
    {
        index.clear();
        index = m_message_index->m_nodeParentId.keys(find[i]);
        for(auto j=0;j<index.size();j++)
        {
            if(m_message_index->m_ID_calculateResult.find(arg1)==m_message_index->m_ID_calculateResult.end())
                continue;
             node_name = m_message_index->m_nodeMark[index[j]];
             node_value = m_message_index->m_ID_calculateResult[arg1][index[j]];   //不同的设备，分数不同，分开存储
             if(node_value>60) continue;
             ui->tableWidget_2->setItem(currentRow,table_location,new QTableWidgetItem(node_name));
             ui->tableWidget_2->item(currentRow,table_location)->setTextAlignment(Qt::AlignCenter); //设置文本内容居中

             table_location++;

             if(table_location==4)
             {
                 table_location = 0;
                 currentRow++;
                 ui->tableWidget_2->insertRow(currentRow);
             }
        }
    }
}


/*           *****************************************                    */
/*           **************************信号槽机制******                    */
/*           *****************************************                    */


void messageassess::Set_Modified_Channel(QTableWidgetItem *item)
{
    if(bflag2==false) return;
    int temp_row=item->row();
    uint temp_channelID=ui->ChannelWidget->item(temp_row,0)->text().toUInt();
    switch(item->column())
    {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        sys_inf->Get_SingleChannel(temp_channelID)._Set_ChannelCapacity(item->text().toFloat());
        break;
    }
}

void messageassess::Set_Modified_Informa(QTableWidgetItem *item)
{
    if(bflag1==false) return;
    int temp_row=item->row();
    uint temp_message=ui->tableWidget->item(temp_row,0)->text().toUInt();
    switch(item->column())
    {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        sys_inf->Get_SingleMessage(temp_message)._Set_MessageRate(item->text().toFloat());
        Set_CriticalDevice_S();
        on_Pchannel_capcity_clicked();
        break;
    case 6:
        sys_inf->Get_SingleMessage(temp_message)._Set_MessageSize(item->text().toUInt());
        Set_CriticalDevice_S();
        on_Pchannel_capcity_clicked();
        break;
    case 7:
        sys_inf->Get_SingleMessage(temp_message)._Set_AimMessageReliability(item->text().toFloat());
        Set_CriticalDevice_S();
        break;
    case 8:
        sys_inf->Get_SingleMessage(temp_message)._Set_AimMessageSecurity(item->text().toFloat());
        Set_CriticalDevice_S();
        break;
    }
}

void messageassess::Set_Modified_InformaType(const QString& current_message)
{
    QStringList position=current_message.split("-");
    int row=position[0].toInt();
    int column=position[1].toInt();
    QComboBox* current_combo=(QComboBox*)ui->tableWidget->cellWidget(row,column);
    sys_inf->Get_SingleMessage(ui->tableWidget->item(row,0)->text().toUInt())._Set_MessageType(current_combo->currentIndex()+1);
}

void messageassess::Set_Modified_ChannelType(const QString& current_channel)
{
    QStringList position=current_channel.split("-");
    int row=position[0].toInt();
    int column=position[1].toInt();
    QComboBox* current_combo=(QComboBox*)ui->ChannelWidget->cellWidget(row,column);
    switch(column)
    {
    case 3:
        sys_inf->Get_SingleChannel(ui->ChannelWidget->item(row,0)->text().toUInt())._Set_ChannelProtocal(current_combo->currentIndex()+1);
        Get_ProtocalGraph();
        break;
    case 4:
        sys_inf->Get_SingleChannel(ui->ChannelWidget->item(row,0)->text().toUInt())._Set_ChannelMedia(current_combo->currentIndex()+1);
        break;
    }
}

void messageassess::on_Csend_device_currentIndexChanged(int index)
{
    Set_MessageTable(index,ui->Creceive_device->currentIndex());
    Set_ChannelTable(index,ui->Creceive_device->currentIndex());
}

void messageassess::on_Creceive_device_currentIndexChanged(int index)
{
    Set_MessageTable(ui->Csend_device->currentIndex(),index);
    Set_ChannelTable(ui->Csend_device->currentIndex(),index);
}

void messageassess::on_protocol_selection_currentIndexChanged(int index)
{
    ui->risk->setText(m_Protocal_Text->at(2*index));
    ui->suggestion->setText(m_Protocal_Text->at(2*index+1));
}

void messageassess::on_pushButton_clicked()
{
    change_window();
    ui->stackedWidget->setCurrentIndex(0);
    current_page=0;
}

void messageassess::on_pushButton_2_clicked()
{
    change_window();
    ui->stackedWidget->setCurrentIndex(1);
    current_page=1;
}

void messageassess::on_pushButton_3_clicked()
{
    change_window();
    ui->stackedWidget->setCurrentIndex(2);
    current_page=2;
}

void messageassess::on_pushButton_4_clicked()
{
    change_window();
    ui->stackedWidget->setCurrentIndex(3);
    current_page=3;
}

void messageassess::change_window()
{
    switch(current_page)
    {
    case 0:
        ui->pushButton->setStyleSheet("");
        break;
    case 1:
        ui->pushButton_2->setStyleSheet("");
        break;
    case 2:
        ui->pushButton_3->setStyleSheet("");
        break;
    case 3:
        ui->pushButton_4->setStyleSheet("");
        break;
    }
}

void messageassess::on_radioBreli_clicked()
{
    ui->analysis_S->setCurrentIndex(0);
}


void messageassess::on_radioBconf_clicked()
{
    ui->analysis_S->setCurrentIndex(1);
}

void messageassess::on_Pinforma_inter_clicked()
{
    ui->topo_visual->clear();
    Display_Message1();
    m_renderer = new QSvgRenderer(QLatin1String("D:/Program Files (x86)/X_Ship/image/info.svg"));

//    QPainter painter(ui->topo_visual);
//    m_renderer->render(&painter);
    //标记
}

void messageassess::on_Pchannel_capcity_clicked()
{
    ui->topo_visual->clear();
    Display_Message2();
    m_renderer = new QSvgRenderer(QLatin1String("D:/Program Files (x86)/X_Ship/image/info.svg"));
//    QPainter painter(ui->topo_visual);
//    m_renderer->render(&painter);
}

void messageassess::on_total_informa_clicked()
{
    ui->topo_visual->clear();
    Display_Message3();

    m_renderer = new QSvgRenderer(QLatin1String("D:/Program Files (x86)/X_Ship/image/info.svg"));
//    QPainter painter(ui->topo_visual);
//    m_renderer->render(&painter);
}

void messageassess::on_edit_question_clicked()  //问卷作答点击按钮触发函数
{
    QString strWorkingDir="D:\\Program Files (x86)\\X_Ship";
    QDir::setCurrent(strWorkingDir);
    QString strQuestionnaireProgramPath=".\\QuestionnaireSystemUser.exe";
    QStringList strQuestionnaireCallArguments;
    strQuestionnaireCallArguments<<".\\WeifenLuo.WinFormsUI.Docking.dll";
    QProcess *myProcess=new QProcess(this);

    myProcess->start(strQuestionnaireProgramPath, strQuestionnaireCallArguments);
    myProcess->waitForFinished(-1);

    QuestionnaireAnalysis();

    m_message_index->DatabaseRead();
    m_message_index->Get_TheSecurity();   //赋值函数 1.10PM
    if(m_message_index->m_ID_calculateResult.find(ui->select_message->currentText())==m_message_index->m_ID_calculateResult.end())
        m_message_index->m_ID_calculateResult.insert(ui->select_message->currentText(),m_message_index->m_calculateResult);
    else m_message_index->m_ID_calculateResult[ui->select_message->currentText()]=m_message_index->m_calculateResult;

    on_message_selection_currentIndexChanged(ui->message_selection->currentText());

    View_TotalMess();
}

void messageassess::on_tableWidget_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    if(bflag1==false) return;
    ui->select_message->setCurrentText(ui->tableWidget->item(ui->tableWidget->currentRow(),1)->text());
}

void messageassess::on_Csubsystem_currentIndexChanged(int index)
{
    Set_MessageTable(ui->Csend_device->currentIndex(),ui->Creceive_device->currentIndex());
    Set_ChannelTable(ui->Csend_device->currentIndex(),ui->Creceive_device->currentIndex());
}

void messageassess::on_Cdomain_currentIndexChanged(int index)
{
    Set_MessageTable(ui->Csend_device->currentIndex(),ui->Creceive_device->currentIndex());
    Set_ChannelTable(ui->Csend_device->currentIndex(),ui->Creceive_device->currentIndex());
}

void messageassess::on_message_selection_currentIndexChanged(const QString &arg1)
{
    View_SingleMess(arg1);
    low_score_index(arg1);
}

void messageassess::on_total_score_view_clicked()
{
    ui->security_satify_stack->setCurrentIndex(1);
}

void messageassess::on_single_score_view_clicked()
{
    ui->security_satify_stack->setCurrentIndex(0);
}

void messageassess::on_radioButton_clicked(bool checked)
{
    m_message_index->Display_Index();
    svgWindow->setFile ("D:/Program Files (x86)/X_Ship/image/index.svg");    //8.6添加

    if(checked)
        svgWindow->show();
    else
        svgWindow->close();
}



void messageassess::on_tableWidget_2_itemClicked(QTableWidgetItem *item)
{
    QString file_name = m_path+"/"+QString(item->text())+".txt";
    QFile file(file_name);
    ui->suggestion->clear();
    QString info = item->text()+": 文件丢失！";
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        ui->suggestion_2->setPlainText(info);
        return;
    }
    QTextStream in(&file);
    ui->suggestion_2->setPlainText(in.readAll());
    file.close();
}
