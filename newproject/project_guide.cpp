#include "newproject/project_guide.h"
#include "ui_project_guide.h"
#include <QMouseEvent>
#include <qdebug.h>
#include "sys_discript/system_informa.h"
#include<QMessageBox>
extern System_Informa* sys_inf;
extern QMap<QString,uint> subsystem;
extern QMap<QString,uint> domain;
extern int active_module;
extern QString creatName;
project_guide::project_guide(const bool *item,int length,QWidget *parent):
    QDialog(parent),ui(new Ui::project_guide)
{


    ui->setupUi(this);
    this->setWindowTitle("新建工程向导");
    {
        for(auto i=0;i<length;i++)
        {
            select_Item[i]=item[i];
             //qDebug()<<"测试"<<select_Item[i];
        }
    }
    initial();
}

/************************************************************************************************************/
//                                 //构造函数里初始化
/************************************************************************************************************/
void project_guide::initial()
{
    this->setMouseTracking(true);//设置光标追踪事件
    ui->next->setMouseTracking(true);
    ui->before->setMouseTracking(true);
    ui->OK->setEnabled(false);
    ui->OK->hide();
    ui->OK->setStyleSheet("background:lightgreen");
    ui->lineEdit1->setText("1");
    windowsID = 1;
    Xml_Info = new QTableWidget(this);
    Xml_Info->setVisible(0);
    //设置分数较低的设备名
    Xml_Info->setColumnCount(8);
    Xml_Info->verticalHeader()->setVisible(false); //隐藏行序号
    //Xml_Info->horizontalHeader()->setVisible(false); //隐藏列序号
    Xml_Info->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空
    Xml_Info->setSelectionBehavior(QAbstractItemView::SelectRows);
    Xml_Info->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //x先自适应宽度
    Xml_Info->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列
    QStringList header;
    header<<"设备ID"<<"设备名"<<"设备类型"<<"平均无故障时间"<<"平均修复时间"<<"关联设备";
    Xml_Info->setHorizontalHeaderLabels(header);
    ui->horizontalLayout_8->removeItem(ui->space);
    ui->horizontalLayout_8->addWidget(Xml_Info);
    xml_wuli();
    items_judge();
    //qDebug()<<"选择的项目个数为"<<items;

}


