#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "message_assess/messageassess.h"
#include "wuli_assess/wuli.h"
#include "index/admin_index_conf.h"
#include "index/index.h"
#include "workflow/wflow.h"
#include "newproject/history.h"
#include "newproject/newproject.h"
#include <QMessageBox>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <unordered_map>

System_Informa *sys_inf=NULL;
QMap<QString,uint> subsystem;
QMap<QString,uint> domain;
static QList<QString> device_name[3];
const QString suggestionPath("D:\\Program Files (x86)\\X_Ship\\DB_51aspx\\jiagou_assess_KnowledgeBase\\");
const QString messageSuggestionPath= "D:/Program Files (x86)/X_Ship/DB_51aspx/message_assess_KnowledgeBase";
const QString wuliSuggestionPath = "D:/Program Files (x86)/X_Ship/DB_51aspx/wuli_assess_KnowledgeBase";
extern int active_module;//一共评估几个构件
extern int active_days;
extern int jihuo;
extern QSqlDatabase m_dbConnect;////数据库修改  基准数据库

extern QList<uint> messagegroup;//信息流评估中的信道容量
extern QList<uint> devicegroup;
extern QString creatName;
extern QList<uint> channelgroup;
extern Index* m_message_index;
extern QList<Index*> index_select;
extern QList<uint> message_index;
extern QSqlDatabase m_dbConnectPro;//工程文件下的数据库
extern WorkflowCom *pCurrrentWorkflow;//工作流中的数据

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lic = new Licence;
    Actives = lic->Read_Date_Module("Module");//此一次模块shi使能条件为：新建项目勾选的信息，此处再加软件激活信息使能模块限制
    jihuo = Actives;
    this->setWindowOpacity(1);//设置软件界面透明度，数值大，透明度低
    this->menuBar()->setStyleSheet("QMenuBar{ border-bottom:1px solid #ffa02f;}");
    this->ui->line->setStyleSheet("QLine{background: #ffa02f;}");


    setWindowTitle("X船信息安全评测软件");
    QIcon icon("./rc1/main32.ico");
    setWindowIcon(icon);
    setWindowState(Qt::WindowMaximized);//初始窗口最大化

    ui->bwuli_assess->setEnabled(false);//就是这句能够实现透明
    ui->bnet_assess->setEnabled(false);
    ui->bmessage_assess->setEnabled(false);
    ui->bworkflow->setEnabled(false);
    ui->btaishi->setEnabled(false);
    ui->bproject_info->setEnabled(false);

    connect(ui->newProject,SIGNAL(triggered()),this,SLOT(new_Project()));
    connect(ui->openProject,SIGNAL(triggered()),this,SLOT(open_project()));

    ui->centralWidget->setAutoFillBackground(true);
    QPalette main_pale=ui->centralWidget->palette();
    main_pale.setColor(QPalette::Window,QColor(245,245,245));//主窗口背景色，浅白色240,240,240  61,145,64
    ui->centralWidget->setPalette(main_pale);
    lic = new Licence;
}

MainWindow::~MainWindow()
{
    if(wuli_assess!=NULL) delete wuli_assess;
    if(m_poInternetDlg!=NULL) delete m_poInternetDlg;
    if(mess_assess!=NULL) delete mess_assess;
    if(m_poWFlowDlg!=NULL) delete m_poWFlowDlg;
    if(situation_simulation!=NULL) delete situation_simulation;
    if(project_information!=NULL) delete project_information;
    delete ui;
}
//主界面关闭的那个“X”按键
void MainWindow::closeEvent(QCloseEvent *event)
{
   on_action_createRecord_triggered();
   QApplication::exit(0);
}


//void MainWindow::on_open_file_triggered()
//{
//    //8.13日修改，自动加载xml文件；   后续要去掉第二行，保留第一行
//    //QString path=QFileDialog::getOpenFileName(this,tr("open File"),".",tr("XML Files(*.xml)"));
//    QString path="D:/Program Files (x86)/X_Ship/2017.06.23.xml";



//    if(!path.isEmpty())
//    {
//        QFile cXmlFile(path);
//        if(!cXmlFile.open(QIODevice::ReadOnly|QIODevice::Text))
//        {
//            QMessageBox::warning(this,tr("Read File"),
//                                 tr("cannot open file:\n%1").arg(path));
//            return;
//        }
//        QTextStream floStream(&cXmlFile);
//        QTextCodec *codec=QTextCodec::codecForName("UTF-8");
//        floStream.setCodec(codec);
//        QString xmlDataStr = floStream.readAll();
//        QDomDocument doc;
//        doc.setContent(xmlDataStr);
//        cXmlFile.close();
//        delete sys_inf;
//        sys_inf=new System_Informa(doc);

//        //sys_inf->SaveToXml("C:/719/2017_06_10_Total/new.xml");

//        //初始化子界面
//        initial();


//    }
//    else
//    {
//        QMessageBox::warning(this,tr("path"),tr("you did not select any file"));
//    }
//}

void MainWindow::initial()
{
    if(mess_assess!=NULL&&wuli_assess!=NULL&&m_poWFlowDlg!=NULL&&m_poInternetDlg!=NULL&&situation_simulation!=NULL&&project_information!=NULL)
    {
        delete mess_assess;
        delete wuli_assess;
        delete m_poWFlowDlg;
        delete m_poInternetDlg;
        delete situation_simulation;
        delete  project_information;
    }
    mess_assess=new messageassess(this);
    mess_assess->setVisible(0);
    wuli_assess=new wuli(this);
    wuli_assess->setVisible(0);
    m_poWFlowDlg = new WFlow(this);
    m_poWFlowDlg ->setVisible(0);
    m_poInternetDlg = new InternetAccess(this);
    m_poInternetDlg->setVisible(0);    
    situation_simulation = new Situation(m_poWFlowDlg,this);
    situation_simulation->setVisible(0);
    project_information = new project_info(this);
    project_information->setVisible(0);

        ui->bwuli_assess->setEnabled(false);//5个模块激活
       ui->bnet_assess->setEnabled(false);
       ui->bmessage_assess->setEnabled(false);
       ui->bworkflow->setEnabled(false);
       ui->btaishi->setEnabled(false);


    ui->bproject_info->setChecked(true);
    on_bproject_info_clicked();
    qDebug()<<"initial读的激活数"<<active_module;

    if(active_module==1)
    {
        workModuleJudge();
    }

    else if(active_module==2)
    {
        workModuleJudge();
    }
    else if(active_module==3)
    {
        workModuleJudge();
    }
    else if(active_module==4)
    {
        workModuleJudge();
    }
    else if(active_module==5)
    {
        workModuleJudge();
    }
    else if(active_module==6)
    {
        workModuleJudge();
    }
    else
    {}
    ui->bproject_info->setEnabled(true);
}


void MainWindow::workModuleJudge()
{
//    qDebug()<<"实际激活"<< Actives<<" 用户仿真"<<active_module;
//    if(Actives>active_module)
//        Actives=active_module; //如果用户实际仿真模块比激活模块少
    if(active_module==1)
    {
        ui->bwuli_assess->setEnabled(true);//1个模块激活
        QMessageBox::information(0, tr("提示"), tr(" 界面初始化完成!本次项目仿真激活模块为：\r\n\r\n物理属性评估"));
     }
    if(active_module==2)
    {
        ui->bwuli_assess->setEnabled(true);//2个模块激活
        ui->bnet_assess->setEnabled(true);
        QMessageBox::information(0, tr("提示"), tr(" 初始化完成!本次仿真模块为：\r\n\r\n物理属性评估+网络架构评估"));
    }
    if(active_module==3)
    {
        ui->bwuli_assess->setEnabled(true);//2个模块激活
        ui->bmessage_assess->setEnabled(true);
        QMessageBox::information(0, tr("提示"), tr(" 初始化完成!本次仿真模块为：\r\n\r\n物理属性评估+信息流评估"));
     }
    if(active_module==4)
    {
        ui->bwuli_assess->setEnabled(true);//4个模块激活
        ui->bnet_assess->setEnabled(true);
        ui->bmessage_assess->setEnabled(true);
        QMessageBox::information(0, tr("提示"), tr(" 初始化完成!本次仿真激活模块为：\r\n\r\n物理属性评估+网络架构评估+信息流评估"));
    }
    if(active_module==5)
    {
        ui->bwuli_assess->setEnabled(true);//5个模块激活
        ui->bnet_assess->setEnabled(true);
        ui->bmessage_assess->setEnabled(true);
        ui->bworkflow->setEnabled(true);
        QMessageBox::information(0, tr("提示"), tr(" 初始化完成!本次仿真模块为：\r\n\r\n物理属性评估+网络架构评估+信息流评估+工作流评估"));
    }
    if(active_module==6)
    {
        ui->bwuli_assess->setEnabled(true);//5个模块激活
        ui->bnet_assess->setEnabled(true);
        ui->bmessage_assess->setEnabled(true);
        ui->bworkflow->setEnabled(true);
        ui->btaishi->setEnabled(true);
        QMessageBox::information(0, tr("提示"), tr(" 初始化完成!本次仿真模块为：\r\n\r\n物理属性评估+网络架构评估+信息流评估+工作流评估+态势仿真"));
    }
}

//下面五个函数分别对应主界面不同评估按钮的槽函数
void MainWindow::on_bwuli_assess_clicked()
{
    if(wuli_assess==NULL) return;
    change_window();
    ui->horizontalLayout->removeItem(ui->horizontalSpacer_2);
    wuli_assess->setVisible(1);
    ui->horizontalLayout->addWidget(wuli_assess);
    ui->lwuli_assess->setStyleSheet("color:#2E8B57");
    ui->bwuli_assess->setStyleSheet("QToolButton:checked{border-style:inset;background-color:#2E8B57;border-radius: 0px}");
    num=1;
}

void MainWindow::on_bnet_assess_clicked()
{
    if(m_poInternetDlg==NULL) return;
    change_window();
    ui->horizontalLayout->removeItem(ui->horizontalSpacer_2);
    m_poInternetDlg->setVisible(1);
    ui->horizontalLayout->addWidget(m_poInternetDlg);
    ui->lnet_assess->setStyleSheet("color:#2E8B57");
    ui->bnet_assess->setStyleSheet("QToolButton:checked{border-style:inset;background-color:#2E8B57;border-radius: 0px}");

    num=2;
}

void MainWindow::on_bmessage_assess_clicked()
{
    if(mess_assess==NULL) return;
    change_window();
    ui->horizontalLayout->removeItem(ui->horizontalSpacer_2);
    mess_assess->setVisible(1);
    ui->horizontalLayout->addWidget(mess_assess);
    ui->lmessage_assess->setStyleSheet("color:#2E8B57");
    ui->bmessage_assess->setStyleSheet("QToolButton:checked{border-style:inset;background-color:#2E8B57;border-radius: 0px}");

    num=3;
}

void MainWindow::on_bworkflow_clicked()
{
    if(m_poWFlowDlg==NULL) return;
    m_poWFlowDlg->Updateinforma();
    change_window();
    ui->horizontalLayout->removeItem(ui->horizontalSpacer_2);
    m_poWFlowDlg->setVisible(1);
    ui->horizontalLayout->addWidget(m_poWFlowDlg);
    ui->lworkflow->setStyleSheet("color:#2E8B57");
    ui->bworkflow->setStyleSheet("QToolButton:checked{border-style:inset;background-color:#2E8B57;border-radius: 0px}");
    num=4;
}

void MainWindow::on_btaishi_clicked()
{
    if(situation_simulation==NULL) return;
    change_window();
    ui->horizontalLayout->removeItem(ui->horizontalSpacer_2);
    situation_simulation->setVisible(1);
    ui->horizontalLayout->addWidget(situation_simulation);
    ui->ltaishi->setStyleSheet("color:#2E8B57");
    ui->btaishi->setStyleSheet("QToolButton:checked{border-style:inset;background-color:#2E8B57;border-radius: 0px}");
    situation_simulation->my_updata();
    num=5;
}
void MainWindow::on_bproject_info_clicked()
{

    if(project_information==NULL) return;
    change_window();
    ui->horizontalLayout->removeItem(ui->horizontalSpacer_2);
    project_information->setVisible(1);
    ui->horizontalLayout->addWidget(project_information);
    ui->lproject_info->setStyleSheet("color:#2E8B57");
    ui->bproject_info->setStyleSheet("QToolButton:checked{border-style:inset;background-color:#2E8B57;border-radius: 0px}");
    num=6;
}

