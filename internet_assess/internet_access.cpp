#include "internet_access.h"
#include "main/mainwindow.h"
#include "ui_internet_access.h"
#include "index/admin_index_conf.h"
#include "internet_assess/internetfunction.h"
#include "sys_discript/base_informa.h"
#include "sys_discript/system_informa.h"
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QSqlDatabase>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QSqlError>
#include <QTableWidget>
#include <QListWidgetItem>
#include <QButtonGroup>
#include <QDebug>
#include <map>
#include <QImage>
#include <QCheckBox>
#include <QComboBox>
#include <QSqlQuery>
#include <QMessageBox>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include "questionnaire_analysis/questionnaire_analysis.h"

extern System_Informa* sys_inf;
extern QMap<QString,uint> subsystem;
extern QSqlDatabase m_dbConnectPro;//全局变量打开数据库文件
extern QMap<QString,int> indexNameNotFound;

const QString liuchengImagePath("D:/Program Files (x86)/X_Ship/rc/internetIndex.bmp");
const QString InternetImagePath("D:/Program Files (x86)/X_Ship/image/internetDot.jpg");
const QString tuopuImagePath("D:/Program Files (x86)/X_Ship/image/zhuxitong.jpg");
QString subSystemtuopuImagePath("D:/Program Files (x86)/X_Ship/image/tuopuImage");
const QString quyuImagePath("D:/Program Files (x86)/X_Ship/rc/quyu1.png");
const QString kekuozhanImagePath("D:/Program Files (x86)/X_Ship/rc/kuozhan1.png");
const QString zaibeiImagePath("D:/Program Files (x86)/X_Ship/rc/zaibei1.png");
const QString bianjieImagePath("D:/Program Files (x86)/X_Ship/rc/bianjieSecurity1.png");
const QString suggestionPath("D:\\Program Files (x86)\\X_Ship\\DB_51aspx\\jiagou_assess_KnowledgeBase\\");


bool flag_FinishInternetAssess[7]={0,0,0,0,0,0,0};
QMap<uint,double> tuopuResult;//保存计算出来的每个子系统的安全性结果

InternetAccess::InternetAccess(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InternetAccess)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    QPalette pale1=palette();
    pale1.setColor(QPalette::Window,QColor(255,255,255));
    setPalette(pale1);
    move(190,40);//子窗口的偏移量
    setAutoFillBackground(true);
     m_pInternetGraphviz = new CInfoInternetVizObj();

    ui->label_index->adjustSize();
    showTuopuComboBox();
    vector<vector<unsigned int> > unMatrix=GetAdjacencyMatrix();
    QList<uint> nKIndex=DegreeDistribution(unMatrix);
    Set_Table(nKIndex);

    connect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(tableWidget_cellChanged(QTableWidgetItem*)));
    ShowPicture(imageTuopu);

    index_select=index_select_return();
    index_question=index_select[3];//初始化时，赋予表名为：网络架构指标树
    index_question->DatabaseRead();
    index_result  = index_question->m_nodeMark;

    ui->treeWidget->setColumnWidth(0,240);
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setHeaderLabels(QStringList()<<"指标名称"<<"权值");
    index_question->RebuildEvaluationTree(ui->treeWidget);
    ui->treeWidget->expandAll(); //结点全部展开




    move(190,40);//子窗口的偏移量
    ui->label_tuoputu_1->installEventFilter(this);  //将svg图片显示在界面的label上
    ui->label_tuoputu_2->installEventFilter(this);  //将svg图片显示在界面的label上
    ui->label_tuoputu_3->installEventFilter(this);  //将svg图片显示在界面的label上
    m_renderer = new QSvgRenderer(QLatin1String("D:/Program Files (x86)/X_Ship/image/zhuxitong.svg"));     //需要刷新调用图片时，把这句话重新重新放到要刷新的位置

    //如果已经评估过，则需要恢复上次评估的结果
    on_pushButton_faultPropagationL_clicked();//显示评估距离
    on_pushButton_faultPropagationC_clicked();//显示聚类指数
    on_pushButton_checkGuzhangRes_clicked();//查看故障传播性评估结果
    on_pushButton_checkQAnswer_clicked();//显示上次评估架构部分四套问卷的评估结果
    on_tabWidget_tabBarClicked(0);//显示最终所有原子指标评估结果
}

InternetAccess::~InternetAccess()
{
    delete ui;
}