/************************************************************************************************************/
//                                 //判断选择了那几个评估组件
/************************************************************************************************************/
void project_guide::items_judge()
{

   QList<QString> list;
   if(select_Item[0]==true) //如果只选择了物理评估
    {
       items = 1;
       active_module=1;  //利用激活模块参数，以及 新建项目选中的模块，激活主界面的按钮。
       list.append("物理 ");
    }
   if(select_Item[1]==true)
    {
       items = 2;
       active_module=2;
       list.append("架构 ");
    }
   if(select_Item[0]==true&&select_Item[2]==true&&select_Item[1]==false) //如果选择了物理评估和信息流
   {
       items = 3;
       list.append("信息 ");       
       active_module=3;
   }
   if(select_Item[0]==true&&select_Item[1]==true&&select_Item[2]==true&&select_Item[3]==false) //如果选择了物理评估和信息流
   {
       items = 4;
       list.append("信息 ");
       active_module=4;
   }
   if(select_Item[3]==true) //如果选择了工作流
   {
       items = 5;active_module=5;list<<"信息 "<<"工作流 ";
   }
   if(select_Item[4]==true) //如果选择了态势仿真
   {
       active_module=6;
       list.append("态势仿真");
   }
   ui->lineEdit2->setText(QString::number(items));
   /******************       2018.1.2添加  往工程文件中写入激活模块个数   *******************/
   QFile pro_file("D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles/projects.dat");
   pro_file.open(QIODevice::WriteOnly|QIODevice::Append);
   QTextStream pro_stream(&pro_file);
   for(int i=0;i<list.size();i++)
   {
       pro_stream<<list.at(i);
   }
       pro_stream<<","<<active_module<<"\n";
}
void project_guide::xml_link()
{
    QStringList header;
    header<<"链路ID"<<"链路头"<<"链路尾"<<"信道容量"<<"通信协议类型"<<"通信介质";
    int currentRow=0;
    Xml_Info->setVisible(1);
    Xml_Info->setRowCount(0);   //相当于重设表格，   用clear清空后，表格还在，只是内容没有
    Xml_Info->setColumnCount(6);
    Xml_Info->setHorizontalHeaderLabels(header);
    if(read_link.size()==0)
    {
    QList<uint> linkgroup=sys_inf->Get_ChannelId_Vector();
    for(int i=0;i!=linkgroup.size();i++)
    {
        Channel_Informa CurrentLink=sys_inf->Get_SingleChannel(linkgroup[i]);
        int currentRow=Xml_Info->rowCount();
        Xml_Info->insertRow(currentRow);
        Xml_Info->setItem(currentRow,0,new QTableWidgetItem(QString::number(CurrentLink._Get_ChannelId())));
        Xml_Info->setItem(currentRow,1,new QTableWidgetItem(QString::number(CurrentLink._Get_Channel().first)));
        Xml_Info->setItem(currentRow,2,new QTableWidgetItem(QString::number(CurrentLink._Get_Channel().second)));
        Xml_Info->setItem(currentRow,3,new QTableWidgetItem(QString::number(CurrentLink._Get_ChannelCapacity())));
        Xml_Info->setItem(currentRow,4,new QTableWidgetItem(QString::number(CurrentLink._Get_ChannelProtocal())));
        Xml_Info->setItem(currentRow,5,new QTableWidgetItem(QString::number(CurrentLink._Get_ChannelMedia())));
        for(auto row=0;row<Xml_Info->columnCount();row++)
            Xml_Info->item(currentRow,row)->setTextAlignment(Qt::AlignCenter); //设置文本内容居中
    }
    }
    else //如果表格信息被更改的话，就读去更改后存储到内存中的数据
    {
        for(int i=0;i!=read_link.size();i++)
        {
            int currentRow=Xml_Info->rowCount();
            Xml_Info->insertRow(currentRow);
            Xml_Info->setItem(currentRow,0,new QTableWidgetItem(read_link[i][0]));
            Xml_Info->setItem(currentRow,1,new QTableWidgetItem(read_link[i][1]));
            Xml_Info->setItem(currentRow,2,new QTableWidgetItem(read_link[i][2]));
            Xml_Info->setItem(currentRow,3,new QTableWidgetItem(read_link[i][3]));
            Xml_Info->setItem(currentRow,4,new QTableWidgetItem(read_link[i][4]));
            Xml_Info->setItem(currentRow,5,new QTableWidgetItem(read_link[i][5]));
            for(auto row=0;row<Xml_Info->columnCount();row++)
                Xml_Info->item(currentRow,row)->setTextAlignment(Qt::AlignCenter); //设置文本内容居中
        }
    }
}

void project_guide::xml_message()
{
    QStringList header;
    header<<"信息ID"<<"信息名"<<"信息类型"<<"发送设备"<<"接收设备"<<"发送频率"<<"数据包大小";
    int currentRow=0;
    Xml_Info->setVisible(1);
    Xml_Info->setRowCount(0);   //相当于重设表格，   用clear清空后，表格还在，只是内容没有
    Xml_Info->setColumnCount(7);
    Xml_Info->setHorizontalHeaderLabels(header);
    if(read_message.size()==0)
    {
    QList<uint> messagegroup=sys_inf->Get_MessageId_Vector();
    for(int i=0;i!=messagegroup.size();i++)
    {
        Message_Informa CurrentMessage=sys_inf->Get_SingleMessage(messagegroup[i]);
        int currentRow=Xml_Info->rowCount();
        Xml_Info->insertRow(currentRow);
        Xml_Info->setItem(currentRow,0,new QTableWidgetItem(QString::number(CurrentMessage._Get_Messageid())));
        Xml_Info->setItem(currentRow,1,new QTableWidgetItem(CurrentMessage._Get_MessageName()));
        Xml_Info->setItem(currentRow,2,new QTableWidgetItem(QString::number(CurrentMessage._Get_MessageType())));
        Xml_Info->setItem(currentRow,3,new QTableWidgetItem(QString::number(CurrentMessage._Get_MessageSendDevice())));
        Xml_Info->setItem(currentRow,4,new QTableWidgetItem(QString::number(CurrentMessage._Get_MessageReceiveDevice())));
        Xml_Info->setItem(currentRow,5,new QTableWidgetItem(QString::number(CurrentMessage._Get_MessageRate())));
        Xml_Info->setItem(currentRow,6,new QTableWidgetItem(QString::number(CurrentMessage._Get_MessageSize())));
        for(auto row=0;row<Xml_Info->columnCount();row++)
            Xml_Info->item(currentRow,row)->setTextAlignment(Qt::AlignCenter); //设置文本内容居中
    }
    }
    else
    {
        for(int i=0;i!=read_message.size();i++)
        {
            int currentRow=Xml_Info->rowCount();
            Xml_Info->insertRow(currentRow);
            Xml_Info->setItem(currentRow,0,new QTableWidgetItem(read_message[i][0]));
            Xml_Info->setItem(currentRow,1,new QTableWidgetItem(read_message[i][1]));
            Xml_Info->setItem(currentRow,2,new QTableWidgetItem(read_message[i][2]));
            Xml_Info->setItem(currentRow,3,new QTableWidgetItem(read_message[i][3]));
            Xml_Info->setItem(currentRow,4,new QTableWidgetItem(read_message[i][4]));
            Xml_Info->setItem(currentRow,5,new QTableWidgetItem(read_message[i][5]));
            Xml_Info->setItem(currentRow,6,new QTableWidgetItem(read_message[i][6]));

            for(auto row=0;row<Xml_Info->columnCount();row++)
                Xml_Info->item(currentRow,row)->setTextAlignment(Qt::AlignCenter); //设置文本内容居中
        }
    }
}


