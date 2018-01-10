//推送仓库测试
//推送仓库测试2
//推送测试3
#include "wuli.h"
#include "ui_wuli.h"
#include "index/index.h"
#include <QTextCodec>
#include <QDir>
#include <QProcess>
#include <QDebug>
#include "wuli_assess/wuli.h"
#include "index/admin_index_conf.h"
#include "questionnaire_analysis/questionnaire_analysis.h"
const QString path = "D:/Program Files (x86)/X_Ship/DB_51aspx/wuli_assess_KnowledgeBase";
extern QSqlDatabase m_dbConnect;
extern System_Informa *sys_inf;
extern QMap<QString,uint> subsystem;
extern QMap<QString,uint> domain;
extern QString creatName;
QList<Index*> index_select;
Index *index_conf;


wuli::wuli(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wuli)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    QPalette pale1=palette();
    pale1.setColor(QPalette::Window,QColor(255,255,255));
    setPalette(pale1);

    QString qss;
    QFile file(":/qss/1.qss");
    file.open(QFile::ReadOnly);
   if(file.isOpen())
   {
        qss = QLatin1String(file.readAll());
        this->setStyleSheet(qss);
        file.close();
   }
    move(190,40);
    //Index::DatabaseConnect();//一定要加啊，7.13移除
    index_select=index_select_return();
    index_conf=index_select[0];
    for(int i=0;i<8;i++)
    {
       name[i]=new QLabel;
       score[i]=new QLabel;
       ui->verticalLayout->addWidget(name[i]);
       ui->verticalLayout_2->addWidget(score[i]);
       //name[i]->setStyleSheet("QLabel{color: rgb(0, 255, 0);}");
       //score[i]->setStyleSheet("QLabel{color: rgb(0, 255, 0);}");
    }
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setHeaderLabels(QStringList()<<"指标名称"<<"权值");
    ui->treeWidget->setColumnWidth(0,480);
    index_conf->RebuildEvaluationTree(ui->treeWidget);
    ui->treeWidget->expandAll(); //结点全部展开

    //设置tableWidget等宽
    ui->wuli_tableWidget->verticalHeader()->setVisible(false); //隐藏行序号
    //ui->wuli_tableWidget->horizontalHeader()->setVisible(false); //隐藏列序号
    ui->wuli_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->wuli_tableWidget->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空


    //设置分数较低的设备名
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->verticalHeader()->setVisible(false); //隐藏行序号
    ui->tableWidget->horizontalHeader()->setVisible(false); //隐藏列序号
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
	

    show_information();//显示从xml读取的设备信息及设备类型的个数

    on_Type_result_show_currentIndexChanged(0);//初始化 ComBox控件显示设备类型及子类设备的名称
    on_equip_index_select_currentIndexChanged(0);//初始化 ComBox控件显示设备类型及子类设备的名称
    on_tabWidget_2_tabBarClicked(0);//初始化，显示柱状图

    svgWindow = new SvgWindow; //8.6添加    

    ui->server_number->setFocusPolicy(Qt::NoFocus); //设置QLineEdit不可获得鼠标焦点，即不能被选中编辑
    ui->monitor_number->setFocusPolicy(Qt::NoFocus);
    ui->control_number->setFocusPolicy(Qt::NoFocus);
    static int count=0;

    ////////////////// 12.28评估界面数据恢复测试///////////////////////
    xml_path = "D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles/";
    xml_path = xml_path+creatName+"/"+creatName+"_wuli.xml";
    qDebug()<<"结果保存路径"<<xml_path;
    for(int i=0;i<3;i++)
    {
        index_conf=index_select[i];  //以前是分类存数设备；现在是对每个index_conf都拷贝了一份数据
        index_conf->m_ID_calculateResult = index_conf->Read_AssessRecord(xml_path);
    }
    all_calculateResult=index_conf->m_ID_calculateResult;
    for(QMap<QString,QMap<uint,double>>::iterator iter = index_conf->m_ID_calculateResult.begin();iter!=index_conf->m_ID_calculateResult.end();iter++)
        device_total_score.insert(iter.key(),iter.value()[1]);

}

wuli::~wuli()
{
    delete ui;
}