bool InternetAccess::eventFilter(QObject *watched, QEvent *event)//将svg图片显示在label上增加的代码
{
    if(watched == ui->label_tuoputu_1 && event->type() == QEvent::Paint)
    {
        QPainter painter(ui->label_tuoputu_1);
        if(m_renderer==new QSvgRenderer(QLatin1String("D:/Program Files (x86)/X_Ship/image/info.svg")))
            return false;
        else
        m_renderer->render(&painter);

    }
    if(watched == ui->label_tuoputu_2 && event->type() == QEvent::Paint)
    {
        QPainter painter(ui->label_tuoputu_2);
        m_renderer = new QSvgRenderer(QLatin1String("D:/Program Files (x86)/X_Ship/image/info.svg"));     //需要刷新调用图片时，把这句话重新重新放到要刷新的位置

        m_renderer->render(&painter);
    }
    if(watched == ui->label_tuoputu_3 && event->type() == QEvent::Paint)
    {
        QPainter painter(ui->label_tuoputu_3);
        m_renderer = new QSvgRenderer(QLatin1String("D:/Program Files (x86)/X_Ship/image/info.svg"));     //需要刷新调用图片时，把这句话重新重新放到要刷新的位置

        m_renderer->render(&painter);
    }


    return QWidget::eventFilter(watched,event);
}
//架构安全性评估中动态生成combo Box选项
void InternetAccess::showTuopuComboBox()
{
    ui->comboBox_tuopu->clear();
    ui->comboBox_tuopu->addItems(QStringList()<<"主系统");
    for(QMap<QString,uint>::iterator iter=subsystem.begin();iter!=subsystem.end();iter++)
        ui->comboBox_tuopu->addItems(QStringList()<<iter.key());

    //设置显示子系统安全性结果的表格
    ui->tableWidget_tuopuResult->setColumnCount(subsystem.size()+1);
    QStringList header;
    for(QMap<QString,uint>::iterator iter=subsystem.begin();iter!=subsystem.end();iter++)
        header.push_back(iter.key());
    header.push_back("主系统");
    ui->tableWidget_tuopuResult->setHorizontalHeaderLabels(header);
    ui->tableWidget_tuopuResult->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
//当combo Box选项发生变化时，调用显示相应的图片
void InternetAccess::on_comboBox_tuopu_currentIndexChanged(int index)
{
    QString temp=QString::number(index);
    QString path=subSystemtuopuImagePath+temp+".svg";
    //qDebug()<<"拓扑图片输出"<<path;

    if(index == 0)
    {
        m_pInternetGraphviz->InternetGraphviz(GetInfoForMainSystem_TuopuGraphviz(),true);
        m_renderer = new QSvgRenderer(QLatin1String("D:/Program Files (x86)/X_Ship/image/zhuxitong.svg"));     //需要刷新调用图片时，把这句话重新重新放到要刷新的位置

    }
    else
    {
        m_pInternetGraphviz->InternetGraphviz(GetInfoForSubSystem_TuopuGraphviz(),false);

        m_renderer = new QSvgRenderer(path);    //需要刷新调用图片时，把这句话重新重新放到要刷新的位置

    }

    //待定
    ui->tableWidget_Ser->clearContents();;
    ui->tableWidget_Par->clearContents();
}


//拓扑安全性评估，串联表格中的内容发生变化
void InternetAccess::on_tableWidget_Ser_itemChanged()
{
    int currentColumn=ui->tableWidget_Ser->columnCount();
    ui->tableWidget_Ser->insertColumn(currentColumn);

    QStringList head;//表头
    for(int i=1;i<currentColumn+2;i++)//动态增加表头
    {
        QString headName="串联节点"+QString::number(i);
        head <<headName;
    }
    ui->tableWidget_Ser->setHorizontalHeaderLabels(head);
    if(ui->comboBox_tuopu->currentIndex()!=0)
    {
        double dReliability_Table_Ser=SubsystemReliability_Table_Ser();
        double dReliability_Table_Par=SubsystemReliability_Table_Par();

        qDebug()<<"串联表格计算结果"<<dReliability_Table_Ser<<"并联表格计算结果"<<dReliability_Table_Par;
        if((dReliability_Table_Ser-invalidInput)>(-0.0005) && (dReliability_Table_Ser-invalidInput)<0.0005)
            QMessageBox::information(NULL, QString("错误"), "<font color='red'>非法输入拓扑图中没有的节点ID!</font>");
        else if((dReliability_Table_Par-invalidInput)>(-0.0005) && (dReliability_Table_Par-invalidInput)<0.0005)
            QMessageBox::information(NULL, QString("错误"), "<font color='red'>非法输入拓扑图中没有的节点ID!</font>");
        else
        {
            fReliability_System=dReliability_Table_Ser*dReliability_Table_Par;
            //把计算结果保存到相应的表格中并动态刷新
            QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(fReliability_System));
            ui->tableWidget_tuopuResult->setItem(0,ui->comboBox_tuopu->currentIndex()-1,newItem);
            //newItem->setTextAlignment (Qt::AlignCenter);
        }
    }
    else//填写的是主系统的串并联结果
    {
        MainSystemReliability_Table_SerAndPar();
    }

}
//拓扑安全性评估，并联表格中的内容发生变化
void InternetAccess::on_tableWidget_Par_itemChanged()
{
    int currentColumn=ui->tableWidget_Par->columnCount();
    int currentRow=ui->tableWidget_Par->rowCount();
    if(currentColumn < 2)
        ui->tableWidget_Par->insertColumn(currentColumn);
    if(currentRow < 2)
        ui->tableWidget_Par->insertRow(currentRow);

    if(currentColumn >=2 && currentRow >= 2)
    {
        if(ui->tableWidget_Par->item(currentRow-1,currentColumn-2)!=NULL && ui->tableWidget_Par->item(currentRow-1,currentColumn-2)->text()!=tr(""))
        {
            ui->tableWidget_Par->insertRow(currentRow);
        }
        if(ui->tableWidget_Par->item(0,currentColumn-1)!=NULL &&ui->tableWidget_Par->item(0,currentColumn-1)->text()!=tr(""))
        {
            ui->tableWidget_Par->insertColumn(currentColumn);
        }
    }
    //动态增加行表头和列表头
    QStringList headerHorizontal;//列表头
    for(int i=1;i<currentColumn+2;i++)//动态增加表头
    {
        QString headName="并联模块"+QString::number(i);
        headerHorizontal <<headName;
    }
    ui->tableWidget_Par->setHorizontalHeaderLabels(headerHorizontal);

    QStringList headerVertical;//行表头
    for(int i=1;i<currentRow+2;i++)//动态增加表头
    {
        QString headName="并联支路"+QString::number(i);
        headerVertical <<headName;
    }
    ui->tableWidget_Par->setVerticalHeaderLabels(headerVertical);

    if(ui->comboBox_tuopu->currentIndex()!=0)
    {
        double dReliability_Table_Ser=SubsystemReliability_Table_Ser();
        double dReliability_Table_Par=SubsystemReliability_Table_Par();

        if((dReliability_Table_Ser-invalidInput)>(-0.0005) && (dReliability_Table_Ser-invalidInput)<0.0005)
            QMessageBox::information(NULL, QString("错误"), "<font color='red'>非法输入拓扑图中没有的节点ID!</font>");
        else if((dReliability_Table_Par-invalidInput)>(-0.0005) && (dReliability_Table_Par-invalidInput)<0.0005)
            QMessageBox::information(NULL, QString("错误"), "<font color='red'>非法输入拓扑图中没有的节点ID!</font>");
        else
        {
            fReliability_System=dReliability_Table_Ser*dReliability_Table_Par;
            //把计算结果保存到相应的表格中并动态刷新
            QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(fReliability_System));
            ui->tableWidget_tuopuResult->setItem(0,ui->comboBox_tuopu->currentIndex()-1,newItem);
            //newItem->setTextAlignment (Qt::AlignCenter);
        }
    }
    else//填写的是主系统的串并联结果
    {
        MainSystemReliability_Table_SerAndPar();
    }

}