//切换评估内容时，将界面保持到初始状态
void MainWindow::change_window()
{
    if(num!=0)
    {
        if(num==1)
        {
            wuli_assess->setVisible(0);
            ui->lwuli_assess->setStyleSheet("");
            ui->bwuli_assess->setStyleSheet("");
        }
        else if(num==2)
        {
            m_poInternetDlg->setVisible(0);
            ui->lnet_assess->setStyleSheet("");
            ui->bnet_assess->setStyleSheet("");
        }
        else if(num==3)
        {
            mess_assess->setVisible(0);
            ui->lmessage_assess->setStyleSheet("");
            ui->bmessage_assess->setStyleSheet("");

        }
        else if(num==4)
        {
            m_poWFlowDlg->setVisible(0);
            ui->lworkflow->setStyleSheet("");
            ui->bworkflow->setStyleSheet("");

        }
        else if(num==5)
        {
            situation_simulation->setVisible(0);
            ui->ltaishi->setStyleSheet("");
            ui->btaishi->setStyleSheet("");

        }
        else
        {
            project_information->setVisible(0);
            ui->lproject_info->setStyleSheet("");
            ui->bproject_info->setStyleSheet("");

        }
    }
}

//测试代码修改记录

//打开项目槽函数
void MainWindow::on_openProject_triggered()
{

    History *history= new History;
    //history->show(); //非模态，也可以用->open();
    history->setSizeGripEnabled(true);  //窗口大小可调整
    if(history->exec() == QDialog::Accepted)   //模态，阻塞窗口，直到QDialog窗口对象做出反馈
    {
        ProjectDatabaseConnect();//数据库修改
        QString save_path=history->xml_path+creatName+".xml";
        //active_module = history->modules;
        //qDebug()<<"测试历史"<<save_path<<history->modules;

        if(!save_path.isEmpty())
        {
            QFile cXmlFile(save_path);
            if(!cXmlFile.open(QIODevice::ReadOnly|QIODevice::Text))
            {
                QMessageBox::warning(this,tr("Read File"),tr("cannot open file:\n%1").arg(save_path));
                return;
            }
            QTextStream floStream(&cXmlFile);
            QTextCodec *codec=QTextCodec::codecForName("UTF-8");
            floStream.setCodec(codec);
            QString xmlDataStr = floStream.readAll();
            QDomDocument doc;
            doc.setContent(xmlDataStr);
            cXmlFile.close();
            delete sys_inf;
            sys_inf=new System_Informa(doc);
            //初始化子界面
            initial();
            //active_module = lic->Read_Date_Module("Module");
        }
        delete history;
    }
    else
        QMessageBox::information(0,"提示","未选择一项有效的工程记录");

}


void MainWindow::on_newProject_triggered()
{
    NewProject project;
    if(project.exec()==QDialog::Accepted)
    {
       ProjectDatabaseConnect(); //数据库修改

       QString save_path="D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles/"+creatName+"/"+creatName+".xml";
        if(!save_path.isEmpty())
        {
            QFile cXmlFile(save_path);
            if(!cXmlFile.open(QIODevice::ReadOnly|QIODevice::Text))
            {
                QMessageBox::warning(this,tr("Read File"),tr("cannot open file:\n%1").arg(save_path));
                return;
            }
            QTextStream floStream(&cXmlFile);
            QTextCodec *codec=QTextCodec::codecForName("UTF-8");
            floStream.setCodec(codec);
            QString xmlDataStr = floStream.readAll();
            QDomDocument doc;
            doc.setContent(xmlDataStr);
            cXmlFile.close();
            delete sys_inf;
            sys_inf=new System_Informa(doc);
    }
       //初始化子界面
       initial();
       //active_module = lic->Read_Date_Module("Module");
    }
   else
       return;
}


//工程文件目录下的数据库,此函数应当放在新建工程那里
void ProjectDatabaseConnect()    //数据库修改
{
    QSqlDatabase dbConnect;
    dbConnect = QSqlDatabase::addDatabase("QODBC");//采用的接口和连接名
    dbConnect.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};\
    FIL={MS Access};DBQ=D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles/"+creatName+"/DB_51aspx/"+"Lj_QuestionnaireSys.mdb");
    QString strUserPassword="123";
    QString strUserName="123";
    dbConnect.setUserName(strUserName);//设置登陆数据库的用户名
    dbConnect.setPassword(strUserPassword);//设置密码
    bool bOpen=dbConnect.open(strUserName,strUserPassword);
    if(!bOpen)
        QMessageBox::information(NULL, QString("数据库"), QString("工程路径下数据库打开失败"));
    else
        m_dbConnect = dbConnect;   //数据库修改
}

//数据库原子指标初始化，仅在新建工程的时候初始化
void resetIndex()
{
    QString sqlquery[7];
    QSqlQuery queryTmp(m_dbConnectPro);
    //因为网络架构评估结果存储在数据库中，因此网络架构指标树的内容不可以被清空
    sqlquery[0] = QString("UPDATE 网络架构指标树 SET result = '-1' ");
    sqlquery[1] = QString("UPDATE 服务器设备指标树 SET result = '-1'");
    sqlquery[2] = QString("UPDATE 控制设备指标树 SET result = '-1'");
    sqlquery[3] = QString("UPDATE 信息流指标树 SET result = '-1'");
    sqlquery[4] = QString("UPDATE 显控设备指标树 SET result = '-1'");
    sqlquery[5] = QString("UPDATE Lj_Result SET result_count = '0'");
    sqlquery[6] = QString("UPDATE Lj_Survey SET survey_Count = '0'");
    for(auto i=0;i<7;i++)
        queryTmp.exec(sqlquery[i]);
}


void MainWindow::on_close_project_triggered()
{
//    delete messageassess();
//    delete NewProject();
//    delete project_guide();
//    delete InternetAccess();
//    delete CAssessResult();
//    delete new_link();
//    delete new_message();
//    delete new_workflow_renwu();
//    delete new_workflow_yewu();
//    delete new_wuli();
//    delete history();

     on_action_createRecord_triggered();
    delete sys_inf;

    QApplication::closeAllWindows();
    //exit(0);
}