void project_guide::xml_renwu()
{
    QStringList header;
    header<<"任务ID"<<"任务名"<<"前置任务"<<"任务之间联系"<<"关联设备"<<"设备与任务的关系"<<"pre-info";
    int currentRow=0;
    Xml_Info->setVisible(1);
    Xml_Info->setRowCount(0);   //相当于重设表格，   用clear清空后，表格还在，只是内容没有
    Xml_Info->setColumnCount(7);
    Xml_Info->setHorizontalHeaderLabels(header);
    if(read_renwu.size()==0)
    {
    QList<uint> taskgroup=sys_inf->Get_TaskId_Vector();
    for(int i=0;i!=taskgroup.size();i++)
    {


        Task_Informa CurrentRenwu=sys_inf->Get_SingleTask(taskgroup[i]);
        int currentRow=Xml_Info->rowCount();
        Xml_Info->insertRow(currentRow);
        Xml_Info->setItem(currentRow,0,new QTableWidgetItem(QString::number(CurrentRenwu._Get_TaskId())));
        Xml_Info->setItem(currentRow,1,new QTableWidgetItem(CurrentRenwu._Get_TaskName()));
        Xml_Info->setItem(currentRow,2,new QTableWidgetItem(VectorToString(CurrentRenwu._Get_PreTask())));
        Xml_Info->setItem(currentRow,3,new QTableWidgetItem(CurrentRenwu._Get_TaskRelation()));

        Xml_Info->setItem(currentRow,4,new QTableWidgetItem(VectorToString(CurrentRenwu._Get_TaskDevice())));

        Xml_Info->setItem(currentRow,5,new QTableWidgetItem(CurrentRenwu._Get_DeviceToTask()));
        Xml_Info->setItem(currentRow,6,new QTableWidgetItem(VectorToString(CurrentRenwu._Get_PreInfo())));
        for(auto row=0;row<Xml_Info->columnCount();row++)
            Xml_Info->item(currentRow,row)->setTextAlignment(Qt::AlignCenter); //设置文本内容居中
        //Xml_Info->setItem(currentRow,7,new QTableWidgetItem(QString::number(CurrentRenwu._Get_TaskSecurity())));
    }
    }
    else
    {
        for(int i=0;i!=read_renwu.size();i++)
        {
            int currentRow=Xml_Info->rowCount();
            Xml_Info->insertRow(currentRow);

            Xml_Info->setItem(currentRow,0,new QTableWidgetItem(read_renwu[i][0]));
            Xml_Info->setItem(currentRow,1,new QTableWidgetItem(read_renwu[i][1]));
            Xml_Info->setItem(currentRow,2,new QTableWidgetItem(read_renwu[i][2]));
            Xml_Info->setItem(currentRow,3,new QTableWidgetItem(read_renwu[i][3]));
            Xml_Info->setItem(currentRow,4,new QTableWidgetItem(read_renwu[i][4]));
            Xml_Info->setItem(currentRow,5,new QTableWidgetItem(read_renwu[i][5]));
            Xml_Info->setItem(currentRow,6,new QTableWidgetItem(read_renwu[i][6]));

            for(auto row=0;row<Xml_Info->columnCount();row++)
                Xml_Info->item(currentRow,row)->setTextAlignment(Qt::AlignCenter); //设置文本内容居中
        }
    }
}