//拓扑安全性评估,子系统串联部分
double InternetAccess::SubsystemReliability_Table_Ser()
{
    QMap<uint, double> mapNodes;//存储一个子系统所有节点的可靠性
    if(ui->comboBox_tuopu->currentIndex()!=0)
    {
        QMap<uint,QMap<uint,double> > allInfo=GetSubSystemID_AndDevIDRel_ForTuopu();
        for(QMap<uint,QMap<uint,double> >::iterator iter=allInfo.begin();iter!=allInfo.end();iter++)
        {
            if(iter.key() == ui->comboBox_tuopu->currentIndex())
                mapNodes=iter.value();
        }
        qDebug()<<"bug测试点----1";

       // mapNodes.clear();

    }
    else
        mapNodes=tuopuResult;
    for(QMap<uint, double>::iterator it=mapNodes.begin();it!=mapNodes.end();it++)
    {
        //qDebug()<<"计算主系统时"<<it.key()<<"  "<<it.value();
        qDebug()<<"bug测试点----2";

    }

    QMap<uint, double>::iterator  iter;
    float fReliability_Table_Ser=1;
    for (int i=0;i < ui->tableWidget_Ser->columnCount();++i)
    {
        qDebug()<<"bug测试点----3";

        //判断指向该cell的指针是否为空,判断该cell的text是否为空
        if(ui->tableWidget_Ser->item(0,i)==NULL||(ui->tableWidget_Ser->item(0,i)&&ui->tableWidget_Ser->item(0,i)->text()==tr("")))
            continue;

        //ui->tableWidget_Par->item(0,i)->setTextAlignment(Qt::AlignCenter);
        qDebug()<<"bug测试点----5";

        QString sID=ui->tableWidget_Ser->item(0,i)->text();
        int nID=sID.toInt();
        iter = mapNodes.begin();
        while(iter != mapNodes.end() && nID != iter.key())
            ++iter;
        if(nID == iter.key())
        {
            if(iter.value() >= -0.05 && iter.value() <= 1.05)//float类型比较
                fReliability_Table_Ser *= iter.value();
            else
            {
                qDebug()<<"从这里返回";
                return invalidInput;
            }
        }
        else//非法输入拓扑图中没有的节点ID
        {
            qDebug()<<"串联部分非法输入拓扑图中没有的节点ID";
            ui->tableWidget_Ser->item(0,i)->text().clear();
            return invalidInput;
        }
        qDebug()<<"bug测试点----7";

    }
    return fReliability_Table_Ser;
}

//拓扑安全性评估,子系统并联部分
float InternetAccess::SubsystemReliability_Table_Par()
{
    QMap<uint, double> mapNodes;//存储一个子系统所有节点的可靠性
    if(ui->comboBox_tuopu->currentIndex()!=0)
    {
        QMap<uint,QMap<uint,double> > allInfo=GetSubSystemID_AndDevIDRel_ForTuopu();
        for(QMap<uint,QMap<uint,double> >::iterator iter=allInfo.begin();iter!=allInfo.end();iter++)
        {
            if(iter.key() == ui->comboBox_tuopu->currentIndex())
            {
                mapNodes=iter.value();
                //qDebug()<<"子系统的编号是"<<iter.key();
                break;
            }
        }
       // 输出这个子系统的所有节点
        //for(QMap<uint, double>::iterator iter=mapNodes.begin();iter!=mapNodes.end();iter++)
            //qDebug()<<"节点ID是"<<iter.key();
    }
    else
        mapNodes=tuopuResult;
    QMap<uint, double>::iterator  iter;
    float fReliability_Table_Par=1;
    float fTemp=1;
    float fResult=1;

    for(int j=0;j<ui->tableWidget_Par->columnCount();++j)
    {
        for(int i=0;i<ui->tableWidget_Par->rowCount();++i)
        {
            if(ui->tableWidget_Par->item(i,j)==NULL||             //判断指向该cell的指针是否为空
                (ui->tableWidget_Par->item(i,j)&&ui->tableWidget_Par->item(i,j)->text()==tr("")))  //判断该cell的text是否为空
               continue;
            //ui->tableWidget_Par->item(i,j)->setTextAlignment(Qt::AlignCenter);
            QString sID=ui->tableWidget_Par->item(i,j)->text();
            //qDebug()<<"string类型的ID节点"<<sID;
            QVector<int> unParNodes=Nodes_FromStringToInt(sID);
            /*for(int mm=0;mm<unParNodes.size();mm++)
                qDebug()<<"转换整型的节点ID"<<unParNodes[mm];*/
            for(int k=0;k<unParNodes.size();++k)
            {
                iter = mapNodes.begin();
                while(iter != mapNodes.end() && unParNodes[k] != iter.key())
                    ++iter;
                if(unParNodes[k] == iter.key())
                {
                    if(iter.value() >= 0 && iter.value() <= 1)
                        fTemp *= iter.value();
                    else
                    {
                        //qDebug()<<"从1处返回";
                        return invalidInput;
                    }
                }
                else
                {
                    //qDebug()<<"从2处返回";
                    return invalidInput;
                }
            }
            fTemp=1-fTemp;
            fReliability_Table_Par *=fTemp;
            fTemp=1;
            unParNodes.clear();
        }
        if(fReliability_Table_Par>0.9999 && fReliability_Table_Par<1.0001)
            continue;
        fResult *=(1-fReliability_Table_Par);
        fReliability_Table_Par=1;
    }
    return fResult;
}
//拓扑结构安全性评估，主系统串联部分
void InternetAccess::MainSystemReliability_Table_SerAndPar()
{
    bool notFinished=false;//所有的子系统是否已经有可靠性
    for(int i=0;i<ui->tableWidget_tuopuResult->columnCount()-1;++i)
    {
        if(ui->tableWidget_tuopuResult->item(0,i)==NULL||             //判断指向该cell的指针是否为空
            (ui->tableWidget_tuopuResult->item(0,i)&&ui->tableWidget_tuopuResult->item(0,i)->text()==tr("")))  //判断该cell的text是否为空
        {
            notFinished=true;
            break;
        }
    }
    if(notFinished)
        QMessageBox::information(NULL, QString("错误"), "<font color='red'>请先评估完子系统再评估主系统!</font>");
    else//所有的子系统都已经评估完成
    {
        //ui->label_tuopuResult->setText("");
        tuopuResult.clear();//保证动态刷新
        for(uint i=0;i<ui->tableWidget_tuopuResult->columnCount()-1;++i)
        {
            QString str_subsystemReliability=ui->tableWidget_tuopuResult->item(0,i)->text();
            float subsystemReliability=str_subsystemReliability.toFloat();
            tuopuResult.insert(i+1,subsystemReliability);
        }
        //把计算结果保存到相应的表格和map中
        float fReliability_MainSystem=SubsystemReliability_Table_Ser()*SubsystemReliability_Table_Par();
        tuopuResult.insert(0,fReliability_MainSystem);

        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(fReliability_MainSystem));
        ui->tableWidget_tuopuResult->setItem(0,subsystem.size(),newItem);
        newItem->setTextAlignment (Qt::AlignCenter);
        flag_FinishInternetAssess[0]=true;
        //把计算的主系统的拓扑安全性结果存入原子指标库
        QString strTbableName="网络架构指标树";
        QString indexName="拓扑结构安全性";
        float indexResult=fReliability_MainSystem*100;
        writeIndexResultToDB(strTbableName,indexName,indexResult);
    }
}