//打开帮助文档
void MainWindow::on_action_oenHelp_triggered()
{
    QProcess *mp_helpProcess = new QProcess(this);
    QStringList argument("D:\\Program Files (x86)\\X_Ship\\references.chm");
    mp_helpProcess->start("hh.exe",argument);//chm格式可用windows自带的hh.exe进行打开
}
//打开支撑文献
void MainWindow::on_action_findPDF_triggered()
{
    QString path = "D:\\Program Files (x86)\\X_Ship\\DB_51aspx\\PDF_Files";
    QProcess::startDetached("explorer "+path);//打开上面获取的目录
}
/*********************************** 函数的功能：生成评估报告pdf********************************************/
void MainWindow::on_action_createRecord_triggered()
{   
    QPrinter printer_text;
    printer_text.setOutputFormat(QPrinter::PdfFormat);
    printer_text.setOutputFileName("D:\\Program Files (x86)\\X_Ship\\DB_51aspx\\ProjectFiles\\"+creatName+"\\"+creatName+"评估报告.pdf");
    QPainter painter_text;
    painter_text.begin(&printer_text);
    QTextOption option(Qt::AlignLeft);//设置自动换行
    option.setFlags(QTextOption::AddSpaceForLineAndParagraphSeparators);
    option.setWrapMode(QTextOption::WordWrap);
    int pageHeight = 1050;//设置页面高度为1050，用来控制是否增加新页面
    int currentHeight = 0;//初始化当前的页面高度
    int indexCount = 0;
    QString PDFtext="",str_temp="";
    QRect rectangle;
    QRectF rectF;
    bool flag = true;
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
/***********************************物理部分生成PDF评估报告*****************************************************/
    /*if(active_module < 1)
    {
        painter_text.end();
        return;
    }*/
    /********************************物理部分每个设备指标分数表格************************************************/
    QMap<QString,QMap<QString,QString> > wuliInf = getWuliIndexInf();
    indexCount=wuliInf.count();//原子指标个数
    int jianxi=25;//行宽大小，间隙
    int beginX=2,beginY=90,endX=772;//设置表格的开始位置和结束位置点的坐标
    int columnDistance1=120,columnDistance2=70,columnDistance3=90;//设置列之间的宽度
    //表格的标题
    painter_text.setFont(QFont("Arial", 16, QFont::Bold));
    QRectF ff(beginX,beginY-70, endX-beginX, 40);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "物理属性安全性评估报告");

    painter_text.setFont(QFont("Arial", 14, QFont::Bold));
    ff.setRect(beginX,beginY-30, endX-beginX, 30);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "评估时间："+current_date);

    painter_text.setFont(QFont("Arial", 12, QFont::Bold));
    ff.setRect(beginX, beginY, columnDistance1, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"设备名称");
    ff.setRect(beginX+columnDistance1, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"保障性");
    ff.setRect(beginX+columnDistance1+columnDistance2, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"可扩展性");
    ff.setRect(beginX+columnDistance1+2*columnDistance2, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"可维护性");
    ff.setRect(beginX+columnDistance1+3*columnDistance2, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"可靠性");
    ff.setRect(beginX+columnDistance1+4*columnDistance2, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"安全性");
    ff.setRect(beginX+columnDistance1+5*columnDistance2, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"完整性");
    ff.setRect(beginX+columnDistance1+6*columnDistance2, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"易用性");
    ff.setRect(beginX+columnDistance1+7*columnDistance2, beginY, columnDistance3, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"环境适应性");
    ff.setRect(beginX+columnDistance1+7*columnDistance2+columnDistance3, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"总得分");
    currentHeight = beginY + jianxi;
    painter_text.setFont(QFont("",12));
    bool drawFlag = true;
    for(QMap<QString,QMap<QString,QString> >::iterator iter=wuliInf.begin();iter!=wuliInf.end();++iter)
    {
        //画横线
        painter_text.drawLine(beginX,currentHeight+jianxi,endX,currentHeight+jianxi);
        //画竖线
        for(int i=0,j=0;j<=7;++j)
        {
            painter_text.drawLine(beginX+i*columnDistance1+j*columnDistance2,currentHeight,beginX+i*columnDistance1+j*columnDistance2,currentHeight+jianxi);
            if(0 == i)
                ++i;
        }
        painter_text.drawLine(beginX+columnDistance1,currentHeight,beginX+columnDistance1,currentHeight+jianxi);
        painter_text.drawLine(beginX+columnDistance1+7*columnDistance2+columnDistance3,currentHeight,beginX+columnDistance1+7*columnDistance2+columnDistance3,currentHeight+jianxi);
        painter_text.drawLine(endX,currentHeight,endX,currentHeight+jianxi);

        ff.setRect(beginX, currentHeight, columnDistance1, jianxi);//设备名称
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,iter.key());
        ff.setRect(beginX+columnDistance1, currentHeight, columnDistance2, jianxi);//第1个原子指标，保障性
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,*iter.value().find("保障性"));
        ff.setRect(beginX+columnDistance1+columnDistance2, currentHeight, columnDistance2, jianxi);//第2个原子指标，可扩展性
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,*iter.value().find("可扩展性"));
        ff.setRect(beginX+columnDistance1+2*columnDistance2, currentHeight, columnDistance2, jianxi);//第3个原子指标，可维护性
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,*iter.value().find("可维护性"));
        ff.setRect(beginX+columnDistance1+3*columnDistance2, currentHeight, columnDistance2, jianxi);//第4个原子指标，可靠性
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,*iter.value().find("可靠性"));
        ff.setRect(beginX+columnDistance1+4*columnDistance2, currentHeight, columnDistance2, jianxi);//第5个原子指标，安全性
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,*iter.value().find("安全性"));
        ff.setRect(beginX+columnDistance1+5*columnDistance2, currentHeight, columnDistance2, jianxi);//第6个原子指标，完整性
        QMap<QString, QString>::const_iterator i = iter.value().find("完整性");
        if (i != iter.value().end() && i.key() == "完整性")
        {
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,*iter.value().find("完整性"));
        }
        else
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"无");
        ff.setRect(beginX+columnDistance1+6*columnDistance2, currentHeight, columnDistance2, jianxi);//第7个原子指标，易用性
        if(iter.value().find("易用性") != iter.value().end())//有这个原子指标
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,*iter.value().find("易用性"));
        else
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"无");
        ff.setRect(beginX+columnDistance1+7*columnDistance2, currentHeight, columnDistance3, jianxi);//第8个原子指标，环境适应性
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,*iter.value().find("环境适应性"));
        ff.setRect(beginX+columnDistance1+7*columnDistance2+columnDistance3, currentHeight, columnDistance2, jianxi);//总分
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,*iter.value().find("总分"));

        if(drawFlag)
        {
            drawFlag=false;
            //表格横线
            painter_text.drawLine(beginX,beginY,endX,beginY);
            painter_text.drawLine(beginX,beginY+jianxi,endX,beginY+jianxi);
            painter_text.drawLine(beginX,beginY+jianxi*2,endX,beginY+jianxi*2);//因为颜色是灰色，应当是黑色，所以重画了这条线
            //表格竖线
            for(int i=0,j=0;j<=7;++j)
            {
                painter_text.drawLine(beginX+i*columnDistance1+j*columnDistance2,beginY,beginX+i*columnDistance1+j*columnDistance2,beginY+jianxi);
                if(0 == i)
                    ++i;
            }
            painter_text.drawLine(beginX+columnDistance1,beginY,beginX+columnDistance1,beginY+jianxi);
            painter_text.drawLine(beginX+columnDistance1+7*columnDistance2+columnDistance3,beginY,beginX+columnDistance1+7*columnDistance2+columnDistance3,beginY+jianxi);
            painter_text.drawLine(endX,beginY,endX,beginY+jianxi);

            for(int i=0,j=0;j<=7;++j)
            {
                painter_text.drawLine(beginX+i*columnDistance1+j*columnDistance2,currentHeight,beginX+i*columnDistance1+j*columnDistance2,currentHeight+jianxi);
                if(0 == i)
                    ++i;
            }
            painter_text.drawLine(beginX+columnDistance1,currentHeight,beginX+columnDistance1,currentHeight+jianxi);
            painter_text.drawLine(beginX+columnDistance1+7*columnDistance2+columnDistance3,currentHeight,beginX+columnDistance1+7*columnDistance2+columnDistance3,currentHeight+jianxi);
            painter_text.drawLine(endX,currentHeight,endX,currentHeight+jianxi);
        }

        currentHeight += jianxi;
        if(currentHeight >= pageHeight)
        {
            printer_text.newPage(); currentHeight = 20;
            //表格在一页放不下，剩余部分在新的一页，需要在表格最上方补画一条线
            painter_text.drawLine(beginX,currentHeight,endX,currentHeight);
        }
    }
    rectangle.setRect(20,currentHeight,700,400);
    /********************************物理部分每个设备低于60分的原子指标************************************************/
    QMap<QString, QMap<QString, QString> > lowScoreWuli = getLowScoreIndexWuli();
    if(currentHeight >= pageHeight)
    {
        printer_text.newPage();
        currentHeight = 20;
        rectangle.setY(currentHeight);
    }
    for(QMap<QString,QMap<QString,QString> >::iterator iter = lowScoreWuli.begin();iter!=lowScoreWuli.end();++iter)
    {
        painter_text.setFont(QFont("Arial", 13, QFont::Bold));//把小标题加黑，字号加大
        str_temp = "设备名称：";
        PDFtext += '\n' + str_temp+iter.key() +",其原子指标中低于60分的有";
        rectF = painter_text.boundingRect(rectangle, PDFtext, option);
        if(currentHeight + rectF.height() > pageHeight)
        {    printer_text.newPage(); currentHeight = 20;rectangle.setY(currentHeight);rectF = painter_text.boundingRect(rectangle, PDFtext, option);}
        else
            rectangle.setY(currentHeight + rectF.height());
        painter_text.drawText(rectF,PDFtext,option);
        currentHeight = currentHeight + rectF.height();
        rectangle.setY(currentHeight);
        PDFtext.clear();
        painter_text.setFont(QFont("",12));//设置正文的字体大小
        for(QMap<QString,QString>::iterator iter_temp=iter.value().begin();iter_temp!=iter.value().end();++iter_temp)
            PDFtext += iter_temp.key()+iter_temp.value()+"分        ";
        rectF = painter_text.boundingRect(rectangle, PDFtext, option);
        if(currentHeight + rectF.height() > pageHeight)
        {    printer_text.newPage(); currentHeight = 20;rectangle.setY(currentHeight);rectF = painter_text.boundingRect(rectangle, PDFtext, option);}
        else
            rectangle.setY(currentHeight + rectF.height());
        painter_text.drawText(rectF,PDFtext,option);
        currentHeight = currentHeight + rectF.height();
        rectangle.setY(currentHeight);
        PDFtext.clear();
    }
    /********************************物理部分低于60分原子指标的安全性建议************************************************/
    PDFtext="";
    rectangle.setRect(20,currentHeight,700,400);
    QSet<QString> wuliSuggestion = getSecSuggestionWuli();
    flag = true;
    QString file_name;
    for(QSet<QString>::iterator iter=wuliSuggestion.begin();iter!=wuliSuggestion.end();++iter)
    {
        file_name = wuliSuggestionPath+"/"+*iter+".txt";
        QFile file(file_name);
        QString info = *iter +": 文件丢失！";
        if(!file.open(QFile::ReadOnly | QFile::Text))
        {
             QMessageBox::information(NULL, QString("错误"), info);
        }
        QTextStream in(&file);
        PDFtext += '\n' +*iter + "安全性建议" ;//添加小标题
        painter_text.setFont(QFont("Arial", 13, QFont::Bold));//把小标题加黑，字号加大
        rectF = painter_text.boundingRect(rectangle, PDFtext, option);
        if(currentHeight+rectF.height() > pageHeight)
        {   printer_text.newPage(); currentHeight=20;rectangle.setY(currentHeight);rectF = painter_text.boundingRect(rectangle, PDFtext, option);   }
        else
            rectangle.setY(currentHeight+rectF.height());
        painter_text.drawText(rectF,PDFtext,option);
        currentHeight = currentHeight + rectF.height();
        rectangle.setY(currentHeight);
        PDFtext.clear();//清除小标题内容
        painter_text.setFont(QFont("",12));//恢复安全性建议正文的字体设置
        flag=true;
        while(flag)
        {
            PDFtext += in.readLine();
            rectF = painter_text.boundingRect(rectangle, PDFtext, option);
            if(currentHeight+rectF.height() > pageHeight)
            {   printer_text.newPage(); currentHeight=20;rectangle.setY(currentHeight);rectF = painter_text.boundingRect(rectangle, PDFtext, option);   }
            else
                rectangle.setY(currentHeight+rectF.height());
            painter_text.drawText(rectF,PDFtext,option);
            PDFtext.clear();
            currentHeight = currentHeight + rectF.height();
            rectangle.setY(currentHeight);
            if(in.atEnd())
                flag = false;
        }
        file.close();
    }
/***********************************架构部分生成PDF评估报告*****************************************************/
    /*下面的功能是画架构评估结果的表格*/
    /*if(!(active_module >= 2 && active_module != 3))
    {
        painter_text.end();
        return;
    }*/
    //printer_text.newPage();
    QMap<QString,double> internetRes=getInternetResFromDB();
    indexCount=internetRes.count();//原子指标个数
    //控制网络架构PDF表格大小的数据
    jianxi=25;//行宽大小，间隙
    beginX=150,endX=600;//设置表格的开始位置和结束位置点的坐标
    columnDistance1=140,columnDistance2=80,columnDistance3=230;//设置列之间的宽度
    //表格的标题
    painter_text.setFont(QFont("Arial", 16, QFont::Bold));
    ff.setRect(beginX, currentHeight, endX-beginX, 40);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "架构安全性评估报告");
    currentHeight += 40;
    beginY=currentHeight;
    painter_text.setFont(QFont("Arial", 12, QFont::Bold));
    QRectF ff1(beginX, currentHeight, columnDistance1, jianxi);
    painter_text.drawText(ff1, Qt::AlignHCenter|Qt::AlignVCenter,"原子指标名称");
    QRectF ff2(beginX+columnDistance1, currentHeight, columnDistance2, jianxi);
    painter_text.drawText(ff2, Qt::AlignHCenter|Qt::AlignVCenter,"评估结果");
    QRectF ff3(beginX+columnDistance1+columnDistance2, currentHeight, columnDistance3, jianxi);
    painter_text.drawText(ff3, Qt::AlignHCenter|Qt::AlignVCenter,"评估时间");
    painter_text.setFont(QFont("",12));
    currentHeight += jianxi;

    //表格两条横线
    painter_text.drawLine(beginX,beginY,endX,beginY);
    painter_text.drawLine(beginX,beginY+jianxi,endX,beginY+jianxi);
    painter_text.drawLine(beginX,beginY+jianxi*2,endX,beginY+jianxi*2);//因为颜色是灰色，应当是黑色，所以重画了这条线
    //表格4条竖线
    painter_text.drawLine(beginX,beginY,beginX,beginY+jianxi);
    painter_text.drawLine(beginX+columnDistance1,beginY,beginX+columnDistance1,beginY+jianxi);
    painter_text.drawLine(beginX+columnDistance1+columnDistance2,beginY,beginX+columnDistance1+columnDistance2,beginY+jianxi);
    painter_text.drawLine(endX,beginY,endX,beginY+jianxi);

    for(QMap<QString,double>::iterator iterTemp=internetRes.begin();iterTemp!=internetRes.end();++iterTemp)
    {
        //画横线
        painter_text.drawLine(beginX,currentHeight+jianxi,endX,currentHeight+jianxi);
        //画竖线
        painter_text.drawLine(beginX,currentHeight,beginX,currentHeight+jianxi);
        painter_text.drawLine(beginX+columnDistance1,currentHeight,beginX+columnDistance1,currentHeight+jianxi);
        painter_text.drawLine(beginX+columnDistance1+columnDistance2,currentHeight,beginX+columnDistance1+columnDistance2,currentHeight+jianxi);
        painter_text.drawLine(endX,currentHeight,endX,currentHeight+jianxi);

        ff1.setRect(beginX, currentHeight, columnDistance1, jianxi);
        painter_text.drawText(ff1, Qt::AlignHCenter|Qt::AlignVCenter,iterTemp.key());
        ff2.setRect(beginX+columnDistance1, currentHeight, columnDistance2, jianxi);
        if(iterTemp.value()<0)
            painter_text.drawText(ff2, Qt::AlignHCenter|Qt::AlignVCenter,"未评估");
        else
            painter_text.drawText(ff2, Qt::AlignHCenter|Qt::AlignVCenter,QString("%10").arg(iterTemp.value()));
        ff3.setRect(beginX+columnDistance1+columnDistance2, currentHeight, columnDistance3, jianxi);
        painter_text.drawText(ff3, Qt::AlignHCenter|Qt::AlignVCenter,current_date);
        currentHeight += jianxi;
        if(currentHeight >= pageHeight)
        {
            printer_text.newPage();currentHeight=20;
            //表格在一页放不下，剩余部分在新的一页，需要在表格最上方补画一条线
            painter_text.drawLine(beginX,currentHeight,endX,currentHeight);
        }
    }