void project_guide::xml_yewu()
{
    QStringList header;
    header<<"业务ID"<<"业务名"<<"业务安全需求"<<"业务任务集";
    int currentRow=0;
    Xml_Info->setVisible(1);
    Xml_Info->setRowCount(0);   //相当于重设表格，   用clear清空后，表格还在，只是内容没有
    Xml_Info->setColumnCount(4);
    Xml_Info->setHorizontalHeaderLabels(header);

    if(read_yewu.size()==0)
    {
    QList<uint> yewugroup=sys_inf->Get_BusinessId_Vector();
    for(int i=0;i!=yewugroup.size();i++)
    {
        Business_Informa CurrentYewu=sys_inf->Get_SingleBusiness(yewugroup[i]);
        int currentRow=Xml_Info->rowCount();
        Xml_Info->insertRow(currentRow);
        Xml_Info->setItem(currentRow,0,new QTableWidgetItem(QString::number(CurrentYewu._Get_BusinessId())));
        Xml_Info->setItem(currentRow,1,new QTableWidgetItem(CurrentYewu._Get_BusinessName()));
        Xml_Info->setItem(currentRow,2,new QTableWidgetItem(QString::number(CurrentYewu._Get_BusinessReqSecurity())));
        Xml_Info->setItem(currentRow,3,new QTableWidgetItem(VectorToString(CurrentYewu._Get_RelationTask())));
        for(auto row=0;row<Xml_Info->columnCount();row++)
            Xml_Info->item(currentRow,row)->setTextAlignment(Qt::AlignCenter); //设置文本内容居中
    }
    }
    else
    {
        for(int i=0;i!=read_yewu.size();i++)
        {
            int currentRow=Xml_Info->rowCount();
            Xml_Info->insertRow(currentRow);

            Xml_Info->setItem(currentRow,0,new QTableWidgetItem(read_yewu[i][0]));
            Xml_Info->setItem(currentRow,1,new QTableWidgetItem(read_yewu[i][1]));
            Xml_Info->setItem(currentRow,2,new QTableWidgetItem(read_yewu[i][2]));
            Xml_Info->setItem(currentRow,3,new QTableWidgetItem(read_yewu[i][3]));

            for(auto row=0;row<Xml_Info->columnCount();row++)
                Xml_Info->item(currentRow,row)->setTextAlignment(Qt::AlignCenter); //设置文本内容居中
        }
    }
}