//拓扑结构安全性并联部分，将表格中的字符串转换为节点ID的int型
QVector<int> InternetAccess::Nodes_FromStringToInt(QString str)
{
    qDebug()<<"string转int类型，传入值是"<<str;
    QVector<int> unParNodes;
    QString str1(str);
    if (str1 == NULL)
        return unParNodes;
    int length=str1.length();
    while(str1[length-1] == 32)
        --length;
    int i=0,begin=i;
    for(;i<length;++i)
    {
        if (str1[i] == 32)
        {
            QString sNodes=str1.mid(begin,i-begin);
            //qDebug()<<"单个解析结果节点ID"<<sNodes;
            unParNodes.push_back(sNodes.toInt());
            if(i<length-1)
                begin=i+1;
        }
    }
    QString sNodes=str1.mid(begin,length-begin);
    //qDebug()<<"单个解析结果节点ID"<<sNodes;
    unParNodes.push_back(sNodes.toInt());
    return unParNodes;
}

//冗余性评估表格
void InternetAccess::Set_Table(QList<uint> nKIndex)
{
    //设置表格的信息
    ui->tableWidget->clearContents();
    ui->tableWidget->setColumnCount(0);

    //显示节点ID和设备名称
    for (unsigned int i=nKIndex.size()-1;i >=nKIndex.size()/2;--i)
    {
        int currentColumn=ui->tableWidget->columnCount();
        ui->tableWidget->insertColumn(currentColumn);
        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(nKIndex[i]));
        ui->tableWidget->setItem(0,currentColumn,newItem);
        newItem->setTextAlignment (Qt::AlignCenter);

        Device_Informa Device_nKIndex=sys_inf->Get_SingleDevice(nKIndex[i]);
        const QString &DeviceName_nKIndex=Device_nKIndex._Get_DeviceName();
        QTableWidgetItem *newItem1 = new QTableWidgetItem(DeviceName_nKIndex);
        ui->tableWidget->setItem(1,currentColumn,newItem1);
        newItem1->setTextAlignment (Qt::AlignCenter);
    }
    QStringList headerHorizontal;//列表头
    for(int i=1;i<=nKIndex.size()/2+1;i++)//动态增加表头
    {
        QString headName="关键节点"+QString::number(i);
        headerHorizontal <<headName;
    }
    ui->tableWidget->setHorizontalHeaderLabels(headerHorizontal);
    ui->tableWidget->setEditTriggers ( QAbstractItemView::NoEditTriggers );//表格不可编辑
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空

    /*int j=0;
    for (unsigned int i=nKIndex.size()-1;i >=nKIndex.size()/2;--i)
    {
        QWidget *widget;
        QHBoxLayout *hLayout;
        QCheckBox *ckb;
        ckb = new QCheckBox();
        hLayout = new QHBoxLayout();
        widget = new QWidget(ui->tableWidget);
        hLayout->addWidget(ckb);
        hLayout->setMargin(0);                          // 必须添加, 否则CheckBox不能正常显示
        hLayout->setAlignment(ckb, Qt::AlignCenter);
        ui->tableWidget->setLayout(hLayout);
        ui->tableWidget->setCellWidget(3,1, widget);
    }*/
    int j=0;
    for (unsigned int i=nKIndex.size()-1;i >=nKIndex.size()/2;--i)
    {
        QTableWidgetItem *check=new QTableWidgetItem();
        check->setCheckState (Qt::Unchecked);
        check->setFlags(check->flags() & (~Qt::ItemIsEditable));//复选框这一行可编辑
        ui->tableWidget->setItem(2,j++,check); //插入复选框
    }
    //不能加这三行，程序会卡在第二行那句，参数不正确
    /*QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(ui->tableWidget);
    ui->tableWidget->setLayout(layout);*/
}
//是否进行了冗余配置复选框
void InternetAccess::tableWidget_cellChanged(QTableWidgetItem* item)
{
    count=0;
    for(int col=0;col<ui->tableWidget->columnCount();++col)
    {
        if(ui->tableWidget->item (2,col)->checkState ()==Qt::Checked)
            count++;
    }
}
void InternetAccess::on_pushButton_CheckRedundancy_clicked()
{
    float res=100*count/ui->tableWidget->columnCount();
    //qDebug()<<res;
    ui->label_Redundancy->setText(QString::number(res));
    flag_FinishInternetAssess[1]=true;
    //把计算的主系统的冗余性结果存入原子指标库
    QString strTbableName="网络架构指标树";
    QString indexName="冗余性";
    writeIndexResultToDB(strTbableName,indexName,res);
}
//冗余性评估选项和结果
/*void InternetAccess::onRadioClickOption()
{
    int nRedundancyResult;
    switch(btnGroupOption->checkedId())
    {
    case 0:
            nRedundancyResult=100;//A选项对应的冗余性结果
            break;
    case 1:
            nRedundancyResult=80;
            break;
    case 2:
            nRedundancyResult=60;
            break;
    case 3:
            nRedundancyResult=40;
            break;
    case 4:
            nRedundancyResult=20;
            break;
    }
   ui->label_Redundancy->setText(QString::number(nRedundancyResult));
   flag_FinishInternetAssess[1]=true;
}*/