/*****************************下面的代码是把架构评估结果中原子指标评估结果低于60分的安全性建议转为pdf****************************************/
    PDFtext="";
    rectangle.setRect(20,currentHeight,700,400);
    QMap<QString,double> indexInternetRes=getIndexInternetResFromDB();
    flag = true;
    for(QMap<QString,double>::iterator iter=indexInternetRes.begin();iter!=indexInternetRes.end();++iter)
    {
        if(iter.value()<60 && iter.value()>= 0)//显示低于60分的原子指标，跟架构部分显示的内容保持一致，如果是-1，即未评估的也显示
        {
            QString file_name = suggestionPath+iter.key()+".txt";
            QFile file(file_name);
            QString info = iter.key() +": 文件丢失！";
            if(!file.open(QFile::ReadOnly | QFile::Text))
            {
                 QMessageBox::information(NULL, QString("错误"), info);
                 return;
            }
            QTextStream in(&file);
            PDFtext += '\n' +iter.key() + "安全性建议" ;//添加小标题
            painter_text.setFont(QFont("Arial", 13, QFont::Bold));//把小标题加黑，字号加大
            rectF = painter_text.boundingRect(rectangle, PDFtext, option);
            if(currentHeight+rectF.height() > pageHeight)
            {   printer_text.newPage(); currentHeight=20;rectangle.setY(currentHeight);rectF = painter_text.boundingRect(rectangle, PDFtext, option);   }
            else
                rectangle.setY(currentHeight+rectF.height());
            painter_text.drawText(rectF,PDFtext,option);
            currentHeight = currentHeight + rectF.height();
            rectangle.setY(currentHeight);
            PDFtext.clear();//清除小标题内容
            painter_text.setFont(QFont("",12));//恢复安全性建议正文的字体设置
            flag=true;
            while(flag)
            {
                PDFtext += in.readLine();
                rectF = painter_text.boundingRect(rectangle, PDFtext, option);
                if(currentHeight+rectF.height() > pageHeight)
                {   printer_text.newPage(); currentHeight=20;rectangle.setY(currentHeight);rectF = painter_text.boundingRect(rectangle, PDFtext, option);   }
                else
                    rectangle.setY(currentHeight+rectF.height());
                painter_text.drawText(rectF,PDFtext,option);
                PDFtext.clear();
                currentHeight = currentHeight + rectF.height();
                rectangle.setY(currentHeight);
                if(in.atEnd())
                    flag = false;
            }
            file.close();
        }
    }

/***********************************信息流部分生成PDF评估报告*****************************************************/

    /******************************信息流评估报告PDF中的信道容量表格**********************************************/
    /*if(active_module < 3)
    {
        painter_text.end();
        return;
    }*/
    //printer_text.newPage();
    QVector<InfCapacity> channelCapacity=getInfCapacityOfMessage();
    indexCount=channelCapacity.size();
    //控制信息流PDF信道容量表格大小的数据
    jianxi=25;//行宽大小，间隙
    beginX=40,endX=700;//设置表格的开始位置和结束位置点的坐标
    columnDistance1=150,columnDistance2=120;//设置列之间的宽度
    //信息流PDF中信道容量表格的标题
    if(currentHeight +100 >pageHeight)
    {
        printer_text.newPage();currentHeight = 20;
    }
    painter_text.setFont(QFont("Arial", 16, QFont::Bold));
    ff.setRect(beginX, currentHeight, endX-beginX, 40);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "信息流安全性评估报告");
    currentHeight += 40;
    painter_text.setFont(QFont("Arial", 14, QFont::Bold));
    ff.setRect(beginX, currentHeight, endX-beginX, 30);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "信道容量评估报告");
    currentHeight += 30;
    painter_text.setFont(QFont("Arial", 12, QFont::Bold));
    ff.setRect(beginX, currentHeight, endX-beginX, 30);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"评估时间："+current_date);
    currentHeight += 30;
    beginY = currentHeight;
    painter_text.setFont(QFont("Arial", 12, QFont::Bold));
    ff.setRect(beginX, beginY, columnDistance1, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"交互设备1");
    ff.setRect(beginX+columnDistance1, beginY, columnDistance1, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"交互设备2");
    ff.setRect(beginX+columnDistance1*2, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"最大容量Kbps");
    QRect ff4(beginX+columnDistance1*2+columnDistance2, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff4, Qt::AlignHCenter|Qt::AlignVCenter,"已用容量Kbps");
    QRect ff5(beginX+columnDistance1*2+columnDistance2*2, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff5, Qt::AlignHCenter|Qt::AlignVCenter,"剩余容量Kbps");
    currentHeight += jianxi;
    painter_text.setFont(QFont("",12));

    //表格两条横线
    painter_text.drawLine(beginX,beginY,endX,beginY);
    painter_text.drawLine(beginX,beginY+jianxi,endX,beginY+jianxi);
    //表格6条竖线
    painter_text.drawLine(beginX,beginY,beginX,beginY+jianxi);
    painter_text.drawLine(beginX+columnDistance1,beginY,beginX+columnDistance1,beginY+jianxi);
    painter_text.drawLine(beginX+2*columnDistance1,beginY,beginX+2*columnDistance1,beginY+jianxi);
    painter_text.drawLine(beginX+2*columnDistance1+columnDistance2,beginY,beginX+2*columnDistance1+columnDistance2,beginY+jianxi);
    painter_text.drawLine(beginX+2*columnDistance1+2*columnDistance2,beginY,beginX+2*columnDistance1+2*columnDistance2,beginY+jianxi);
    painter_text.drawLine(endX,beginY,endX,beginY+jianxi);

    for(int i=0;i<channelCapacity.size();++i)
    {
        //画横线
        painter_text.drawLine(beginX,currentHeight+jianxi,endX,currentHeight+jianxi);
        //画竖线
        painter_text.drawLine(beginX,currentHeight,beginX,currentHeight+jianxi);
        painter_text.drawLine(beginX+columnDistance1,currentHeight,beginX+columnDistance1,currentHeight+jianxi);
        painter_text.drawLine(beginX+2*columnDistance1,currentHeight,beginX+2*columnDistance1,currentHeight+jianxi);
        painter_text.drawLine(beginX+2*columnDistance1+columnDistance2,currentHeight,beginX+2*columnDistance1+columnDistance2,currentHeight+jianxi);
        painter_text.drawLine(beginX+2*columnDistance1+2*columnDistance2,currentHeight,beginX+2*columnDistance1+2*columnDistance2,currentHeight+jianxi);
        painter_text.drawLine(endX,currentHeight,endX,currentHeight+jianxi);

        ff.setRect(beginX, currentHeight, columnDistance1, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,channelCapacity[i].deviceName1);
        ff.setRect(beginX+columnDistance1, currentHeight, columnDistance1, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,channelCapacity[i].deviceName2);
        ff.setRect(beginX+columnDistance1*2, currentHeight, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,QString("%10").arg(channelCapacity[i].maxCapacity));
        ff.setRect(beginX+columnDistance1*2+columnDistance2, currentHeight, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,QString("%10").arg(channelCapacity[i].usedCapacity));
        ff.setRect(beginX+columnDistance1*2+columnDistance2*2, currentHeight, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,QString("%10").arg(channelCapacity[i].leftCapacity));
        currentHeight += jianxi;
        if(currentHeight >= pageHeight)
        {
            printer_text.newPage();currentHeight=20;
            //表格在一页放不下，剩余部分在新的一页，需要在表格最上方补画一条线
            painter_text.drawLine(beginX,currentHeight,endX,currentHeight);
        }
    }
/*******************************************信息流协议安全性分析*******************************************************/
    currentHeight += 20;
    rectangle.setY(currentHeight);
    QVector<protocalInf> usedProtocalInf=getProtocalInfForPDF();
    painter_text.setFont(QFont("Arial", 13, QFont::Bold));//把小标题加黑，字号加大
    str_temp = "协议特性分析";
    PDFtext = str_temp + '\n';
    rectF = painter_text.boundingRect(rectangle, PDFtext, option);
    if(currentHeight + rectF.height() > pageHeight)
    {    printer_text.newPage(); currentHeight = 20;rectangle.setY(currentHeight);rectF = painter_text.boundingRect(rectangle, PDFtext, option);}
    else
        rectangle.setY(currentHeight + rectF.height());
    painter_text.drawText(rectF,PDFtext,option);
    currentHeight = currentHeight + rectF.height();
    PDFtext.clear();
    painter_text.setFont(QFont("",12));//设置正文的字体大小
    for(int i=0;i<usedProtocalInf.size();++i)
    {
        PDFtext += "协议名称" + usedProtocalInf[i].protocalName + "     使用比例" + usedProtocalInf[i].protocalScale  + '\n';//添加小标题
        PDFtext += "        " + usedProtocalInf[i].protocalProblem + '\n' ;
        rectF = painter_text.boundingRect(rectangle, PDFtext, option);
        if(currentHeight + rectF.height() > pageHeight)
        {   printer_text.newPage(); currentHeight = 20;rectangle.setY(currentHeight);rectF = painter_text.boundingRect(rectangle, PDFtext, option);}
        else
            rectangle.setY(currentHeight + rectF.height());
        painter_text.drawText(rectF,PDFtext,option);
        currentHeight = currentHeight + rectF.height();
        rectangle.setY(currentHeight);
        PDFtext.clear();
        PDFtext += "        " + usedProtocalInf[i].protocalSuggestion+ '\n' ;
        rectF = painter_text.boundingRect(rectangle, PDFtext, option);
        if(currentHeight + rectF.height() > pageHeight)
        {   printer_text.newPage(); currentHeight = 20;rectangle.setY(currentHeight);rectF = painter_text.boundingRect(rectangle, PDFtext, option);}
        else
            rectangle.setY(currentHeight + rectF.height());
        painter_text.drawText(rectF,PDFtext,option);
        currentHeight = currentHeight + rectF.height();
        rectangle.setY(currentHeight);
        PDFtext.clear();
    }
/*******************************************信息流部分设备推荐的排序*******************************************************/
    QMap<QString,QStringList> deviceSorted = getDeviceSortForPDF();
    for(QMap<QString,QStringList>::iterator iter=deviceSorted.begin();iter!=deviceSorted.end();++iter)
    {
        painter_text.setFont(QFont("Arial", 13, QFont::Bold));//把小标题加黑，字号加大
        PDFtext += '\n' + iter.key() ;
        rectF = painter_text.boundingRect(rectangle, PDFtext, option);
        if(currentHeight + rectF.height() > pageHeight)
        {    printer_text.newPage(); currentHeight = 20;rectangle.setY(currentHeight);rectF = painter_text.boundingRect(rectangle, PDFtext, option);}
        else
            rectangle.setY(currentHeight + rectF.height());
        painter_text.drawText(rectF,PDFtext,option);
        currentHeight = currentHeight + rectF.height();
        rectangle.setY(currentHeight);
        PDFtext.clear();
        painter_text.setFont(QFont("",12));//设置正文的字体大小
        for(int k=0;k<iter.value().size();++k)
        {
            if(iter.value().size()-1 == k)
                PDFtext += iter.value()[k];
            else
                PDFtext += iter.value()[k] + "->";
        }
        rectF = painter_text.boundingRect(rectangle, PDFtext, option);
        if(currentHeight + rectF.height() > pageHeight)
        {    printer_text.newPage(); currentHeight = 20;rectangle.setY(currentHeight);rectF = painter_text.boundingRect(rectangle, PDFtext, option);}
        else
            rectangle.setY(currentHeight + rectF.height());
        painter_text.drawText(rectF,PDFtext,option);
        currentHeight = currentHeight + rectF.height();
        rectangle.setY(currentHeight);
        PDFtext.clear();
    }