void project_guide::xml_wuli()
{
    QStringList header;
    header<<"设备ID"<<"设备名"<<"设备类型"<<"所属系统"<<"安全域类型"<<"平均无故障时间"<<"平均修复时间"<<"关联设备";
    int currentRow=0;
    Xml_Info->setVisible(1);
    Xml_Info->setRowCount(0);   //相当于重设表格，   用clear清空后，表格还在，只是内容没有
    Xml_Info->setColumnCount(8);
    Xml_Info->setHorizontalHeaderLabels(header);

    if(read_wuli.size()==0)
    {
        QList<uint> devicegroup=sys_inf->Get_DeviceId_Vector();
        for(int i=0;i!=devicegroup.size();i++)
        {
            Device_Informa CurrentDevice=sys_inf->Get_SingleDevice(devicegroup[i]);
            int currentRow=Xml_Info->rowCount();
            Xml_Info->insertRow(currentRow);

            Xml_Info->setItem(currentRow,0,new QTableWidgetItem(QString::number(CurrentDevice._Get_Deviceid())));
            Xml_Info->setItem(currentRow,1,new QTableWidgetItem(CurrentDevice._Get_DeviceName()));
            Xml_Info->setItem(currentRow,2,new QTableWidgetItem(QString::number(CurrentDevice._Get_DeviceType())));
            Xml_Info->setItem(currentRow,3,new QTableWidgetItem(subsystem.key (CurrentDevice._Get_SystemType())));
            Xml_Info->setItem(currentRow,4,new QTableWidgetItem(domain.key (CurrentDevice._Get_DomainType())));
            Xml_Info->setItem(currentRow,5,new QTableWidgetItem(QString::number(CurrentDevice._Get_DeviceMTBF())));
            Xml_Info->setItem(currentRow,6,new QTableWidgetItem(QString::number(CurrentDevice._Get_DeviceMTTR())));
            Xml_Info->setItem(currentRow,7,new QTableWidgetItem(VectorToString(CurrentDevice._Get_Devicelink())));
            for(auto row=0;row<Xml_Info->columnCount();row++)
                Xml_Info->item(currentRow,row)->setTextAlignment(Qt::AlignCenter); //设置文本内容居中
        }
    }
    else
    {
        for(int i=0;i!=read_wuli.size();i++)
        {
            int currentRow=Xml_Info->rowCount();
            Xml_Info->insertRow(currentRow);

            Xml_Info->setItem(currentRow,0,new QTableWidgetItem(read_wuli[i][0]));
            Xml_Info->setItem(currentRow,1,new QTableWidgetItem(read_wuli[i][1]));
            Xml_Info->setItem(currentRow,2,new QTableWidgetItem(read_wuli[i][2]));
            Xml_Info->setItem(currentRow,3,new QTableWidgetItem(read_wuli[i][3]));
            Xml_Info->setItem(currentRow,4,new QTableWidgetItem(read_wuli[i][4]));
            Xml_Info->setItem(currentRow,5,new QTableWidgetItem(read_wuli[i][5]));
            Xml_Info->setItem(currentRow,6,new QTableWidgetItem(read_wuli[i][6]));
            Xml_Info->setItem(currentRow,7,new QTableWidgetItem(read_wuli[i][7]));
            for(auto row=0;row<Xml_Info->columnCount();row++)
                Xml_Info->item(currentRow,row)->setTextAlignment(Qt::AlignCenter); //设置文本内容居中
        }
    }
}


project_guide::~project_guide()
{
    delete ui;
}

void project_guide::get_select_info(bool item[5])
{
    for(auto i=0;i<5;i++)
    {
        select_Item[i] = item[i];
        //qDebug()<<"测试"<<select_Item[i];
    }
    items_judge();


}
void project_guide::on_before_clicked()
{
    read_table();
    workCount();
    if(windowsID==1) return;  //如果是最小步骤数，就不能再使windowsID减少
    windowsID--;
    window_judge();
    ui->lineEdit1->setText(QString::number(windowsID));
}

void project_guide::on_next_clicked()
{
    read_table();
    workCount();

    if(windowsID==items) return;  //如果是最大步骤数，就不能再使windowsID增加
    windowsID++;
    window_judge();    
    ui->lineEdit1->setText(QString::number(windowsID));
}

void project_guide::workCount()
{
    if(windowsID == items)
    {
        ui->OK->setEnabled(true);
        ui->OK->show();
    }
}
//处理鼠标移动事件,用来实时刷新判断复选框的选择状态
void project_guide::window_judge()
{
    if(windowsID == items)
    {
        ui->next->hide();
        ui->next->setEnabled(false);
        ui->OK->setEnabled(true);
        ui->OK->show();
    }
    else
    {
        ui->next->show();
        ui->next->setEnabled(true);
        ui->OK->setEnabled(false);
        ui->OK->hide();
    }
    if(windowsID == 1)
    {
        ui->before->hide();
        ui->before->setEnabled(false);
    }
    else
    {
        ui->before->show();
        ui->before->setEnabled(true);
    }
    if(windowsID == 1)
    {
        xml_wuli();
    }
    else if(windowsID == 2)
    {
       xml_link();
    }
    else if(windowsID == 3)
    {
       xml_message();

    }
    else if(windowsID == 4)
    {
        xml_yewu();
    }
    else
    {
        xml_renwu();
    }


}
void project_guide::on_OK_clicked() //由于xml文件必须完整，所以当新建工程为部分模块时，需要把欠缺的xml信息补充上
{
    read_table();
    if(items==1||items==2) {xml_link();windowsID=2;read_table();xml_message();windowsID=3;read_table();xml_yewu();windowsID=4;read_table();xml_renwu();windowsID=5;read_table();}
    else if(items==3) {xml_yewu();windowsID=4;read_table();xml_renwu();windowsID=5;read_table();}
    else if(items==4||items==5) {}
    else {}
    creat_xml();

    done(QDialog::Accepted);
}