/************************************************************************************************************/
//                                 //获取设备类型信息，然后显示到QLineEdit里
/************************************************************************************************************/
void wuli::show_information()
{
    QString m=0;
    int device_type[3]={0,0,0};
    QList<uint> devicegroup=sys_inf->Get_DeviceId_Vector();
    for(int i=0;i!=devicegroup.size();i++)
    {
        Device_Informa CurrentDevice=sys_inf->Get_SingleDevice(devicegroup[i]);
        int currentRow=ui->wuli_tableWidget->rowCount();
        m=QString::number(CurrentDevice._Get_DeviceType());
        if(m=="1")   //第一类设备
            device_type[0]++;
        else if(m=="2")
            device_type[1]++;
        else if(m=="3")
            device_type[2]++;

        ui->wuli_tableWidget->insertRow(currentRow);
        ui->wuli_tableWidget->setItem(currentRow,0,new QTableWidgetItem(QString::number(CurrentDevice._Get_Deviceid())));
        ui->wuli_tableWidget->setItem(currentRow,1,new QTableWidgetItem(CurrentDevice._Get_DeviceName()));
        ui->wuli_tableWidget->setItem(currentRow,2,new QTableWidgetItem(QString::number(CurrentDevice._Get_DeviceType())));

        ui->wuli_tableWidget->setItem(currentRow,3,new QTableWidgetItem(subsystem.key (CurrentDevice._Get_SystemType())));
        ui->wuli_tableWidget->setItem(currentRow,4,new QTableWidgetItem(domain.key (CurrentDevice._Get_DomainType())));

        ui->wuli_tableWidget->setItem(currentRow,5,new QTableWidgetItem(QString::number(CurrentDevice._Get_DeviceMTBF())));
        ui->wuli_tableWidget->setItem(currentRow,6,new QTableWidgetItem(QString::number(CurrentDevice._Get_DeviceMTTR())));
        ui->wuli_tableWidget->setItem(currentRow,7,new QTableWidgetItem(VectorToString(CurrentDevice._Get_Devicelink())));
    }

    //设置物理设备概览某几列不可编辑,设置单元格内容居中
    for(auto i=0;i<ui->wuli_tableWidget->rowCount();i++)
    {
        for(auto col=0;col<ui->wuli_tableWidget->columnCount();col++)
            ui->wuli_tableWidget->item(i,col)->setTextAlignment(Qt::AlignCenter);
        for(auto j=0;j<8;j++)
        {
            QTableWidgetItem *item = ui->wuli_tableWidget->item(i,j);
            item->setFlags(item->flags()&(~Qt::ItemIsEditable));
            //或者用下面这一行代码也可以
            //ui->wuli_tableWidget->item(i,j)->setFlags(ui->wuli_tableWidget->item(i,j)->flags()&~Qt::ItemIsEditable);
        }
    }
    ui->server_number->setText(QString::number(device_type[0]));
    ui->server_number->setAlignment(Qt::AlignCenter);
    ui->monitor_number->setText(QString::number(device_type[1]));
    ui->monitor_number->setAlignment(Qt::AlignCenter);
    ui->control_number->setText(QString::number(device_type[2]));
    ui->control_number->setAlignment(Qt::AlignCenter);
}