/**********************************信息流部分安全符合性分析表格*****************************************************/
    QMap<QString,QMap<QString,QString> > totalMessRes=getTotalMessRes();
    indexCount=totalMessRes.size();
    jianxi=25;//行宽大小，间隙
    if((indexCount+3)*jianxi + currentHeight > pageHeight)
    {
        printer_text.newPage();
        currentHeight = 20;
    }
    //控制信息流PDF安全符合性分析表格表格大小的数据

    beginX=60,endX=680;//设置表格的开始位置和结束位置点的坐标
    columnDistance1=220,columnDistance2=100;//设置列之间的宽度
    //信息流PDF中信道容量表格的标题
    painter_text.setFont(QFont("Arial", 14, QFont::Bold));
    ff.setRect(beginX, currentHeight, endX-beginX, 30);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "信息流安全符合性评估报告");
    currentHeight += 30;
    painter_text.setFont(QFont("Arial", 12, QFont::Bold));
    ff.setRect(beginX, currentHeight, endX-beginX, 30);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"评估时间："+current_date);
    currentHeight += 30;
    beginY = currentHeight;
    painter_text.setFont(QFont("Arial", 12, QFont::Bold));
    ff.setRect(beginX, beginY, columnDistance1, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"交互信息名称");
    ff.setRect(beginX+columnDistance1, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"可用性得分");
    ff.setRect(beginX+columnDistance1+columnDistance2, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"完整性得分");
    ff.setRect(beginX+columnDistance1+2*columnDistance2, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"保密性得分");
    ff.setRect(beginX+columnDistance1+3*columnDistance2, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,"总分数");
    currentHeight += jianxi;
    painter_text.setFont(QFont("",12));

    //表格两条横线
    painter_text.drawLine(beginX,beginY,endX,beginY);
    painter_text.drawLine(beginX,beginY+jianxi,endX,beginY+jianxi);
    //表格6条竖线
    painter_text.drawLine(beginX,beginY,beginX,beginY+jianxi);
    painter_text.drawLine(beginX+columnDistance1,beginY,beginX+columnDistance1,beginY+jianxi);
    painter_text.drawLine(beginX+columnDistance1+columnDistance2,beginY,beginX+columnDistance1+columnDistance2,beginY+jianxi);
    painter_text.drawLine(beginX+columnDistance1+2*columnDistance2,beginY,beginX+columnDistance1+2*columnDistance2,beginY+jianxi);
    painter_text.drawLine(beginX+columnDistance1+3*columnDistance2,beginY,beginX+columnDistance1+3*columnDistance2,beginY+jianxi);
    painter_text.drawLine(endX,beginY,endX,beginY+jianxi);

    QMap<QString,QString> temp_indexNameAndGrade;
    for(QMap<QString,QMap<QString,QString> >::iterator iter=totalMessRes.begin();iter!=totalMessRes.end();++iter)
    {
        //画横线
        painter_text.drawLine(beginX,currentHeight+jianxi,endX,currentHeight+jianxi);
        //表格6条竖线
        painter_text.drawLine(beginX,currentHeight,beginX,currentHeight+jianxi);
        painter_text.drawLine(beginX+columnDistance1,currentHeight,beginX+columnDistance1,currentHeight+jianxi);
        painter_text.drawLine(beginX+columnDistance1+columnDistance2,currentHeight,beginX+columnDistance1+columnDistance2,currentHeight+jianxi);
        painter_text.drawLine(beginX+columnDistance1+2*columnDistance2,currentHeight,beginX+columnDistance1+2*columnDistance2,currentHeight+jianxi);
        painter_text.drawLine(beginX+columnDistance1+3*columnDistance2,currentHeight,beginX+columnDistance1+3*columnDistance2,currentHeight+jianxi);
        painter_text.drawLine(endX,currentHeight,endX,currentHeight+jianxi);

        ff.setRect(beginX, currentHeight, columnDistance1, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,iter.key());
        temp_indexNameAndGrade = iter.value();
        ff.setRect(beginX+columnDistance1, currentHeight, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,temp_indexNameAndGrade.find("可用性").value());//注意原子指标的值必须与被调函数保持一致
        ff.setRect(beginX+columnDistance1+columnDistance2, currentHeight, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,temp_indexNameAndGrade.find("完整性").value());
        ff.setRect(beginX+columnDistance1+2*columnDistance2, currentHeight, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,temp_indexNameAndGrade.find("保密性").value());
        ff.setRect(beginX+columnDistance1+3*columnDistance2, currentHeight, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,temp_indexNameAndGrade.find("评估总分数").value());
        currentHeight += jianxi;
        if(currentHeight >= pageHeight)
        {
            printer_text.newPage();currentHeight=20;
            //表格在一页放不下，剩余部分在新的一页，需要在表格最上方补画一条线
            painter_text.drawLine(beginX,currentHeight,endX,currentHeight);
        }
    }
/********************************信息流每个交互信息中低于60分的原子指标*******************************************/
    QMap<QString,QMap<QString,QString> > lowScoreIndex=getLowScoreIndexMess();
    rectangle.setY(currentHeight);
    for(QMap<QString,QMap<QString,QString> >::iterator iter = lowScoreIndex.begin();iter!=lowScoreIndex.end();++iter)
    {
        painter_text.setFont(QFont("Arial", 13, QFont::Bold));//把小标题加黑，字号加大
        str_temp = "交互信息名称：";
        PDFtext += '\n' + str_temp+iter.key() +",其原子指标中低于60分的有";
        rectF = painter_text.boundingRect(rectangle, PDFtext, option);
        if(currentHeight + rectF.height() > pageHeight)
        {    printer_text.newPage(); currentHeight = 20;rectangle.setY(currentHeight);rectF = painter_text.boundingRect(rectangle, PDFtext, option);}
        else
            rectangle.setY(currentHeight + rectF.height());
        painter_text.drawText(rectF,PDFtext,option);
        currentHeight = currentHeight + rectF.height();
        rectangle.setY(currentHeight);
        PDFtext.clear();
        painter_text.setFont(QFont("",12));//设置正文的字体大小
        for(QMap<QString,QString>::iterator iter_temp=iter.value().begin();iter_temp!=iter.value().end();++iter_temp)
            PDFtext += iter_temp.key()+iter_temp.value()+"分        ";
        rectF = painter_text.boundingRect(rectangle, PDFtext, option);
        if(currentHeight + rectF.height() > pageHeight)
        {    printer_text.newPage(); currentHeight = 20;rectangle.setY(currentHeight);rectF = painter_text.boundingRect(rectangle, PDFtext, option);}
        else
            rectangle.setY(currentHeight + rectF.height());
        painter_text.drawText(rectF,PDFtext,option);
        currentHeight = currentHeight + rectF.height();
        rectangle.setY(currentHeight);
        PDFtext.clear();
    }
/***************************************信息流中安全性建议****************************************************/
    QSet<QString> indexSug = getIndexSuggestion();
    for(QSet<QString>::iterator iter=indexSug.begin();iter!=indexSug.end();++iter)
    {
        QString file_name = messageSuggestionPath+"/"+*iter+".txt";
        QFile file(file_name);
        QString info = *iter +": 文件丢失！";
        if(!file.open(QFile::ReadOnly | QFile::Text))
        {
             QMessageBox::information(NULL, QString("错误"), info);
             return;
        }
        QTextStream in(&file);
        PDFtext += '\n' + *iter + "安全性建议" ;//添加小标题
        painter_text.setFont(QFont("Arial", 13, QFont::Bold));//把小标题加黑，字号加大
        rectF = painter_text.boundingRect(rectangle, PDFtext, option);
        if(currentHeight+rectF.height() > pageHeight)
        {   printer_text.newPage(); currentHeight=20;rectangle.setY(currentHeight);rectF = painter_text.boundingRect(rectangle, PDFtext, option);   }
        else
            rectangle.setY(currentHeight+rectF.height());
        painter_text.drawText(rectF,PDFtext,option);
        currentHeight = currentHeight + rectF.height();
        rectangle.setY(currentHeight);
        PDFtext.clear();//清除小标题内容
        painter_text.setFont(QFont("",12));//恢复安全性建议正文的字体设置
        flag=true;
        while(flag)
        {
            PDFtext += in.readLine();
            rectF = painter_text.boundingRect(rectangle, PDFtext, option);
            if(currentHeight+rectF.height() > pageHeight)
            {   printer_text.newPage(); currentHeight=20;rectangle.setY(currentHeight);rectF = painter_text.boundingRect(rectangle, PDFtext, option);   }
            else
                rectangle.setY(currentHeight+rectF.height());
            painter_text.drawText(rectF,PDFtext,option);
            PDFtext.clear();
            currentHeight = currentHeight + rectF.height();
            rectangle.setY(currentHeight);
            if(in.atEnd())
                flag = false;
        }
        file.close();
    }