//故障传播性评估中，求取平均距离L的值
void InternetAccess::on_pushButton_faultPropagationL_clicked()
{
    vector<vector<uint> > nMatrix=GetAdjacencyMatrix();
    float fL = SimulationOfAverageDistance(nMatrix);
    if (fL != invalidInput)
    {
        ui->label_dL->setText(QString::number(fL));
        //数据归一化处理
        //fL=(fL-1)*100/18;
        fL=100/18*fL;
        qDebug()<<"fL值"<<fL;
        //把平均距离L结果存入原子指标库
        QString strTbableName="网络架构指标树";
        QString indexName="平均距离";
        writeIndexResultToDB(strTbableName,indexName,fL);
    }
    else
        ui->label_dL->setText("平均距离输出错误!");
}
//故障传播性评估中，求取聚类指数C的值
void InternetAccess::on_pushButton_faultPropagationC_clicked()
{
    vector<vector<uint> > nMatrix=GetAdjacencyMatrix();
    double dC = ClusteringIndex(nMatrix);
    if (dC != invalidInput)
    {
        ui->label_dC->setText(QString::number(dC));
        //数据归一化
        dC=(1-dC)*100;
        flag_FinishInternetAssess[2]=true;
        //把计算的主系统的聚类指数结果存入原子指标库
        QString strTbableName="网络架构指标树";
        QString indexName="聚类指数";
        writeIndexResultToDB(strTbableName,indexName,dC);
    }
    else
        ui->label_dC->setText("聚类指数输出错误!");
}
//查看故障传播性评估结果
void InternetAccess::on_pushButton_checkGuzhangRes_clicked()
{
    bool finished=true;
    double dC=0,dL=0;
    QSqlQuery queryTmp(m_dbConnectPro);
    QString strTmp = "select nodevalue,result from 网络架构指标树 where description='聚类指数'";
    queryTmp.exec(strTmp);
    queryTmp.next();
    if(-1!=queryTmp.value(1).toInt())
        dC=queryTmp.value(0).toDouble()*queryTmp.value(1).toDouble();
    else
    {
        finished=false;
        ui->label_dC->setText("聚类指数输出错误!");
    }
    strTmp = "select nodevalue,result from 网络架构指标树 where description='平均距离'";
    queryTmp.exec(strTmp);
    queryTmp.next();
    if(-1!=queryTmp.value(1).toInt())
        dL=queryTmp.value(0).toDouble()*queryTmp.value(1).toDouble();
    else
    {
        finished=false;
        ui->label_dL->setText("平均距离输出错误!");
    }
    if(finished)
        ui->label_faultPropagationResult->setText(QString::number(dC+dL));
    else
        ui->label_faultPropagationResult->setText("平均距离或聚类系数输出错误!");
}
//得到网络架构指标树中没有对应上题目的原子指标
QString static getIndexNameNotFound()
{
    QString show="";
    for(QMap<QString,int>::iterator iter=indexNameNotFound.begin();iter!=indexNameNotFound.end();iter++)
    {
        if(3==iter.value())//出现了网络架构指标树中的原子指标没有对应上题目
        {
            show += iter.key()+"\n";
        }
    }
    return show;
}

//最终评估结果
/*void InternetAccess::on_pushButton_finalResult_clicked()
{
    double internetFinalRes=0;
    bool finished=true;//检查原子指标是否有评估结果
    QList<QString> list_ZhibiaoContent;
    QList<double> list_ZhibiaoGrade;

    QSqlQuery queryTmp(m_dbConnect);

    QString strTmp = "select description,nodevalue,result from 网络架构指标树 where (parentid=1 and atomic=1)";
    queryTmp.exec(strTmp);
    queryTmp.next();
    while(queryTmp.isValid())
    {
        if(queryTmp.value(2).toInt() != -1)
        {
            list_ZhibiaoContent.push_back(queryTmp.value(0).toString());
            internetFinalRes+=queryTmp.value(1).toDouble()*queryTmp.value(2).toInt();
            qDebug()<<"父节点"<<queryTmp.value(1).toDouble()*queryTmp.value(2).toInt();
            list_ZhibiaoGrade.push_back(queryTmp.value(2).toDouble());
        }
        else//没有评估完所有的指标项
            finished=false;
        queryTmp.next();
    }

    strTmp = "select nodeid,description,nodevalue from 网络架构指标树 where (parentid=1 and atomic=0)";
    queryTmp.exec(strTmp);
    queryTmp.next();

    while(queryTmp.isValid())
    {
        double parResHasChild=0;//有子节点的原子指标的值
        bool childFinished=true;//这个原子指标对应的子指标全部评估完
        QString childIndex="select nodevalue,result from 网络架构指标树 where parentid="+queryTmp.value(0).toString();
        QSqlQuery queryChildIndex;
        queryChildIndex.exec(childIndex);
        queryChildIndex.next();
        while(queryChildIndex.isValid())
        {
            if(-1==queryChildIndex.value(1).toInt())
            {
                childFinished=false;
                finished=false;
                break;//只要有一个子指标没有评估，则此原子指标没有值
            }
            else
                parResHasChild += queryChildIndex.value(0).toDouble() * queryChildIndex.value(1).toInt();
            queryChildIndex.next();
        }
        qDebug()<<"没有乘以权重"<<parResHasChild;
        if(childFinished)
        {
            qDebug()<<"完成所有评估指标";
            list_ZhibiaoContent.push_back(queryTmp.value(1).toString());
            qDebug()<<"原子指标名称"<<queryTmp.value(1).toString();
            list_ZhibiaoGrade.push_back(parResHasChild*queryTmp.value(2).toDouble());
            qDebug()<<"原子指标fenshu"<<parResHasChild*queryTmp.value(2).toDouble();
            internetFinalRes+=parResHasChild*queryTmp.value(2).toDouble();
        }
        queryTmp.next();
    }

    m_pFinalResultShow->BarDisplay(list_ZhibiaoContent, list_ZhibiaoGrade, "网络架构评估结果", ui->graphicsView);
    if(!finished)//没有评估完所有的指标项
        QMessageBox::information(NULL, QString("错误"), "请完成所有评估内容再查看评估结果！");
        //ui->label_finalResult->setText("请完成所有评估内容再查看评估结果！");
    else
        ui->label_finalResult->setText(QString::number(internetFinalRes));

    //for(int i=0;i<list_ZhibiaoGrade.size();i++)
        //qDebug()<<"指标分数值是"<<list_ZhibiaoGrade[i];
}*/


