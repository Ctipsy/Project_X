#include "main/mainwindow.h"
#include "login/loginpanel.h"
#include <QApplication>
#include <QFile>
#include <QVector>
#include <QDebug>
#include <QtSql>
#include "sys_discript/system_informa.h"
#include "sys_discript/base_informa.h"
bool g_session = false;         //全局变量 账户类型判断,其实可以不同初始化，因为编译器自动将初始值设为false(已测试)
int  active_module=0;           //全局变量 激活模块判断,其实可以不同初始化，因为编译器自动将初始值设为0
int  active_days=0;             //全局变量 激活时间判断
int  jihuo = 0;
QString  creatUser;     //全局变量 创建项目人
QString  creatTime;     //全局变量 创建项目时间
QString  creatPath;     //全局变量 创建项目路径
QString  creatName;     //全局变量 创建项目名称
QSqlDatabase m_dbConnect;//基准数据库
QSqlDatabase m_dbConnectPro;//工程文件下的数据库
QSqlDatabase DatabaseConnect();
QMap<QString,int> CheckQuestionnaire();
QMap<QString,int> indexNameNotFound;
void resetIndexStandardDB();
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    m_dbConnect = DatabaseConnect();    //基准数据库
    resetIndexStandardDB();             //基准数据库的初始化
    indexNameNotFound=CheckQuestionnaire();
    MainWindow w;
    app.setQuitOnLastWindowClosed(false);

    QString qss;
    QFile file(":/qss/chatClient.qss");
    file.open(QFile::ReadOnly);
   if(file.isOpen())
   {
        qss = QLatin1String(file.readAll());
        app.setStyleSheet(qss);
        file.close();
   }
   //加载登录面板

    LoginPanel logingPanel;
    logingPanel.show();//不加这一句的话，进入找回密码等子界面，就没法操作鼠标，焦点始终在登陆主界面

    if(logingPanel.exec()==QDialog::Accepted)
    {
        if(g_session==false)
        {
            QString qss;
            QFile file(":/qss/dark_orange.qss");
            file.open(QFile::ReadOnly);
           if(file.isOpen())
           {
                qss = QLatin1String(file.readAll());
                w.setStyleSheet(qss);
                file.close();
           }
            w.show();
        }
        else
        {
           admin_config  *config_window=new admin_config;
           QString qss;
           QFile file(":/qss/dark_orange.qss");
           file.open(QFile::ReadOnly);
           if(file.isOpen())
           {
                qss = QLatin1String(file.readAll());
                config_window->setStyleSheet(qss);
                file.close();
           }
           config_window->resize(1330, 490);
           config_window->show();    //管理员账号登陆
        }
        return app.exec();
    }
    else   return 0;
}

//基准数据库
QSqlDatabase DatabaseConnect()
{
    QSqlDatabase dbConnect;
    QString mdb_path;
    mdb_path = "D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles/"+creatName+"/Lj_QuestionnaireSys.mdb";
    dbConnect = QSqlDatabase::addDatabase("QODBC");//采用的接口和连接名

        dbConnect.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};\
        FIL={MS Access};DBQ=D:/Program Files (x86)/X_Ship/DB_51aspx/Lj_QuestionnaireSys.mdb");

//    else
//    {
//        dbConnect.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};\
//        FIL={MS Access};DBQ=D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles/"+"creatName+/Lj_QuestionnaireSys.mdb");
//    }
    QString strUserPassword="123";
    QString strUserName="123";
    dbConnect.setUserName(strUserName);//设置登陆数据库的用户名
    dbConnect.setPassword(strUserPassword);//设置密码
    bool bOpen=dbConnect.open(strUserName,strUserPassword);
    if(!bOpen)
        QMessageBox::information(NULL, QString("数据库"), QString("Lj_QuestionnaireSys.mdb数据库打开失败"));
    return dbConnect;
}
//基准数据库的初始化
void resetIndexStandardDB()
{
    QString sqlquery[7];
    QSqlQuery queryTmp(m_dbConnect);
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



/*
在关闭调查问卷第三方软件的时候，需要检查所有的原子指标是否对应上了题目
设置一个此cpp的全局变量，如果有原子指标没有对应上题目，则显示原子指标树
名称和对应的原子指标名称*/
QMap<QString, int> CheckQuestionnaire()
{
    const QString allIndexName[5]={"服务器设备指标树","控制设备指标树","显控设备指标树","网络架构指标树","信息流指标树"};
    QMap<QString,int> indexNameNotFound;//存储没有对应题目的原子指标名称和表名
    for(int i=0;i<5;i++)
    {
        QString strTmp="select nodeid,description from " + allIndexName[i]+" Where atomic=1";
        QSqlQuery oQuery(m_dbConnect);
        oQuery.exec(strTmp);
        oQuery.next();
        while(oQuery.isValid())
        {
            bool flag=false;
            QString resTmp;
            switch(i)
            {
                case 0:resTmp="select result_index from Lj_Result where result_sid between 1 and 10";break;
                case 1:resTmp="select result_index from Lj_Result where result_sid between 11 and 20";break;
                case 2:resTmp="select result_index from Lj_Result where result_sid between 21 and 30";break;
                case 3:resTmp="select result_index from Lj_Result where result_sid between 101 and 200";break;
                case 4:resTmp="select result_index from Lj_Result where result_sid between 201 and 300";break;
                default:break;
            }
            QSqlQuery oFindIndex(m_dbConnect);
            oFindIndex.exec(resTmp);
            oFindIndex.next();
            while(oFindIndex.isValid())
            {
                if(oFindIndex.value(0).toInt() == oQuery.value(0).toInt())
                {
                    flag=true;
                    break;
                }
                else
                {
                    if(oFindIndex.next());
                    else
                        break;
                }
            }
            //如果没有找到原子指标对应的题目
            if(!flag)
            {
                QString tableNameAndIndexName=allIndexName[i] + oQuery.value(1).toString();
                //删掉网络架构用模型评估
                if(3==i)
                {
                    if("网络架构指标树拓扑结构安全性"==tableNameAndIndexName||
                        "网络架构指标树冗余性"==tableNameAndIndexName||
                         "网络架构指标树平均距离"== tableNameAndIndexName||
                           "网络架构指标树聚类指数" == tableNameAndIndexName);
                    else
                    {
                        indexNameNotFound.insert(tableNameAndIndexName,i);
                    }
                }
                else
                {
                    indexNameNotFound.insert(tableNameAndIndexName,i);
                }
            }
            if(oQuery.next());
            else
                break;
        }
    }
    return indexNameNotFound;
}