/**********************************工作流部分生成PDF报告*****************************************************/
    /*if(active_module < 5)
    {
        painter_text.end();
        return;
    }*/
    QVector<workFlowPDF> workFlowInf = getWorkFlowPDF();
    beginX=70,endX=670;//设置表格的开始位置和结束位置点的坐标
    if(currentHeight + 100 >= pageHeight)
    {
        printer_text.newPage();
        currentHeight = 20;      
    }
    painter_text.setFont(QFont("Arial", 16, QFont::Bold));
    ff.setRect(beginX, currentHeight, endX-beginX, 30);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "工作流安全性评估报告");
    currentHeight += 30;
    painter_text.setFont(QFont("Arial", 12, QFont::Bold));
    ff.setRect(beginX, currentHeight, endX-beginX, 30);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "评估时间："+current_date);
    currentHeight += 30;
    beginY = currentHeight;
    //开始对每个工作流的结果进行排版
    for(int i=0;i<workFlowInf.size();++i)
    {
        if(currentHeight + 60 >= pageHeight)
        {
            printer_text.newPage();
            currentHeight = 20;
        }
        painter_text.setFont(QFont("Arial", 12, QFont::Bold));
        ff.setRect(70, currentHeight, 600, 30);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "工作流名称："+workFlowInf[i].workFlowName);
        currentHeight += 30;
        ff.setRect(70, currentHeight, 600, 30);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, workFlowInf[i].workFlowName + "设备可用性评估结果");
        currentHeight += 30;

        //每一个工作流的设备可用性表格
        indexCount = workFlowInf[i].deviceNameAndValue.size();
        jianxi = 25;
        beginX=240,beginY=currentHeight,endX=490;//设置表格的开始位置和结束位置点的坐标
        columnDistance1=150,columnDistance2=100;//设置列之间的宽度
        painter_text.setFont(QFont("Arial", 12, QFont::Bold));
        ff.setRect(beginX, beginY, columnDistance1, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "设备名称");
        ff.setRect(beginX+columnDistance1, beginY, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "动态可用性");
        painter_text.setFont(QFont("",12));
        currentHeight += jianxi;

        //表格两条横线
        painter_text.drawLine(beginX,beginY,endX,beginY);
        painter_text.drawLine(beginX,beginY+jianxi,endX,beginY+jianxi);
        //表格3条竖线
        painter_text.drawLine(beginX,beginY,beginX,beginY+jianxi);
        painter_text.drawLine(beginX+columnDistance1,beginY,beginX+columnDistance1,beginY+jianxi);
        painter_text.drawLine(endX,beginY,endX,beginY+jianxi);

        for(QMap<QString,QString>::iterator iter_temp = workFlowInf[i].deviceNameAndValue.begin(); iter_temp!=workFlowInf[i].deviceNameAndValue.end();++iter_temp)
        {
            painter_text.drawLine(beginX,currentHeight+jianxi,endX,currentHeight+jianxi);
            //表格3条竖线
            painter_text.drawLine(beginX,currentHeight,beginX,currentHeight+jianxi);
            painter_text.drawLine(beginX+columnDistance1,currentHeight,beginX+columnDistance1,currentHeight+jianxi);
            painter_text.drawLine(endX,currentHeight,endX,currentHeight+jianxi);

            ff.setRect(beginX, currentHeight, columnDistance1, jianxi);
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,iter_temp.key());
            ff.setRect(beginX+columnDistance1, currentHeight, columnDistance2, jianxi);
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,iter_temp.value());
            currentHeight += jianxi;
            if(currentHeight >= pageHeight)
            {
                printer_text.newPage();currentHeight=20;
                //表格在一页放不下，剩余部分在新的一页，需要在表格最上方补画一条线
                painter_text.drawLine(beginX,currentHeight,endX,currentHeight);
            }
        }

        //每个工作流任务可用性表格
        painter_text.setFont(QFont("Arial", 12, QFont::Bold));
        ff.setRect(70, currentHeight, 600, 30);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, workFlowInf[i].workFlowName + "任务可用性评估结果");
        currentHeight += 30;
        indexCount = workFlowInf[i].taskNameAndValue.size();
        //每一个工作流的任务可用性表格
        jianxi = 25;
        beginX=200,beginY=currentHeight,endX=540;//设置表格的开始位置和结束位置点的坐标
        columnDistance1=240,columnDistance2=100;//设置列之间的宽度
        painter_text.setFont(QFont("Arial", 12, QFont::Bold));
        ff.setRect(beginX, beginY, columnDistance1, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "任务名称");
        ff.setRect(beginX+columnDistance1, beginY, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "动态可用性");
        painter_text.setFont(QFont("",12));
        currentHeight += jianxi;
        //表格两条横线
        painter_text.drawLine(beginX,beginY,endX,beginY);
        painter_text.drawLine(beginX,beginY+jianxi,endX,beginY+jianxi);
        //表格3条竖线
        painter_text.drawLine(beginX,beginY,beginX,beginY+jianxi);
        painter_text.drawLine(beginX+columnDistance1,beginY,beginX+columnDistance1,beginY+jianxi);
        painter_text.drawLine(endX,beginY,endX,beginY+jianxi);
        for(QMap<QString,QString>::iterator iter_temp = workFlowInf[i].taskNameAndValue.begin(); iter_temp!=workFlowInf[i].taskNameAndValue.end();++iter_temp)
        {
            painter_text.drawLine(beginX,currentHeight+jianxi,endX,currentHeight+jianxi);
            //表格3条竖线
            painter_text.drawLine(beginX,currentHeight,beginX,currentHeight+jianxi);
            painter_text.drawLine(beginX+columnDistance1,currentHeight,beginX+columnDistance1,currentHeight+jianxi);
            painter_text.drawLine(endX,currentHeight,endX,currentHeight+jianxi);

            ff.setRect(beginX, currentHeight, columnDistance1, jianxi);
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,iter_temp.key());
            ff.setRect(beginX+columnDistance1, currentHeight, columnDistance2, jianxi);
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,iter_temp.value());
            currentHeight += jianxi;
            if(currentHeight >= pageHeight)
            {
                printer_text.newPage();currentHeight=20;
                //表格在一页放不下，剩余部分在新的一页，需要在表格最上方补画一条线
                painter_text.drawLine(beginX,currentHeight,endX,currentHeight);
            }
        }

        //画每个工作流设备保密性表格
        QVector<deviceSecurity> devSecurityWF = workFlowInf[i].deviceSecurityInf;//设备机密性表格的内容
        indexCount = devSecurityWF.size();
        jianxi = 25;
        beginX=110,beginY=currentHeight,endX=610;//设置表格的开始位置和结束位置点的坐标
        columnDistance1=120,columnDistance2=150,columnDistance3=80;//设置列之间的宽度
        painter_text.setFont(QFont("Arial", 12, QFont::Bold));
        ff.setRect(70, beginY, 600, 30);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, workFlowInf[i].workFlowName + "设备保密性评估结果");
        currentHeight += 30;
        beginY = currentHeight;
        ff.setRect(beginX, beginY,columnDistance1 , jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "设备名称");
        ff.setRect(beginX+columnDistance1, beginY, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "信息最大保密性");
        ff.setRect(beginX+columnDistance1+columnDistance2, beginY, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "设备当前保密性");
        ff.setRect(beginX+columnDistance1+2*columnDistance2, beginY, columnDistance3, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "是否满足");
        painter_text.setFont(QFont("",12));
        currentHeight += jianxi;
        //表格两条横线
        painter_text.drawLine(beginX,beginY,endX,beginY);
        painter_text.drawLine(beginX,beginY+jianxi,endX,beginY+jianxi);
        //表格5条竖线
        painter_text.drawLine(beginX,beginY,beginX,beginY+jianxi);
        painter_text.drawLine(beginX+columnDistance1,beginY,beginX+columnDistance1,beginY+jianxi);
        painter_text.drawLine(beginX+columnDistance1+columnDistance2,beginY,beginX+columnDistance1+columnDistance2,beginY+jianxi);
        painter_text.drawLine(beginX+columnDistance1+2*columnDistance2,beginY,beginX+columnDistance1+2*columnDistance2,beginY+jianxi);
        painter_text.drawLine(endX,beginY,endX,beginY+jianxi);

        for(int i_temp=0;i_temp<devSecurityWF.size();++i_temp)
        {
            painter_text.drawLine(beginX,currentHeight+jianxi,endX,currentHeight+jianxi);
            //表格5条竖线
            painter_text.drawLine(beginX,currentHeight,beginX,currentHeight+jianxi);
            painter_text.drawLine(beginX+columnDistance1,currentHeight,beginX+columnDistance1,currentHeight+jianxi);
            painter_text.drawLine(beginX+columnDistance1+columnDistance2,currentHeight,beginX+columnDistance1+columnDistance2,currentHeight+jianxi);
            painter_text.drawLine(beginX+columnDistance1+2*columnDistance2,currentHeight,beginX+columnDistance1+2*columnDistance2,currentHeight+jianxi);
            painter_text.drawLine(endX,currentHeight,endX,currentHeight+jianxi);

            ff.setRect(beginX, currentHeight, columnDistance1, jianxi);
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,devSecurityWF[i_temp].deviceName);
            ff.setRect(beginX+columnDistance1, currentHeight, columnDistance2, jianxi);
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,devSecurityWF[i_temp].infMaxSecurity);
            ff.setRect(beginX+columnDistance1+columnDistance2, currentHeight, columnDistance2, jianxi);
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,devSecurityWF[i_temp].currentSecurity);
            ff.setRect(beginX+columnDistance1+2*columnDistance2, currentHeight, columnDistance3, jianxi);
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,devSecurityWF[i_temp].satisfied);
            currentHeight += jianxi;
            if(currentHeight >= pageHeight)
            {
                printer_text.newPage();currentHeight=20;
                //表格在一页放不下，剩余部分在新的一页，需要在表格最上方补画一条线
                painter_text.drawLine(beginX,currentHeight,endX,currentHeight);
            }
        }

        //每一个工作流任务保密性表格
        QVector<infSecurity> taskSecurityWF = workFlowInf[i].infSecurityInf;//信息机密性表格的内容
        indexCount = taskSecurityWF.size();
        jianxi = 25;
        beginX=100,beginY=currentHeight,endX=640;//设置表格的开始位置和结束位置点的坐标
        columnDistance1=240,columnDistance2=100;//设置列之间的宽度
        painter_text.setFont(QFont("Arial", 12, QFont::Bold));
        ff.setRect(70, beginY, 600, 30);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, workFlowInf[i].workFlowName + "信息保密性评估结果");
        currentHeight += 30;
        beginY = currentHeight;

        ff.setRect(beginX, beginY,columnDistance1 , jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "信息名称");
        ff.setRect(beginX+columnDistance1, beginY, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "机密性需求");
        ff.setRect(beginX+columnDistance1+columnDistance2, beginY, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "当前机密性");
        ff.setRect(beginX+columnDistance1+2*columnDistance2, beginY, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "是否满足");
        painter_text.setFont(QFont("",12));
        currentHeight += jianxi;
        //表格两条横线
        painter_text.drawLine(beginX,beginY,endX,beginY);
        painter_text.drawLine(beginX,beginY+jianxi,endX,beginY+jianxi);
        //表格5条竖线
        painter_text.drawLine(beginX,beginY,beginX,beginY+jianxi);
        painter_text.drawLine(beginX+columnDistance1,beginY,beginX+columnDistance1,beginY+jianxi);
        painter_text.drawLine(beginX+columnDistance1+columnDistance2,beginY,beginX+columnDistance1+columnDistance2,beginY+jianxi);
        painter_text.drawLine(beginX+columnDistance1+2*columnDistance2,beginY,beginX+columnDistance1+2*columnDistance2,beginY+jianxi);
        painter_text.drawLine(endX,beginY,endX,beginY+jianxi);

        for(int i_temp=0;i_temp<taskSecurityWF.size();++i_temp)
        {
            painter_text.drawLine(beginX,currentHeight+jianxi,endX,currentHeight+jianxi);
            //表格5条竖线
            painter_text.drawLine(beginX,currentHeight,beginX,currentHeight+jianxi);
            painter_text.drawLine(beginX+columnDistance1,currentHeight,beginX+columnDistance1,currentHeight+jianxi);
            painter_text.drawLine(beginX+columnDistance1+columnDistance2,currentHeight,beginX+columnDistance1+columnDistance2,currentHeight+jianxi);
            painter_text.drawLine(beginX+columnDistance1+2*columnDistance2,currentHeight,beginX+columnDistance1+2*columnDistance2,currentHeight+jianxi);
            painter_text.drawLine(endX,currentHeight,endX,currentHeight+jianxi);

            ff.setRect(beginX, currentHeight, columnDistance1, jianxi);
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,taskSecurityWF[i_temp].infName);
            ff.setRect(beginX+columnDistance1, currentHeight, columnDistance2, jianxi);
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,taskSecurityWF[i_temp].infSecurityDemand);
            ff.setRect(beginX+columnDistance1+columnDistance2, currentHeight, columnDistance2, jianxi);
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,taskSecurityWF[i_temp].currentSecurity);
            ff.setRect(beginX+columnDistance1+2*columnDistance2, currentHeight, columnDistance2, jianxi);
            painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,taskSecurityWF[i_temp].satisfied);
            currentHeight += jianxi;
            if(currentHeight >= pageHeight)
            {
                printer_text.newPage();currentHeight=20;
                //表格在一页放不下，剩余部分在新的一页，需要在表格最上方补画一条线
                painter_text.drawLine(beginX,currentHeight,endX,currentHeight);
            }
        }
    }
    //画工作流可用性需求与评估结果对比表格
    QVector<workFlowRes> workFRes = getWorkFlowRes();
    indexCount = workFRes.size();
    if(currentHeight + 30 >= pageHeight)
    {
        printer_text.newPage();
        currentHeight = 20;
        beginY = currentHeight;
    }
    else
        beginY = currentHeight+10;
    painter_text.setFont(QFont("Arial", 12, QFont::Bold));
    ff.setRect(70, beginY, 600, 30);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "工作流可用性评估需求与评估结果");
    currentHeight += 40;
    jianxi = 25;
    beginX=90,beginY=currentHeight,endX=650;//设置表格的开始位置和结束位置点的坐标
    columnDistance1=200,columnDistance2=120;//设置列之间的宽度
    painter_text.setFont(QFont("Arial", 12, QFont::Bold));
    ff.setRect(beginX, beginY, columnDistance1, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "工作流名称");
    ff.setRect(beginX+columnDistance1, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "可用性需求");
    ff.setRect(beginX+columnDistance1+columnDistance2, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "实际评估结果");
    ff.setRect(beginX+columnDistance1+2*columnDistance2, beginY, columnDistance2, jianxi);
    painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter, "是否满足");
    painter_text.setFont(QFont("",12));
    currentHeight += jianxi;
    //表格两条横线
    painter_text.drawLine(beginX,beginY,endX,beginY);
    painter_text.drawLine(beginX,beginY+jianxi,endX,beginY+jianxi);
    //表格5条竖线
    painter_text.drawLine(beginX,beginY,beginX,beginY+jianxi);
    painter_text.drawLine(beginX+columnDistance1,beginY,beginX+columnDistance1,beginY+jianxi);
    painter_text.drawLine(beginX+columnDistance1+columnDistance2,beginY,beginX+columnDistance1+columnDistance2,beginY+jianxi);
    painter_text.drawLine(beginX+columnDistance1+2*columnDistance2,beginY,beginX+columnDistance1+2*columnDistance2,beginY+jianxi);
    painter_text.drawLine(endX,beginY,endX,beginY+jianxi);

    for(int i_temp=0;i_temp<workFRes.size();++i_temp)
    {
        painter_text.drawLine(beginX,currentHeight+jianxi,endX,currentHeight+jianxi);
        //表格5条竖线
        painter_text.drawLine(beginX,currentHeight,beginX,currentHeight+jianxi);
        painter_text.drawLine(beginX+columnDistance1,currentHeight,beginX+columnDistance1,currentHeight+jianxi);
        painter_text.drawLine(beginX+columnDistance1+columnDistance2,currentHeight,beginX+columnDistance1+columnDistance2,currentHeight+jianxi);
        painter_text.drawLine(beginX+columnDistance1+2*columnDistance2,currentHeight,beginX+columnDistance1+2*columnDistance2,currentHeight+jianxi);
        painter_text.drawLine(endX,currentHeight,endX,currentHeight+jianxi);

        ff.setRect(beginX, currentHeight, columnDistance1, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,workFRes[i_temp].workFlowName);
        ff.setRect(beginX+columnDistance1, currentHeight, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,workFRes[i_temp].workFlowReqAvail);
        ff.setRect(beginX+columnDistance1+columnDistance2, currentHeight, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,workFRes[i_temp].workFlowActualAvail);
        ff.setRect(beginX+columnDistance1+2*columnDistance2, currentHeight, columnDistance2, jianxi);
        painter_text.drawText(ff, Qt::AlignHCenter|Qt::AlignVCenter,workFRes[i_temp].satisfied);
        currentHeight += jianxi;
        if(currentHeight >= pageHeight)
        {
            printer_text.newPage();currentHeight=20;
            //表格在一页放不下，剩余部分在新的一页，需要在表格最上方补画一条线
            painter_text.drawLine(beginX,currentHeight,endX,currentHeight);
        }
    }

    painter_text.end();
}
/********************************物理设备安全性输出PDF评估报告调用函数开始******************************************/
//物理部分每个设备的六大性评估指标结果
QMap<QString,QMap<QString,QString> > MainWindow::getWuliIndexInf()
{
    int list_cout;
    QString node_name,dev_type;
    QList<unsigned int> find;
    QMap<QString,QMap<QString,QString> > wuliInf;
    QMap<QString,QString> indexNameAndScore;
    for(int i=0;i!=devicegroup.size();i++)
    {
        Device_Informa CurrentDevice=sys_inf->Get_SingleDevice(devicegroup[i]);
        dev_type=QString::number(CurrentDevice._Get_DeviceType());
        if(dev_type=="1")
           device_name[0].append(CurrentDevice._Get_DeviceName());
        else if(dev_type=="2")
            device_name[1].append(CurrentDevice._Get_DeviceName());
        else if(dev_type=="3")
            device_name[2].append(CurrentDevice._Get_DeviceName());
        else//第四类非信息类设备，不填加到下拉列表
        {}
     }
    for(int m=0;m<3;++m)
    {
        Index* index_conf = index_select[m];
        find = index_conf->m_nodeParentId.keys(1); //找到主根节点下的所有一级子指标的节点nodeid，组成列表
        list_cout = find.size();
        for(QList<QString>::iterator j=device_name[m].begin();j!=device_name[m].end();++j)
        {
            QString deviceName = *j;
            if(index_conf->m_ID_calculateResult.find(deviceName)==index_conf->m_ID_calculateResult.end())
            {
                for(int i = 0;i<list_cout;i++)
                {
                    node_name = index_conf->m_nodeMark[find[i]];  //名称在一个指标体系下是相同的
                    indexNameAndScore.insert(node_name,"未评估");
                }
                indexNameAndScore.insert("总分","未评估");
            }
            else
            {
                indexNameAndScore.insert("总分",QString::number(index_conf->m_ID_calculateResult[deviceName][1]));
                for(int i = 0;i<list_cout;i++)
                {
                    node_name = index_conf->m_nodeMark[find[i]];  //名称在一个指标体系下是相同的
                    double node_value = index_conf->m_ID_calculateResult[deviceName][find[i]];   //不同的设备，分数不同，分开存储
                    if(node_value<0)
                        indexNameAndScore.insert(node_name,"未评估");
                    else
                        indexNameAndScore.insert(node_name,QString::number(node_value));
                }
            }
            wuliInf.insert(deviceName,indexNameAndScore);
            indexNameAndScore.clear();
        }
    }
    return wuliInf;
}
//物理部分每个设备的低于60分的原子指标及其分数
QMap<QString, QMap<QString, QString> > MainWindow::getLowScoreIndexWuli()
{
    QMap<QString,QMap<QString,QString> > wuliInf;
    QMap<QString,QString> indexNameAndScore;
    //每一类设备
    for(int m=0;m<3;++m)
    {
        Index* index_conf = index_select[m];
        //每一类中的每一个设备,设备名称与此设备所有指标和分数
        //qDebug()<<"这是什么"<<index_conf->m_ID_calculateResult;
        for(QMap<QString,QMap<uint,double> >::iterator iter=index_conf->m_ID_calculateResult.begin();iter!=index_conf->m_ID_calculateResult.end();++iter)
        {
            //每一个设备中的所有指标和分数
            for(QMap<uint,double>::iterator iter_temp=iter.value().begin();iter_temp!=iter.value().end();++iter_temp)
            {
                if(index_conf->m_nodeIsAutomic.empty())
                    qDebug()<<"判断是否是原子指标，为空";
                QMap<unsigned int,bool>::const_iterator i_temp = index_conf->m_nodeIsAutomic.find(iter_temp.key());
                if (i_temp != (index_conf->m_nodeIsAutomic).end() && i_temp.key() == iter_temp.key())
                {
                    if(i_temp.value() && iter_temp.value()<60 && iter_temp.value()>=0)//是原子指标且分数低于60分
                    {
                        indexNameAndScore.insert(index_conf->m_nodeMark[i_temp.key()],QString::number(iter_temp.value(), 10, 2));
                    }
                }
                else
                    qDebug()<<"未找到此原子指标";
            }
            if(!indexNameAndScore.empty())
            {
                wuliInf.insert(iter.key(),indexNameAndScore);
                indexNameAndScore.clear();
            }
        }
    }
    qDebug()<<"物理部分每个设备的低于60分的原子指标及其分数"<<wuliInf;
    return wuliInf;
}
//物理部分低于60分原子指标的安全性建议,返回的是低于60分的原子指标名称
QSet<QString> MainWindow::getSecSuggestionWuli()
{
    QMap<QString,QMap<QString,QString> >lowScoreIndex = getLowScoreIndexWuli();
    QSet<QString> lowScoreIndexName;
    for(QMap<QString,QMap<QString,QString> >::iterator iter=lowScoreIndex.begin();iter!=lowScoreIndex.end();++iter)
    {
        for(QMap<QString,QString>::iterator iter_temp=iter.value().begin();iter_temp!=iter.value().end();++iter_temp)
        {
            if(iter_temp.key()!="总分" && (iter_temp.value().toFloat()<60 || iter_temp.value()=="-1"))
                lowScoreIndexName.insert(iter_temp.key());
        }
    }
    qDebug()<<"物理部分低于60分的原子指标名称"<<lowScoreIndexName;
    return lowScoreIndexName;
}