//显示系统所需要图片
void InternetAccess::ShowPicture(QImage *imageTuopu)
{
    m_pInternetGraphviz->InternetGraphviz(GetInfoForMainSystem_TuopuGraphviz(),true);

    imageTuopu = new QImage(tuopuImagePath);

//    QImage newImage1 = imageTuopu->scaledToWidth(ui->label_tuoputu_1->width(), Qt::SmoothTransformation);//调整图片以适应窗口大小
//    ui->label_tuoputu_1->setPixmap(QPixmap::fromImage(newImage1));

//    imageTuopu = new QImage(InternetImagePath);
//    QImage newImage2 = imageTuopu->scaledToWidth(ui->label_tuoputu_2->width(), Qt::SmoothTransformation);//调整图片以适应窗口大小
//    ui->label_tuoputu_2->setPixmap(QPixmap::fromImage(newImage2));

//    QImage newImage3 = imageTuopu->scaledToWidth(ui->label_tuoputu_3->width(), Qt::SmoothTransformation);//调整图片以适应窗口大小
//    ui->label_tuoputu_3->setPixmap(QPixmap::fromImage(newImage3));

    /*QPixmap pixmap0(liuchengImagePath);
    ui->label_liuchengtu->setPixmap(pixmap0);
    ui->label_liuchengtu->setScaledContents(true);*/
    ui->frame_liuchengtu->setObjectName("myframe");
    ui->frame_liuchengtu-> resize( 200 , 400 );
    ui->frame_liuchengtu-> setStyleSheet( " QFrame#myframe{border-image:url(D:/Program Files (x86)/X_Ship/rc/internetIndex.bmp)} "  );
    ui->frame_liuchengtu-> show();

    QPixmap pixmap1(quyuImagePath);
    ui->label_quyu->setPixmap(pixmap1);
    ui->label_quyu->setScaledContents(true);

    QPixmap pixmap2(kekuozhanImagePath);
    ui->label_kekuozhan->setPixmap(pixmap2);
    ui->label_kekuozhan->setScaledContents(true);

    QPixmap pixmap3(bianjieImagePath);
    ui->label_bianjie->setPixmap(pixmap3);
    //ui->label_bianjie->setScaledContents(true);

    QPixmap pixmap(zaibeiImagePath);
  //QPixmap fitpixmap=pixmap.scaled(ui->label_zaibei->width(),ui->label_zaibei->height(), Qt::KeepAspectRatio);
  //QPixmap fitpixmap=pixmap.scaled(ui->label_zaibei->width(),ui->label_zaibei->height(),Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->label_zaibei->setPixmap(pixmap);
    ui->label_zaibei->setScaledContents(true);
  //ui->label_zaibei->resize(ui->label_zaibei->width(),ui->label_zaibei->height());
  //QPixmap fitPixmap = pixmap.scaled(width(),height(), Qt::KeepAspectRatio);



}
//help文档
/*void InternetAccess::on_pushButton_2_clicked()
{
    QProcess *mp_helpProcess = new QProcess(this);
    QStringList argument("D:\\719\\2017_06_10_Total\\WinRAR.chm");
    mp_helpProcess->start("hh.exe",argument);//chm格式可用windows自带的hh.exe进行打开
}*/
//把用模型计算出来的结果存入原子指标库对应的结果中
void InternetAccess::writeIndexResultToDB(QString strTbableName,QString indexName,float indexResult)
{
    bool bWrite = true;
    QString sqlquery;
    QList<unsigned int> find;
    find = index_result.keys(indexName);
    QSqlQuery queryTmp(m_dbConnectPro);

    sqlquery = QString("UPDATE 网络架构指标树 SET result = '%1' WHERE description ='%2'").arg(indexResult).arg(indexName);
    bWrite = queryTmp.exec(sqlquery);
    if(bWrite == false)
        qDebug()<<"原子指标计算结果存入数据库出错"<<queryTmp.lastError();
}

//拓扑结构安全性评估
void InternetAccess::on_pushButton_tuopu_clicked()
{
    change_window();
    ui->pushButton_tuopu->setStyleSheet("color:#ffa02f;");
    ui->stackedWidget->setCurrentIndex(0);
    current_page=0;

}

//冗余性评估
void InternetAccess::on_pushButton_rongyu_clicked()
{   
    change_window();
    ui->pushButton_rongyu->setStyleSheet("color:#ffa02f;");
    ui->stackedWidget->setCurrentIndex(1);
    current_page=1;
}