void project_guide::creat_xml()
{
    QString save_path="D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles/"+creatName+"/"+creatName+".xml";
    qDebug()<<"保存路径测试"<<save_path;
    QDomDocument dom;
    QDomProcessingInstruction instruction;
    instruction=dom.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"");
    dom.appendChild(instruction);
    QDomElement system=dom.createElement("system");
    QDomElement devices=dom.createElement("devices");
    for(auto i=0;i<read_wuli.size();i++)
    {
        if(read_wuli.empty())
            continue;
        int j=0;
        QDomElement device=dom.createElement("device");
        QDomElement device_id=dom.createElement("device_id");
        QDomText device_id_text=dom.createTextNode(read_wuli[i][j]);
        device_id.appendChild(device_id_text);
        device.appendChild(device_id);

        j++;
        QDomElement device_name=dom.createElement("device_name");
        QDomText device_name_text=dom.createTextNode(read_wuli[i][j]);
        device_name.appendChild(device_name_text);
        device.appendChild(device_name);

        j++;
        QDomElement device_type=dom.createElement("device_type");
        QDomText device_type_text=dom.createTextNode(read_wuli[i][j]);
        device_type.appendChild(device_type_text);
        device.appendChild(device_type);
        j++;
        QDomElement system_type=dom.createElement("system_type");
        QDomText system_type_text=dom.createTextNode(read_wuli[i][j]);
        system_type.appendChild(system_type_text);
        device.appendChild(system_type);

        j++;
        QDomElement domain_type=dom.createElement("domain_type");
        QDomText domain_type_text=dom.createTextNode(read_wuli[i][j]);
        domain_type.appendChild(domain_type_text);
        device.appendChild(domain_type);

        j++;
        QDomElement device_mtbf=dom.createElement("device_mtbf");
        QDomText device_mtbf_text=dom.createTextNode(read_wuli[i][j]);
        device_mtbf.appendChild(device_mtbf_text);
        device.appendChild(device_mtbf);

        j++;
        QDomElement device_mttr=dom.createElement("device_mttr");
        QDomText device_mttr_text=dom.createTextNode(read_wuli[i][j]);
        device_mttr.appendChild(device_mttr_text);
        device.appendChild(device_mttr);

        j++;
        QDomElement device_link=dom.createElement("device_link");
        QDomText device_link_text=dom.createTextNode(read_wuli[i][j]);
        device_link.appendChild(device_link_text);
        device.appendChild(device_link);
        devices.appendChild(device);
    }
    system.appendChild(devices);


    QDomElement channels=dom.createElement("channels");
    for(auto i=0;i<read_link.size();i++)
    {
        if(read_link.empty())
            continue;
        int j=0;
        QDomElement channel=dom.createElement("channel");
        QDomElement channel_id=dom.createElement("channel_id");
        QDomText channel_id_text=dom.createTextNode(read_link[i][j]);
        channel_id.appendChild(channel_id_text);
        channel.appendChild(channel_id);
        j++;
        QDomElement channel_min=dom.createElement("channel_min");
        QDomText channel_min_text=dom.createTextNode(read_link[i][j]);
        channel_min.appendChild(channel_min_text);
        channel.appendChild(channel_min);
        j++;
        QDomElement channel_max=dom.createElement("channel_max");
        QDomText channel_max_text=dom.createTextNode(read_link[i][j]);
        channel_max.appendChild(channel_max_text);
        channel.appendChild(channel_max);
        j++;
        QDomElement channel_capacity=dom.createElement("channel_capacity");
        QDomText channel_capacity_text=dom.createTextNode(read_link[i][j]);
        channel_capacity.appendChild(channel_capacity_text);
        channel.appendChild(channel_capacity);
        j++;
        QDomElement channel_protocal=dom.createElement("channel_protocal");
        QDomText channel_protocal_text=dom.createTextNode(read_link[i][j]);
        channel_protocal.appendChild(channel_protocal_text);
        channel.appendChild(channel_protocal);
        j++;
        QDomElement channel_media=dom.createElement("channel_media");
        QDomText channel_media_text=dom.createTextNode(read_link[i][j]);
        channel_media.appendChild(channel_media_text);
        channel.appendChild(channel_media);

        channels.appendChild(channel);
    }
    system.appendChild(channels);




    QDomElement informas=dom.createElement("informas");
    for(auto i=0;i<read_message.size();i++)
    {
        if(read_message.empty())
            continue;
        int j=0;
        QDomElement informa=dom.createElement("informa");
        QDomElement informa_id=dom.createElement("informa_id");
        QDomText informa_id_text=dom.createTextNode(read_message[i][j]);
        informa_id.appendChild(informa_id_text);
        informa.appendChild(informa_id);
        j++;
        QDomElement informa_name=dom.createElement("informa_name");
        QDomText informa_name_text=dom.createTextNode(read_message[i][j]);
        informa_name.appendChild(informa_name_text);
        informa.appendChild(informa_name);
        j++;
        QDomElement informa_type=dom.createElement("informa_type");
        QDomText informa_type_text=dom.createTextNode(read_message[i][j]);
        informa_type.appendChild(informa_type_text);
        informa.appendChild(informa_type);
        j++;
        QDomElement send_device=dom.createElement("send_device");
        QDomText send_device_text=dom.createTextNode(read_message[i][j]);
        send_device.appendChild(send_device_text);
        informa.appendChild(send_device);
        j++;
        QDomElement receive_device=dom.createElement("receive_device");
        QDomText receive_device_text=dom.createTextNode(read_message[i][j]);
        receive_device.appendChild(receive_device_text);
        informa.appendChild(receive_device);
        j++;
        QDomElement informa_rate=dom.createElement("informa_rate");
        QDomText informa_rate_text=dom.createTextNode(read_message[i][j]);
        informa_rate.appendChild(informa_rate_text);
        informa.appendChild(informa_rate);
        j++;
        QDomElement informa_size=dom.createElement("informa_size");
        QDomText informa_size_text=dom.createTextNode(read_message[i][j]);
        informa_size.appendChild(informa_size_text);
        informa.appendChild(informa_size);

        informas.appendChild(informa);
    }
    system.appendChild(informas);


    QDomElement businesses=dom.createElement("businesses");
    for(auto i=0;i<read_yewu.size();i++)
    {
        if(read_yewu.empty())
            continue;
        int j=0;
        QDomElement business=dom.createElement("business");
        QDomElement business_id=dom.createElement("business_id");
        QDomText business_id_text=dom.createTextNode(read_yewu[i][j]);
        business_id.appendChild(business_id_text);
        business.appendChild(business_id);
        j++;
        QDomElement business_name=dom.createElement("business_name");
        QDomText business_name_text=dom.createTextNode(read_yewu[i][j]);
        business_name.appendChild(business_name_text);
        business.appendChild(business_name);
        j++;
        QDomElement business_Ave_Require=dom.createElement("business_Ave_Require");
        QDomText business_Ave_Require_text=dom.createTextNode(read_yewu[i][j]);
        business_Ave_Require.appendChild(business_Ave_Require_text);
        business.appendChild(business_Ave_Require);
        j++;
        QDomElement relation_task=dom.createElement("relation_task");
        QDomText relation_task_text=dom.createTextNode(read_yewu[i][j]);
        relation_task.appendChild(relation_task_text);
        business.appendChild(relation_task);
        businesses.appendChild(business);
    }
    system.appendChild(businesses);

    QDomElement tasks=dom.createElement("tasks");
    for(auto i=0;i<read_renwu.size();i++)
    {

        if(read_yewu.empty())
            continue;
        int j=0;
        QDomElement task=dom.createElement("task");
        QDomElement task_id=dom.createElement("task_id");
        QDomText task_id_text=dom.createTextNode(read_renwu[i][j]);
        task_id.appendChild(task_id_text);
        task.appendChild(task_id);
        j++;
        QDomElement task_name=dom.createElement("task_name");
        QDomText task_name_text=dom.createTextNode(read_renwu[i][j]);
        task_name.appendChild(task_name_text);
        task.appendChild(task_name);
        j++;
        QDomElement pre_task=dom.createElement("pre_task");
        QDomText pre_task_text=dom.createTextNode(read_renwu[i][j]);
        pre_task.appendChild(pre_task_text);
        task.appendChild(pre_task);
        j++;
        QDomElement task_to_task=dom.createElement("task_to_task");
        QDomText task_to_task_text=dom.createTextNode(read_renwu[i][j]);
        task_to_task.appendChild(task_to_task_text);
        task.appendChild(task_to_task);
        j++;
        QDomElement relation_device=dom.createElement("relation_device");
        QDomText relation_device_text=dom.createTextNode(read_renwu[i][j]);
        relation_device.appendChild(relation_device_text);
        task.appendChild(relation_device);
        j++;
        QDomElement device_to_task=dom.createElement("device_to_task");
        QDomText device_to_task_text=dom.createTextNode(read_renwu[i][j]);
        device_to_task.appendChild(device_to_task_text);
        task.appendChild(device_to_task);
        j++;
        QDomElement pre_info=dom.createElement("pre_info");
        QDomText pre_info_text=dom.createTextNode(read_renwu[i][j]);
        pre_info.appendChild(pre_info_text);
        task.appendChild(pre_info);
        tasks.appendChild(task);
    }
    system.appendChild(tasks);



    dom.appendChild(system);

    QFile *file=new QFile(save_path);

    if(file->open(QIODevice::WriteOnly|QIODevice::Append))
    {
        file->resize(0);
        QTextStream stream(file);
        dom.save(stream,4);
        file->close();
    }

}