/********************************物理设备安全性输出PDF评估报告调用函数结束******************************************/

/********************************架构安全性输出PDF评估报告调用函数开始******************************************/

/*
 * 函数功能：生成评估报告中的表格html,网络架构评估报告中的表格
 * 调用的函数：getInternetResFromDB()
*/
QString getPDFHtmlForInternet()
{
    QStringList chartTitle;
    chartTitle.push_back(QStringLiteral("原子指标名称"));
    chartTitle.push_back(QStringLiteral("分数"));
    chartTitle.push_back(QStringLiteral("评估时间"));

    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
    QString temp="未评估";
    QString html;
    html += "<h2 align=\"center\">"+QStringLiteral("信息安全评测报告")+"</h2>";
    html += "<h4 align=\"center\">"+current_date+"<h4>";
    html += "<h4 align=\"center\">"+QStringLiteral("网络架构安全性评估结果")+"<h4>";
    html += "<table width=\"500\"  border=\"1\" align=\"center\" style=\"border-collapse:collapse;\"bordercolor=\"gray\">";
    html += "<tr style=\"background-color:gray\"  margin-left=\"auto\" margin-right=\"auto\">";
    for(int i=0;i<chartTitle.count();i++)
        html += QString("<th>%1</th>").arg(chartTitle.at(i));
    html += "</tr>";

    QMap<QString,double> internetRes=getInternetResFromDB();
    for(QMap<QString,double>::iterator iter=internetRes.begin();iter!=internetRes.end();iter++)
    {
        html += "<tr>";
        //html += QString("<th>%1</th>").arg(iter.key());
        html += "<td align=\"center\">" + iter.key() + "</td>";
        if(iter.value()<0)
            html += "<td align=\"center\">" + temp + "</td>";
            //html += QString("<td>%1</td>").arg("未评估");
        else
            //html += QString("<td>%1</td>").arg( QString("%10").arg(iter.value()));
            html += "<td align=\"center\">" + QString("%10").arg(iter.value()) + "</td>";
        //html += QString("<td>%1</td>").arg(current_date);
        html += "<td align=\"center\">" + current_date + "</td>";
        html += "</tr>";
    }
    html += "</table>";
    return html;
}
/*
 * 函数的功能：从数据库中读取网络架构指标名称和分数，提供给在PDF文档中画表格的函数
 * 此函数返回了所有的指标，包括原子指标和复合指标
*/
QMap<QString,double> getInternetResFromDB()
{
    QMap<QString,double> internetRes;//存储网络架构原子指标名称和分数
    QString indexName="";
    double indexGrade;
    QString index="select description,result from 网络架构指标树";
    QSqlQuery queryIndex;
    queryIndex.exec(index);
    queryIndex.next();
    queryIndex.next();//过滤数据库的第一行数据，第一行数据不是指标项
    while(queryIndex.isValid())
    {
        indexName=queryIndex.value(0).toString();
        indexGrade=queryIndex.value(1).toDouble();
        internetRes.insert(indexName,indexGrade);
        queryIndex.next();
    }
    return internetRes;
}
/*
 * 函数的功能：从数据库中读取网络架构原子指标名称和分数，提供给在PDF文档中输出安全性建议的函数
 * 此函数只返回了原子指标，不包括复合指标
*/
QMap<QString,double> getIndexInternetResFromDB()
{
    QMap<QString,double> indexInternetRes;//存储网络架构原子指标名称和分数
    QString indexName="";
    double indexGrade;
    QString index="select description,result from 网络架构指标树 where atomic=1";
    QSqlQuery queryIndex;
    queryIndex.exec(index);
    queryIndex.next();
    while(queryIndex.isValid())
    {
        indexName=queryIndex.value(0).toString();
        indexGrade=queryIndex.value(1).toDouble();
        indexInternetRes.insert(indexName,indexGrade);
        queryIndex.next();
    }
    return indexInternetRes;
}

/**********************************架构安全性输出PDF评估报告调用函数结束******************************************/

/*********************************信息流安全性输出PDF评估报告调用函数开始******************************************/

/************************************信息流获得每个信道容量的函数************************************************/
QVector<InfCapacity> getInfCapacityOfMessage()
{
    QVector<InfCapacity> channelCapacity;
    QHash<pair<uint,uint>,pair<float,float>> flow;
    for(int i_temp=0;i_temp!=messagegroup.size();i_temp++)
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
            flow.insert(pair<uint,uint>(temp_SendDevice,temp_ReceiveDevice),pair<float,float>(temp_flow,                                                                                             sys_inf->Get_ChannelCapacity(temp_ReceiveDevice,temp_SendDevice)));
        }
    }
    InfCapacity channel;//存储一条链路容量的信息
    for(auto tem=flow.begin();tem!=flow.end();tem++)
    {
        channel.deviceName1 = sys_inf->Get_SingleDevice(tem.key().first)._Get_DeviceName();
        channel.deviceName2 = sys_inf->Get_SingleDevice(tem.key().second)._Get_DeviceName();
        channel.maxCapacity = tem.value().second;//最大信道容量
        channel.usedCapacity = tem.value().first;//已经使用的信道容量
        channel.leftCapacity = tem.value().second - tem.value().first;
        channelCapacity.push_back(channel);
    }
    return channelCapacity;
}
/*********************************信息流协议特点的函数，提供给生成PDF的函数******************************************/
QVector<protocalInf> MainWindow::getProtocalInfForPDF()
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
    QList<double> verStrTime;//用来求每个协议的百分比,统计此协议出现的次数
    int totalProtalCount=0;
    for(auto i_temp=Protocal_Time.begin();i_temp!=Protocal_Time.end();i_temp++)
    {
        verStrProtocalId.push_back(i_temp.key());
        verStrTime.push_back(i_temp.value());
        totalProtalCount += i_temp.value();
    }

    QVector<protocalInf> protocalInfForPDF;
    protocalInf oneProtocalInf;
    QStringList* totalProtocalSuggestion = mess_assess->Get_ProtocalText();
    for(int i=0;i<vecStrProtocalName.size() && i<verStrTime.size();++i)
    {
        oneProtocalInf.protocalName = vecStrProtocalName.at(i);
        oneProtocalInf.protocalScale = QString::number((verStrTime.at(i)/totalProtalCount) * 100) + "%";
        for(int j=0;j<total_protocal.size();++j)
        {
            if(total_protocal.at(j) == vecStrProtocalName.at(i))
            {
                oneProtocalInf.protocalProblem = totalProtocalSuggestion->at(2*j);
                oneProtocalInf.protocalSuggestion = totalProtocalSuggestion->at(2*j+1);
            }
        }
        protocalInfForPDF.push_back(oneProtocalInf);
    }
    return protocalInfForPDF;
}