//故障传播性评估
void InternetAccess::on_pushButton_guzhang_clicked()
{   
    change_window();
    ui->pushButton_guzhang->setStyleSheet("color:#ffa02f;");
    ui->stackedWidget->setCurrentIndex(2);
    current_page=2;
}
//当切换stackWidget时，Pushbutton的变化
void InternetAccess::change_window()
{
    switch(current_page)
    {
    case 0:
        ui->pushButton_tuopu->setStyleSheet("");
        break;
    case 1:
        ui->pushButton_rongyu->setStyleSheet("");
        break;
    case 2:
        ui->pushButton_guzhang->setStyleSheet("");
        break;
    }
}
//查看评估原理
void InternetAccess::on_pushButton_readQuestionnaire_clicked()
{
    ui->pushButton_readQuestionnaire->setStyleSheet("color:#ffa02f;");
    ui->pushButton_checkQAnswer->setStyleSheet("");
    ui->pushButton_writeQuestionnaire->setStyleSheet("");
    QMessageBox::information(NULL, QString("提示"), "在填写调查问卷之前可在左下方查看评估原理！");
}
//填写调查问卷
void InternetAccess::on_pushButton_writeQuestionnaire_clicked()
{
    ui->pushButton_writeQuestionnaire->setStyleSheet("color:#ffa02f;");
    ui->pushButton_checkQAnswer->setStyleSheet("");
    ui->pushButton_readQuestionnaire->setStyleSheet("");
    QString show=getIndexNameNotFound();
    //如果show为空，则可以打开调查问卷，否则，在界面显示show中的字符串
    if(""==show)
    {
        QString strWorkingDir="D:\\Program Files (x86)\\X_Ship";
        QDir::setCurrent(strWorkingDir);
        QString strQuestionnaireProgramPath=".\\QuestionnaireSystemUser.exe";
        QStringList strQuestionnaireCallArguments;
        strQuestionnaireCallArguments<<".\\WeifenLuo.WinFormsUI.Docking.dll";
        QProcess *myProcess=new QProcess(this);
        myProcess->start(strQuestionnaireProgramPath, strQuestionnaireCallArguments);
        myProcess->waitForFinished(-1);//可以保证把计算的结果存到原子指标计算结果中
        QuestionnaireAnalysis();//是否计算完结果并保存到相应的数据库中

    }
    else
    {
        //自己在界面显示
        QString s1="以下原子指标没有对应上题目:";
        s1=s1+"\n";
        QString s2="请以管理员身份登陆，针对未对应原子指标的问卷增加问卷题目！";
        QMessageBox::information(NULL, QString("错误"), s1+show+s2);
    }
}
//查看调查问卷的结果
void InternetAccess::on_pushButton_checkQAnswer_clicked()
{
    ui->pushButton_writeQuestionnaire->setStyleSheet("");
    ui->pushButton_checkQAnswer->setStyleSheet("color:#ffa02f;");
    ui->pushButton_readQuestionnaire->setStyleSheet("");
    ui->tableWidget_QResult->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_QResult->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空
    QSqlQuery queryTmp(m_dbConnectPro);
    QString strIndexOfInternet[4];
    strIndexOfInternet[0]="select nodeid,atomic,result from 网络架构指标树 where description='可扩展性'";
    strIndexOfInternet[1]="select nodeid,atomic,result from 网络架构指标树 where description='区域划分合理性'";
    strIndexOfInternet[2]="select nodeid,atomic,result from 网络架构指标树 where description='灾备性'";
    strIndexOfInternet[3]= "select nodeid,atomic,result from 网络架构指标树 where description='边界安全性'";
    bool finishQ[4];
    for(int i=0;i<4;i++)
        finishQ[i]=0;
    for(int i=0;i<4;i++)
    {
        queryTmp.exec(strIndexOfInternet[i]);
        queryTmp.next();
        //判断是否为原子指标
        if(0==queryTmp.value(1).toInt())//不是原子指标
        {
            QString strIndex="select nodevalue,result from 网络架构指标树 where parentid="+queryTmp.value(0).toString();
            queryTmp.exec(strIndex);
            queryTmp.next();
            float result=0;
            while(queryTmp.isValid())
            {
                result += queryTmp.value(0).toFloat()*queryTmp.value(1).toFloat();
                if(queryTmp.next());
                else
                    break;
            }
            finishQ[i]=showQuestionnaireRes(0,i,QString::number(result));
        }
        else
        {
            QString result=queryTmp.value(2).toString();
            finishQ[i]=showQuestionnaireRes(0,i,result);
        }
    }
    ui->tableWidget_QResult->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bool finishAll=true;
    for(int i=0;i<4;i++)
    {
        if(finishQ[i]==false)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem(QString("未评估"));
            ui->tableWidget_QResult->setItem(0,i,newItem);
            newItem->setTextColor(Qt::red);
            newItem->setTextAlignment (Qt::AlignCenter);
        }
        finishAll=finishAll&&finishQ[i];
    }
    /*if(!finishAll)
        QMessageBox::information(NULL, QString("错误"), "请完成所有问卷再查看评估结果！");*/
}
//把问卷调查的作答结果显示到对应的表格中
bool InternetAccess::showQuestionnaireRes(int row,int col,QString result)
{
    //qDebug()<<"进入啦";
    //qDebug()<<result;
    bool flag=false;//在问卷结果中能否找到对应的原子指标项，如果没有找到，则表示未答题
    //ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:black;}");
    if(result!=NULL && result!="-1")
    {
        flag=true;
        QTableWidgetItem *newItem = new QTableWidgetItem(QString(result));
        ui->tableWidget_QResult->setItem(row,col,newItem);
        newItem->setTextAlignment (Qt::AlignCenter);
    }
    if(flag==true)
    {
        float res=result.toFloat();
        if(res>=0 && res<34)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem(QString("低"));
            ui->tableWidget_QResult->setItem(row+1,col,newItem);
            newItem->setTextColor(Qt::red);
            newItem->setTextAlignment (Qt::AlignCenter);
        }
        else if(res>=34 && res<67)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem(QString("中"));
            ui->tableWidget_QResult->setItem(row+1,col,newItem);
            newItem->setTextAlignment (Qt::AlignCenter);
        }
        else if(res>=67 && res<=100)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem(QString("高"));
            ui->tableWidget_QResult->setItem(row+1,col,newItem);
            newItem->setTextColor(Qt::green);
            newItem->setTextAlignment (Qt::AlignCenter);
        }
        else
            QMessageBox::information(NULL, QString("错误"), "问卷解析模块出错！");
    }
    return flag;
}

//查看可扩展评估原理
void InternetAccess::on_pushButton_kekuozhan_clicked()
{
    ui->pushButton_kekuozhan->setStyleSheet("color:#ffa02f;");
    ui->pushButton_quyu->setStyleSheet("");
    ui->pushButton_bianjie->setStyleSheet("");
    ui->pushButton_zaibei->setStyleSheet("");
    ui->stackedWidget_wenjuan->setCurrentIndex(0);
}