void project_guide::read_table()
{
    //qDebug()<<"开始创建xml,窗口号"<<windowsID;
    int row=Xml_Info->rowCount();
    int col=Xml_Info->columnCount();
    QVector<QVector<QString> > tmp;

    tmp.resize(row);
    for(auto j=0;j<row;j++)
        tmp[j].resize(col); //初始化二维Vector数组的大小。

    for(auto j=0;j<col;j++)
    {
        if(Xml_Info->item(0,0)==NULL)
            break;
        for(auto i=0;i<row;i++)
        {
            QTableWidgetItem *item = Xml_Info->item(i,j);  //重设xml后，因为没有insert item所以，不加上一句if判断，是会出错的
                tmp[i][j].push_back(item->text());
        }
    }

    //qDebug()<<"容器行大小为： "<<tmp.size();
   // qDebug()<<"容器列大小为： "<<tmp[0].size();


    if(windowsID==1)
    {
        read_wuli   =tmp;
        //qDebug()<<"测试第一个窗口输出"<<read_wuli;
    }
    else if(windowsID==2)
      {  read_link   =tmp;
    //qDebug()<<"测试第2个窗口输出"<<read_wuli;
    }
    else if(windowsID==3)
     {   read_message=tmp;
    //qDebug()<<"测试第3个窗口输出"<<read_wuli;
    }
    else if(windowsID==4)
      {  read_yewu  =tmp;
    //qDebug()<<"测试第4个窗口输出"<<read_wuli;
    }
    else
       { read_renwu   =tmp;
       // qDebug()<<"测试第5个窗口输出"<<read_wuli;
       }
}