/************************************//*点击问卷作答后，调用问卷作答软件*//**********************************************/
//            包括题目分数计算、原子指标分数存储到数据库；   读取数据库，刷新index类的各项数据
//        计算非原子指标得分，并存储（待定7.12PM）；    根据当前ComboBox内容，对应存储到内存中m_ID_calculateResult
//            获取设备名称，和总的根节点计算得分，保存到device_total_score里，供后续画图使用
/******************************************************************************************************************/
void wuli::on_answer_questionnaire_clicked()
{
    //先清空数据库的得分内容

     //9.6pm 添加注释，自动作答问卷
    QString strWorkingDir="D:\\Program Files (x86)\\X_Ship";
    QDir::setCurrent(strWorkingDir);
    QString strQuestionnaireProgramPath=".\\QuestionnaireSystemUser.exe";
    QStringList strQuestionnaireCallArguments;
    strQuestionnaireCallArguments<<".\\WeifenLuo.WinFormsUI.Docking.dll";
    QProcess *myProcess=new QProcess(this);

    myProcess->start(strQuestionnaireProgramPath, strQuestionnaireCallArguments);

    myProcess->waitForFinished(-1);

    QuestionnaireAnalysis();
    //问卷软件退出后，调用问卷作答、计算、存储 函数  （每次刷新数据库）
    //9.6pm 添加注释，自动作答问卷




    //创建当前设备类型、设备名称所对应的 存储空间
    index_conf->DatabaseRead();   //因为每次新的设备作答后，数据库被更新了，所以，需要重新读取各项评估分数

    index_conf->Get_TheSecurity(); //根据最新的数据，计算得分，暂时存到类里面,计算的得分已经更新到数据库了

    //把最新的计算结果，存到对应的设备QMap空间里。实现不同设备区分

    //index_conf->m_ID_calculateResult[ui->detail_equip_select->currentText()].clear();//清空相同的内容项（待定）
    //保存当前选择的评估项（后期考虑重复点击、设备名字重复等问题）

    index_conf->m_ID_calculateResult.insert(ui->detail_equip_select->currentText(),index_conf->m_calculateResult);
    //把当前评估的设备的名称和总分数存储下来 ， 为绘制 柱状图 做准备
    device_total_score.insert( ui->detail_equip_select->currentText(), index_conf->m_ID_calculateResult[ui->detail_equip_select->currentText()][1] );


    //************************2017.12.27新加代码***************************************//
    if(index_conf->m_ID_calculateResult[ui->detail_equip_select->currentText()][1] > 0)  //如果当前设备正确评估（正确作答了相应的问卷，且总得分不为-1）
    {        
        all_calculateResult.insert(ui->detail_equip_select->currentText(),index_conf->m_calculateResult);
        index_conf->AssessRecord(all_calculateResult,xml_path);
    }



    for(QMap<QString,QMap<uint,double>>::Iterator iter = all_calculateResult.begin();iter!=all_calculateResult.end();iter++)
    {
        //qDebug()<<"测试输出设备名称"<<iter.key();
        for(QMap<uint,double>::Iterator i = iter.value().begin();i!=iter.value().end();i++)
        {
        //   qDebug()<<"测试输出设备得分"<<i.key()<<" " <<i.value();
        }
     }


//    ////////////////// 12.28评估界面数据恢复测试///////////////////////
//    for(int i=0;i<3;i++)
//    {
//        index_conf=index_select[i];  //以前是分类存数设备；现在是对每个index_conf都拷贝了一份数据
//        index_conf->m_ID_calculateResult = index_conf->Read_AssessRecord();
//    }
//    for(QMap<QString,QMap<uint,double>>::iterator iter = index_conf->m_ID_calculateResult.begin();iter!=index_conf->m_ID_calculateResult.end();iter++)
//        device_total_score.insert(iter.key(),iter.value()[1]);

}




/************************************************************************************************************/
//                                     //指标页面，下拉时对应项的返回索引值
/************************************************************************************************************/
void wuli::on_equip_index_select_currentIndexChanged(int index)
{
    //qDebug()<<"当前index："<<index;
    index_conf=index_select[index];//下拉菜单变动时，直接传递当前的选项索引值index
    index_conf->RebuildEvaluationTree(ui->treeWidget);
    ui->treeWidget->expandAll(); //结点全部展开
    on_Type_result_show_currentIndexChanged(index+3);//修改设备详细下拉表内容
    /*****************************显示指标图片，后续要改成SVG图***************************/
}