//查看区域划分合理性评估原理
void InternetAccess::on_pushButton_quyu_clicked()
{
    ui->pushButton_quyu->setStyleSheet("color:#ffa02f;");
    ui->pushButton_kekuozhan->setStyleSheet("");
    ui->pushButton_bianjie->setStyleSheet("");
    ui->pushButton_zaibei->setStyleSheet("");
    ui->stackedWidget_wenjuan->setCurrentIndex(1);
}
//查看灾备性评估原理
void InternetAccess::on_pushButton_zaibei_clicked()
{
    ui->pushButton_zaibei->setStyleSheet("color:#ffa02f;");
    ui->pushButton_kekuozhan->setStyleSheet("");
    ui->pushButton_bianjie->setStyleSheet("");
    ui->pushButton_quyu->setStyleSheet("");
    ui->stackedWidget_wenjuan->setCurrentIndex(2);
}
//查看边界安全性评估原理
void InternetAccess::on_pushButton_bianjie_clicked()
{
    ui->pushButton_bianjie->setStyleSheet("color:#ffa02f;");
    ui->pushButton_kekuozhan->setStyleSheet("");
    ui->pushButton_zaibei->setStyleSheet("");
    ui->pushButton_quyu->setStyleSheet("");
    ui->stackedWidget_wenjuan->setCurrentIndex(3);
}
//查看评估结果界面
void InternetAccess::on_tabWidget_tabBarClicked(int index)
{
    if(0!=index)
        return;
    low_score_index();
    ui->suggestion->setReadOnly(true);
    ui->suggestion->setTextColor("white");
    double internetFinalRes=0; //存储最后的评估结果
    bool finished=true;//检查原子指标是否有评估结果
    QList<QString> list_ZhibiaoContent;
    QList<double> list_ZhibiaoGrade;
    QSqlQuery queryTmp(m_dbConnectPro);
    QString strTmp = "select description,nodevalue,result from 网络架构指标树 where (parentid=1 and atomic=1)";
    queryTmp.exec(strTmp);
    queryTmp.next();
    while(queryTmp.isValid())
    {
        if(queryTmp.value(2).toInt() != -1)
        {
            list_ZhibiaoContent.push_back(queryTmp.value(0).toString());
            internetFinalRes+=queryTmp.value(1).toDouble()*queryTmp.value(2).toInt();
            list_ZhibiaoGrade.push_back(queryTmp.value(2).toDouble());
        }
        else//没有评估完所有的指标项
        {
            finished=false;
            list_ZhibiaoContent.push_back(queryTmp.value(0).toString());
            internetFinalRes=0;
            list_ZhibiaoGrade.push_back(internetFinalRes);
        }
        queryTmp.next();
    }

    strTmp = "select nodeid,description,nodevalue from 网络架构指标树 where (parentid=1 and atomic=0)";
    queryTmp.exec(strTmp);
    queryTmp.next();

    while(queryTmp.isValid())
    {
        double parResHasChild=0;//有子节点的原子指标的值
        bool childFinished=true;//这个原子指标对应的子指标全部评估完
        QString childIndex="select nodevalue,result from 网络架构指标树 where parentid="+queryTmp.value(0).toString();
        QSqlQuery queryChildIndex;
        queryChildIndex.exec(childIndex);
        queryChildIndex.next();
        while(queryChildIndex.isValid())
        {
            if(-1==queryChildIndex.value(1).toInt())
            {
                childFinished=false;
                finished=false;
                break;//只要有一个子指标没有评估，则此原子指标没有值
            }
            else
                parResHasChild += queryChildIndex.value(0).toDouble() * queryChildIndex.value(1).toInt();
            queryChildIndex.next();
        }
        if(childFinished)
        {
            list_ZhibiaoContent.push_back(queryTmp.value(1).toString());
            list_ZhibiaoGrade.push_back(parResHasChild);
            internetFinalRes+=parResHasChild*queryTmp.value(2).toDouble();

            //把由多个原子指标构成的指标项的分数值存入数据库中
            bool bWrite = true;
            QString sqlquery;
            QString updateTableName="网络架构指标树";

            QSqlQuery queryTemp(m_dbConnectPro);
            sqlquery = QString("UPDATE " + updateTableName + " SET result = %1 WHERE nodeid = %2 ").arg(parResHasChild).arg(queryTmp.value(0).toString());
            bWrite = queryTemp.exec(sqlquery);
            if(bWrite == false)
                qDebug()<<"有子节点的指标问卷结果存入数据库出错！"<<queryTmp.lastError();
        }
        else
        {
            list_ZhibiaoContent.push_back(queryTmp.value(1).toString());
            list_ZhibiaoGrade.push_back(0);
        }
        queryTmp.next();
    }

    m_pFinalResultShow->BarDisplay(list_ZhibiaoContent, list_ZhibiaoGrade, "网络架构评估结果", ui->graphicsView);

    //画雷达图
    QString radarName="网络架构评估结果";
    m_radarPicture.PolarChartDisplay(list_ZhibiaoContent, list_ZhibiaoGrade,radarName, ui->PolarChart);
    if(!finished&&index==0)//没有评估完所有的指标项
    {
        ui->label_finalResult->setText("尚未完成所有评估！");
        ui->label_finalResult->setStyleSheet("color:red;");
        //QMessageBox::information(NULL, QString("错误"), "请完成所有评估内容再查看评估结果！");
    }
    else
        ui->label_finalResult->setText(QString::number(internetFinalRes));
}

//架构评估最终评估结果，单击分数很低的项目，打开对应的txt文件，并显示到文本框中
void InternetAccess::on_tableWidget_selres_itemClicked(QTableWidgetItem *item)
{
    QString file_name = suggestionPath+QString(item->text())+".txt";
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
//把低于60分的原子指标显示出来
void InternetAccess::low_score_index()
{
    int currentRow=0;
    ui->tableWidget_selres->setRowCount(0);
    ui->tableWidget_selres->setColumnCount(3);
    ui->tableWidget_selres->insertRow(currentRow);
    int table_location=0;
    QSqlQuery queryTmp(m_dbConnectPro);
    QString strTmp = "select nodeid,description,result from 网络架构指标树 where atomic=1";
    queryTmp.exec(strTmp);
    queryTmp.next();
    while(queryTmp.isValid())
    {
        if(queryTmp.value(2).toFloat()<60)
        {
            QString item_name=queryTmp.value(1).toString();
            ui->tableWidget_selres->setItem(currentRow,table_location,new QTableWidgetItem(item_name));
            ui->tableWidget_selres->item(currentRow,table_location)->setTextAlignment(Qt::AlignCenter); //设置文本内容居中
            table_location++;
        }
        if(table_location==3)
        {
            table_location = 0;
            currentRow++;
            ui->tableWidget_selres->insertRow(currentRow);
        }
        queryTmp.next();

    }
    ui->tableWidget_selres->verticalHeader()->setVisible(false); //隐藏行序号
    ui->tableWidget_selres->horizontalHeader()->setVisible(false); //隐藏列序号
    ui->tableWidget_selres->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_selres->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空
}