//void project_guide::on_pushButton_clicked()
//{
//    QMessageBox msgBox(QMessageBox::Warning, tr("警告"),"是否清空表格内容？", 0, this);
//    msgBox.addButton(tr("确定"), QMessageBox::AcceptRole);
//    msgBox.addButton(tr("取消"), QMessageBox::RejectRole);
//    if (msgBox.exec() == QMessageBox::AcceptRole)
//    {
//        Xml_Info->clear();
//        Xml_Info->setRowCount(1);   //相当于重设表格，   用clear清空后，表格还在，只是内容没有

//        if(windowsID==1)
//            Xml_Info->setColumnCount(8);
//        else if(windowsID==2)
//            Xml_Info->setColumnCount(6);
//        else if(windowsID==3)
//            Xml_Info->setColumnCount(7);
//        else if(windowsID==4)
//            Xml_Info->setColumnCount(4);
//        else
//            Xml_Info->setColumnCount(7);
//    }

//}

void project_guide::on_addRow_clicked()
{
    int row = Xml_Info->rowCount();
    Xml_Info->insertRow(row);
}

void project_guide::on_pushButton_3_clicked()  //删除行槽函数
{
    //int row = Xml_Info->rowCount();

    int rowIndex = Xml_Info->currentRow();
    //qDebug()<<"当前所指行数"<<rowIndex;
    QMessageBox msgBox(QMessageBox::Warning, tr("警告"),"确定要删除该行数据？", 0, this);
    msgBox.addButton(tr("确定"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("取消"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::AcceptRole)
    {
        if (rowIndex != -1)
            Xml_Info->removeRow(rowIndex);
        ui->label_6->setFocus();
    }
}