/***************************信息流关键设备、边界防护推荐的设备排序，提供给生成PDF的函数******************************************/
QMap<QString,QStringList> getDeviceSortForPDF()
{
    QList<double> dDeviceId;
    QList<QString> vecStrDeviceName;
    QList<double> temp_secu,temp_reli,temp_message_total,temp_secu_min;
    QMap<QString,QStringList> deviceSorted;//根据不同的要求QString，推荐的设备排序QStringList
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
    QStringList sortedDev_byReli = set_sort(temp_reli);
    deviceSorted.insert("根据报文可靠性需求，推荐的设备排序",sortedDev_byReli);
    QStringList sortedDev_bySecu = set_sort(temp_secu);
    deviceSorted.insert("根据报文保密性需求，推荐的设备排序",sortedDev_bySecu);
    QStringList sortedDev_byTotalMessage = set_sort(temp_message_total);
    deviceSorted.insert("根据流经设备总流量，推荐的设备排序",sortedDev_byTotalMessage);
    QList<double> temp_secu_diff;
    for(int i=0;i!=devicegroup.size();i++)
        temp_secu_diff.push_back(temp_secu[i]-temp_secu_min[i]);
    QStringList sortedDev_bySecuDiff = set_sort(temp_secu_diff);
    deviceSorted.insert("根据由密级服务产生的边界安全，推荐的设备排序",sortedDev_bySecuDiff);
    return deviceSorted;
}
QStringList set_sort(QList<double> &temp_reli)
{
    QStringList sortedDevice;
    QList<pair<double,uint>> sort_secu;
    for(int i=0;i!=devicegroup.size();i++)
    {
        sort_secu.push_back(pair<double,uint>(temp_reli[i],devicegroup[i]));
    }
    std::sort(sort_secu.begin(),sort_secu.end(),CompaireByValue);
    for(int i=0;i!=devicegroup.size();i++)
    {
        sortedDevice = sortedDevice<<sys_inf->Get_SingleDevice(sort_secu[i].second)._Get_DeviceName();
    }
    return sortedDevice;
}
bool CompaireByValue(pair<double,uint> &a1,pair<double,uint> &a2)
{
    return (a1.first)>(a2.first);
}
/***************************信息流安全符合性分析表格，提供给生成PDF的函数******************************************/
QMap<QString,QMap<QString,QString> > getTotalMessRes()
{
    QMap<QString,QMap<QString,QString> > totalMessRes;
    QString current_mess;
    QMap<QString,QString> indexNameAndGrade;
    bool allIndexFinished;//有一个指标没有评估完，则没有总分数
    for(int i=0;i!=messagegroup.size();i++)
    {
        current_mess = sys_inf->Get_SingleMessage(messagegroup[i])._Get_MessageName();
        for(int i_temp=0;i_temp!=message_index.size();i_temp++)
        {
            allIndexFinished = true;
            if(m_message_index->m_ID_calculateResult.find(current_mess)==m_message_index->m_ID_calculateResult.end())
            {
                allIndexFinished = false;
                indexNameAndGrade.insert(m_message_index->m_nodeMark[message_index[i_temp]],"未评估");
            }
            else
            {
                double score_temp = m_message_index->m_ID_calculateResult[current_mess][message_index[i_temp]];
                if(score_temp<0)
                {    allIndexFinished = false;indexNameAndGrade.insert(m_message_index->m_nodeMark[message_index[i_temp]],"未评估");}
                else
                    indexNameAndGrade.insert(m_message_index->m_nodeMark[message_index[i_temp]],QString::number(score_temp));
            }
        }
        if(!allIndexFinished || (m_message_index->m_ID_calculateResult.find(current_mess)==m_message_index->m_ID_calculateResult.end()))
            indexNameAndGrade.insert("评估总分数","未评估");
        else
            indexNameAndGrade.insert("评估总分数",QString::number(m_message_index->m_ID_calculateResult[current_mess][1]));
        totalMessRes.insert(current_mess,indexNameAndGrade);
        current_mess = "";
        indexNameAndGrade.clear();
    }
    return totalMessRes;
}
/*********************************信息流中每个交互信息中低于60分的原子指标*********************************/
//返回每个交互信息中低于60分的原子指标及分数
QMap<QString,QMap<QString,QString> > getLowScoreIndexMess()
{
    QList<unsigned int> find;
    QList<unsigned int> index = {NULL};
    QString node_name;
    double node_value;
    QMap<QString,QMap<QString,QString> >  lowScoreIndexInf;
    QMap<QString,QMap<QString,QString> > interactiveInf=getTotalMessRes();//交互信息的集合，它的key就是所有的交互信息
    find = m_message_index->m_nodeParentId.keys(1); //找到主根节点下的所有一级子指标的节点nodeid，组成列表
    bool existLowIndex;
    QMap<QString,QString> lowIndexNameAndScore;
    for(QMap<QString,QMap<QString,QString> >::iterator iter = interactiveInf.begin();iter!=interactiveInf.end();++iter)
    {
        for(auto i=0;i<find.size();i++)
        {
            index.clear();
            existLowIndex = false;
            index = m_message_index->m_nodeParentId.keys(find[i]);
            for(auto j=0;j<index.size();j++)
            {
                if(m_message_index->m_ID_calculateResult.find(iter.key())==m_message_index->m_ID_calculateResult.end())
                    continue;
                 node_name = m_message_index->m_nodeMark[index[j]];
                 node_value = m_message_index->m_ID_calculateResult[iter.key()][index[j]];   //不同的设备，分数不同，分开存储
                 if(node_value>=0 && node_value<60)
                 {
                     existLowIndex = true;
                     lowIndexNameAndScore.insert(node_name,QString::number(node_value));
                 }
                 else if(node_value<0)
                     lowIndexNameAndScore.insert(node_name,"未评估");
            }
        }
        if(existLowIndex)
        {
            lowScoreIndexInf.insert(iter.key(),lowIndexNameAndScore);
            lowIndexNameAndScore.clear();
        }
    }
    return lowScoreIndexInf;
}
//低于60分的原子指标安全性建议，返回的是所有交互信息中低于60分的原子指标名称
QSet<QString> getIndexSuggestion()
{
    QSet<QString> lowScoreSug;
    QMap<QString,QMap<QString,QString> > lowScoreInf=getLowScoreIndexMess();
    for(QMap<QString,QMap<QString,QString> >::iterator iter=lowScoreInf.begin();iter!=lowScoreInf.end();++iter)
    {
        for(QMap<QString,QString>::iterator iter_temp = iter.value().begin();iter_temp != iter.value().end();++iter_temp)
            lowScoreSug.insert(iter_temp.key());
    }
    return lowScoreSug;
}
/***************************************工作流生成PDF评估报告时调用的函数**************************************************/
QVector<workFlowPDF> MainWindow::getWorkFlowPDF()
{
    QVector<workFlowPDF> workFlowInf;
    QMap<QString,QString> deviceInf;//工作流可用性评估，设备名称和设备的动态可用性
    QMap<QString,QString> taskInf;//工作流可用性评估，任务名称和任务的可用性
    QVector<deviceSecurity> deviceSecurityOneWorkF;//设备机密性表格的内容
    QVector<infSecurity> infSecurityOneWorkF;//信息机密性表格的内容
    QList<WorkflowCom *> allWorkFlow = m_poWFlowDlg->m_pWorkflowList;//所有的工作流
    for(int i=0;i<allWorkFlow.size();++i)
    {
        //每一条的工作流的信息
        workFlowPDF oneWorkFlowInf;
        oneWorkFlowInf.workFlowName = allWorkFlow[i]->m_strWorkflowName;
        //每一条工作流设备名称和设备的动态可用性
        for (QMap<uint, double>::iterator iter = allWorkFlow[i]->m_oDeviceDynamicAvail.begin();
             iter != allWorkFlow[i]->m_oDeviceDynamicAvail.end(); ++iter)
        {
            deviceInf.insert(sys_inf->Get_SingleDevice(iter.key())._Get_DeviceName(),QString::number(iter.value()));
        }
        oneWorkFlowInf.deviceNameAndValue = deviceInf;
        for (QMap<uint, double>::iterator iter = allWorkFlow[i]->m_oTaskAvail.begin();
             iter != allWorkFlow[i]->m_oTaskAvail.end(); ++iter)
        {
            taskInf.insert(allWorkFlow[i]->m_oWorkflowTask[iter.key()].GetTaskName(),QString::number(iter.value()));
        }
        oneWorkFlowInf.taskNameAndValue = taskInf;

        QMap<uint,double> device_group=allWorkFlow[i]->m_oDeviceStaticCon;
        QMap<uint,double> mess_group=allWorkFlow[i]->m_oInfoStaticCon;
        for(auto i_temp=device_group.begin();i_temp!=device_group.end();i_temp++)
        {
            Device_Informa current_device=sys_inf->Get_SingleDevice(i_temp.key());
            deviceSecurity oneDevSecurity;//一个工作流相关设备保密性列表中的某一行
            oneDevSecurity.deviceName = current_device._Get_DeviceName();
            float max_con=0;
            for(auto j_temp=mess_group.begin();j_temp!=mess_group.end();++j_temp)
            {
                Message_Informa current_mess=sys_inf->Get_SingleMessage(j_temp.key());
                if(current_mess._Get_MessageSendDevice()==i_temp.key()||
                        current_mess._Get_MessageReceiveDevice()==i_temp.key())
                {
                    if(current_mess._Get_AimMessageSecurity()>max_con)
                        max_con=current_mess._Get_AimMessageSecurity();
                }
            }
            oneDevSecurity.infMaxSecurity = QString::number(max_con);
            oneDevSecurity.currentSecurity = QString::number(i_temp.value());
            oneDevSecurity.satisfied = QString::number((i_temp.value()>=max_con));
            deviceSecurityOneWorkF.push_back(oneDevSecurity);//一条工作流的设备保密性列表
            /*qDebug()<<"工作流相关的设备保密性，设备名称"<<oneDevSecurity.deviceName;
            qDebug()<<"工作流相关的设备保密性，信息最大保密性"<<oneDevSecurity.infMaxSecurity;
            qDebug()<<"工作流相关的设备保密性，设备当前保密性"<<oneDevSecurity.currentSecurity;
            qDebug()<<"工作流相关的设备保密性，是否满足"<<oneDevSecurity.satisfied;*/
        }
        oneWorkFlowInf.deviceSecurityInf = deviceSecurityOneWorkF;

        for(auto i_temp=mess_group.begin();i_temp!=mess_group.end();i_temp++)
        {
            Message_Informa current_mess=sys_inf->Get_SingleMessage(i_temp.key());
            infSecurity oneInfSecurity;
            oneInfSecurity.infName = current_mess._Get_MessageName();
            oneInfSecurity.infSecurityDemand = QString::number(current_mess._Get_AimMessageSecurity());
            oneInfSecurity.currentSecurity = QString::number(i_temp.value());
            oneInfSecurity.satisfied = QString::number((i_temp.value()>=current_mess._Get_AimMessageSecurity()));
            infSecurityOneWorkF.push_back(oneInfSecurity);
        }
        oneWorkFlowInf.infSecurityInf = infSecurityOneWorkF;

        workFlowInf.push_back(oneWorkFlowInf);
        deviceInf.clear();
        taskInf.clear();
        deviceSecurityOneWorkF.clear();
        infSecurityOneWorkF.clear();
    }
    return workFlowInf;
}
//此函数返回所有工作流的可用性需求和实际可用性
QVector<workFlowRes> MainWindow::getWorkFlowRes()
{
    QVector<workFlowRes> workFRes;
    QList<WorkflowCom *> allWorkFlow = m_poWFlowDlg->m_pWorkflowList;//所有的工作流
    for(int i=0;i<allWorkFlow.size();++i)
    {
        //每一条的工作流的信息
        workFlowRes oneWorkFlowInf;
        pair<QString, pair<double, double>> workflowAvail = allWorkFlow[i]->GetWorkflowAvail();
        oneWorkFlowInf.workFlowName = workflowAvail.first;
        oneWorkFlowInf.workFlowReqAvail = QString::number(workflowAvail.second.first,10,2);
        oneWorkFlowInf.workFlowActualAvail = QString::number(workflowAvail.second.second,10,2);
        if(workflowAvail.second.first > workflowAvail.second.second)
            oneWorkFlowInf.satisfied = "否";
        else
            oneWorkFlowInf.satisfied = "是";
        workFRes.push_back(oneWorkFlowInf);
    }
    return workFRes;
}
//查看生成的PDF评估报告
void MainWindow::on_action_viewRecord_triggered()
{
    QString qtManulFile="D:\\Program Files (x86)\\X_Ship\\DB_51aspx\\ProjectFiles\\"+creatName+"\\"+creatName+"评估报告.pdf";
    QDesktopServices::openUrl(QUrl::fromLocalFile(qtManulFile));
}