/************************************************************************************************************/
//                          //数据库及类连接 定位，并动态刷新关联第二个ComboBox的内容
/************************************************************************************************************/
void wuli::on_Type_result_show_currentIndexChanged(int index)
{
    QString m=0;
    QList<QString> device_name[3];
    QList<uint> devicegroup=sys_inf->Get_DeviceId_Vector();

    if(index<3)
        index_conf=index_select[index];//下拉菜单变动时，直接传递当前的选项索引值index
    else
        index_conf=index_select[index-3];//下拉菜单变动时，直接传递当前的选项索引值index


    for(int i=0;i!=devicegroup.size();i++)
    {
        Device_Informa CurrentDevice=sys_inf->Get_SingleDevice(devicegroup[i]);
        m=QString::number(CurrentDevice._Get_DeviceType());
        if(m=="1")
           device_name[0].append(CurrentDevice._Get_DeviceName());
        else if(m=="2")
            device_name[1].append(CurrentDevice._Get_DeviceName());
        else if(m=="3")
            device_name[2].append(CurrentDevice._Get_DeviceName());
        else//第四类非信息类设备，不填加到下拉列表
        {}
     }
    if(index<3)
    {
        ui->device_result_show->clear();//先清空评估界面combox的内容
        //如果为显示界面
        if(index == 0)
            ui->device_result_show->addItems(device_name[0]);
        else if(index == 1)
            ui->device_result_show->addItems(device_name[1]);
        else// if(index == 2)
            ui->device_result_show->addItems(device_name[2]);
    }
    else
    {

        ui->detail_equip_select->clear();//先清空选择界面combox的内容
        //如果为指标浏览界面
        if(index == 3)
            ui->detail_equip_select->addItems(device_name[0]);
        else if(index == 4)
            ui->detail_equip_select->addItems(device_name[1]);
        else// if(index == 5)
            ui->detail_equip_select->addItems(device_name[2]);
    }
}




/************************************************************************************************************/
//                              低分数指标tableWidget显示
/************************************************************************************************************/
void wuli::low_score_index(const QString &arg1)
{
    int i,table_location=0;
    QList<unsigned int> find;
    QMap<QString, double>::iterator iter;
    QList<unsigned int> index = {NULL};
    int currentRow=0;
    QString node_name;
    double node_value;
    //设置分数较低的设备名
    ui->tableWidget->setRowCount(0);   //相当于重设表格，   用clear清空后，表格还在，只是内容没有
    ui->tableWidget->setColumnCount(4);
    find = index_conf->m_nodeParentId.keys(1); //找到主根节点下的所有一级子指标的节点nodeid，组成列表
    ui->tableWidget->insertRow(currentRow);
    for(auto i=0;i<find.size();i++)
    {
        index.clear();
        index = index_conf->m_nodeParentId.keys(find[i]);
        for(auto j=0;j<index.size();j++)
        {
             node_name = index_conf->m_nodeMark[index[j]];   //分数低的指标名称
             node_value = index_conf->m_ID_calculateResult[arg1][index[j]];   //分数低的指标得分
             if(node_value>60) continue;
             ui->tableWidget->setItem(currentRow,table_location,new QTableWidgetItem(node_name));
             ui->tableWidget->item(currentRow,table_location)->setTextAlignment(Qt::AlignCenter); //设置文本内容居中

             //qDebug()<<"talbeWidget:  "<<"当前行数："<<currentRow<<"当前列数"<<table_location<<node_name;
             table_location++;

             if(table_location==4)
             {
                 table_location = 0;
                 currentRow++;
                 ui->tableWidget->insertRow(currentRow);
             }
        }
    }
}




/************************************************************************************************************/
//                           /评估结果显示界面，设备下拉列表内容发生改变时  //
/************************************************************************************************************/
void wuli::on_device_result_show_currentIndexChanged(const QString &arg1)
{
    //qDebug()<<"当前文字："<<arg1; //小bug，当选择的设备大类发生变化时，设备详细列表被清空，然后会触发 本槽函数
    QMap<QString, double> score_show;
    int i,list_cout;
    QList<unsigned int> find;
    QMap<QString, double>::iterator iter;
    QList<QString> index_name = {NULL};
    QList<double> index_value = {NULL};
    QString strTile = arg1+" : 尚未进行评估";
    QList<unsigned int> index = {NULL};

    find = index_conf->m_nodeParentId.keys(1); //找到主根节点下的所有一级子指标的节点nodeid，组成列表
    list_cout = find.size();
    ui->tableWidget->clear();
    ui->suggestion->clear();

    ////////六大属性数据清空////////////
    for(auto j=0;j<8;j++)
    {
        name[j]->clear();
        score[j]->clear();
    }
    ui->total_score->clear();
    m_radarPicture.PolarChartDisplay(index_name, index_value, strTile, ui->PolarChart);
    ////////六大属性数据清空////////////

    if(index_conf->m_ID_calculateResult.find(arg1)==index_conf->m_ID_calculateResult.end())
    {
        qDebug()<<"当前内容："<<arg1;
        ui->total_score->setText("当前设备尚未评估！！");
    }
    else
    {
        if(index_conf->m_ID_calculateResult[arg1][1] < 0)
        {
            //QMessageBox::warning(0,"提示！","问卷未填写，或者填写错误");
            ui->total_score->setText("当前设备评估问卷未填写或填写错误！");
            return;
        }
        for(i = 0;i<list_cout;i++)
        {
            QString node_name = index_conf->m_nodeMark[find[i]];  //名称在一个指标体系下是相同的
            double node_value = index_conf->m_ID_calculateResult[arg1][find[i]];   //不同的设备，分数不同，分开存储            
            score_show.insert(node_name,node_value); //获取子指标的名称和分数
        }       
        index_name.clear();
        index_value.clear();
        for(i=0,iter = score_show.begin();iter!=score_show.end();iter++,i++)
        {
            name[i]->setText(iter.key());
            if(iter.value()<0)
                score[i]->setText("未答或漏答");
            else
                score[i]->setText(QString::number(iter.value(),10, 2));

            index_name.append(iter.key());
            index_value.append(iter.value());
        }
        ui->total_score->setText(QString::number(index_conf->m_ID_calculateResult[arg1][1]));   //主根节点，nodeid为1，zhijie直接找它的分数
        QString strTile = arg1+" : 评估结果";
        m_radarPicture.PolarChartDisplay(index_name, index_value, strTile, ui->PolarChart);
        low_score_index(arg1);
    }
}

