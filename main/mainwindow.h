#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QtXml>
#include <QDomDocument>
#include <QDebug>
#include <QPrinter>
#include "wuli_assess/wuli.h"

#include "internet_assess/internet_access.h"
#include "message_assess/messageassess.h"
#include "situation_simulation/situation.h"
#include "sys_discript/system_informa.h"
#include "workflow/wflow.h"
#include "newproject/newproject.h"
#include "newproject/project_guide.h"
#include "newproject/project_info.h"
#include "licence/Licence.h"
#include <unordered_map>
#include "message_assess/messageassess.h"
//#include"svg_show/svgwindow.h"  //8.6添加



//信息流部分生成pdf报告时，信道容量结构体
struct InfCapacity
{
    QString deviceName1;
    QString deviceName2;
    float maxCapacity;
    float usedCapacity;
    float leftCapacity;
};
//信息流部分生成pdf报告时，协议特点分析结构体
struct protocalInf
{
    QString protocalName;//协议名称
    QString protocalScale;//此协议所占的比例，显示为百分比形式
    QString protocalProblem;//协议存在的问题
    QString protocalSuggestion;//协议的安全性建议
};
//工作流相关设备的保密性
struct deviceSecurity
{
    QString deviceName;//设备名称
    QString infMaxSecurity;//信息最大保密性
    QString currentSecurity;//设备当前的保密性
    QString satisfied;//是否满足
};
//工作流相关信息的保密性
struct infSecurity
{
    QString infName;//信息名称
    QString infSecurityDemand;//信息机密性需求
    QString currentSecurity;//当前信息的机密性
    QString satisfied;//是否满足
};
//工作流部分生成pdf报告时的结构体
struct workFlowPDF
{
    QString workFlowName;//工作流的名称
    QMap<QString,QString> deviceNameAndValue;//工作流可用性评估，设备名称和设备的动态可用性
    QMap<QString,QString> taskNameAndValue;//工作流可用性评估，任务名称和任务的可用性
    QVector<deviceSecurity> deviceSecurityInf;//设备机密性表格的内容
    QVector<infSecurity> infSecurityInf;//信息机密性表格的内容
};

struct workFlowRes
{
    QString workFlowName;//工作流名称
    QString workFlowReqAvail;//工作流可用性需求
    QString workFlowActualAvail;//工作流实际可用性
    QString satisfied;//是否满足
};

QString getPDFHtmlForInternet();
QMap<QString,double> getInternetResFromDB();
QMap<QString,double> getIndexInternetResFromDB();

QVector<InfCapacity> getInfCapacityOfMessage();//信道表格数据来源
QVector<protocalInf> getProtocalInfForPDF();//用到的协议信息
QMap<QString, QStringList> getDeviceSortForPDF();//返回按不同需求推荐的设备排序
QStringList set_sort(QList<double> &temp_reli);
bool CompaireByValue(pair<double,uint> &a1,pair<double,uint> &a2);
QMap<QString, QMap<QString, QString> > getTotalMessRes();//返回每个交互信息中三大一级指标的名称和分数
QMap<QString,QMap<QString,QString> > getLowScoreIndexMess();//返回每个交互信息中低于60分的原子指标及分数
QSet<QString> getIndexSuggestion();//低于60分的原子指标安全性建议，返回的是所有交互信息中低于60分的原子指标名称
QSqlDatabase ProjectDatabaseConnect();//工程目录下的数据库文件
void resetIndex();//当新建工程时，初始化工程文件中的数据库
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    void closeEvent(QCloseEvent *event);

public:
    explicit MainWindow(QWidget *parent = 0);
    void change_window();
    void initial();
    ~MainWindow();
    void workModuleJudge();
    QVector<protocalInf> getProtocalInfForPDF();
    QMap<QString,QMap<QString,QString> > getWuliIndexInf();
    QMap<QString,QMap<QString,QString> >getLowScoreIndexWuli();
    QSet<QString> getSecSuggestionWuli();
    QVector<workFlowPDF> getWorkFlowPDF();
    QVector<workFlowRes> getWorkFlowRes();
private slots:
   // void on_open_file_triggered();
    void on_bmessage_assess_clicked();
    void on_bwuli_assess_clicked();
    void on_bnet_assess_clicked();
    void on_bworkflow_clicked();
    void on_btaishi_clicked();
    //void on_open_file_triggered();
    void on_newProject_triggered();

    void on_openProject_triggered();

    void on_close_project_triggered();

    void on_bproject_info_clicked();

    void on_action_oenHelp_triggered();

    void on_action_findPDF_triggered();

    void on_action_createRecord_triggered();

    void on_action_viewRecord_triggered();

private:
    Ui::MainWindow *ui;
    messageassess* mess_assess=NULL;
    InternetAccess* m_poInternetDlg=NULL;
    wuli* wuli_assess=NULL;
    Situation* situation_simulation=NULL;
    WFlow *m_poWFlowDlg=NULL;
    project_info *project_information=NULL;
    Licence *lic;
    int num=0;
    int Actives;
};


#endif // MAINWINDOW_H