/************************************************************************************************************/
//    由于在评估结果显示界面可能变更了 数据库/类型 的连接 ，所以回到 指标界面时，需要重新定位当前指标体系所对应的表，防止紊乱  //
/************************************************************************************************************/
void wuli::on_tabWidget_tabBarClicked(int index)
{
    int i;
    //on_device_result_show_currentIndexChanged("声呐");
    if(ui->detail_equip_select->currentText() == ui->device_result_show->currentText()) //如果第二标签页的设备名和结果显示的标签名名称一样，那么，就强制刷新
    {
        on_device_result_show_currentIndexChanged(ui->device_result_show->currentText());
    }
    if(index == 1)
    {
        i = ui->equip_index_select->currentIndex();
        index_conf=index_select[i];//切换到第二页时，先读取当前下拉列表选择的是第几项，然后重新定位到 3个 index中的某一个
    }
    if(index==2)
    {
        on_tabWidget_2_tabBarClicked(1);
        on_tabWidget_2_tabBarClicked(0);
    }
    //qDebug()<<"点击测试";
}


/************************************************************************************************************/
//                                点击评估结果标签，更新绘制柱状图                                               //
/************************************************************************************************************/
void wuli::on_tabWidget_2_tabBarClicked(int index)
{
//    static int count=0;
//    if(count==2)
//        count=2;
//    else
//    {
//       on_Type_result_show_currentIndexChanged(1);
//       count++;
//    }
    QMap<QString,double>::iterator i;
    QList<QString> name;
    QList<double> value;    

    for(i=device_total_score.begin();i!=device_total_score.end();i++)
    {
        if(i.value()<0)    continue;  //如果因为没有填写问卷导致最终评分小于 0， 那么不显示当前项
        name.append(i.key());
        value.append(i.value());
    }
    m_barPicture.BarDisplay(name, value,"艇载设备评估结果",ui->bar_Chart);
}

/************************************************************************************************************/
//                               切换视图                                            //
/************************************************************************************************************/
void wuli::on_radioButton_clicked(bool checked)
{
    index_conf->Display_Index();

    svgWindow->setFile ("D:/Program Files (x86)/X_Ship/image/index.svg");    //8.6添加
    if(checked)
        svgWindow->show();
    else
        svgWindow->close();

}


/************************************************************************************************************/
//                                              单击分数很低的项目，打开对应的txt文件，并显示到文本框中               //
/************************************************************************************************************/
void wuli::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
   QString file_name = path+"/"+QString(item->text())+".txt";
   QFile file(file_name);
   ui->suggestion->clear();
   QString info = item->text()+": 文件丢失！";
   if(!file.open(QFile::ReadOnly | QFile::Text))
   {
       ui->suggestion->setPlainText(info);
       return;
   }
   QTextStream in(&file);
   ui->suggestion->setPlainText(in.readAll());
   file.close();
}

