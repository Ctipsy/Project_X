#ifndef _WORKFLOW_H
#define _WORKFLOW_H

#include<vector>
#include<string>
#include<unordered_map>
#include<map>

#include <QString>
#include <QVector>
#include <QMap>
#include <QtCharts>

#include"workflow/logicalRealtionship.h"
#include"graphviz/workflow_graphviz.h"
#include "sys_discript/system_informa.h"
#include "result_display/result_display.h"
#include "index/index.h"
#include "graphviz/internet_info_graphviz.h"

using namespace std;

QT_CHARTS_USE_NAMESPACE

class CWorkflowVizObj;

/*
struct BusinessProcess
{
    QString BusinessName;
    uint BusinessNumber;
    double AvailRequirement;
    QList<Task> task;
};*/


struct virusFunctionality
{
    QString virusName;//病毒名称
    uint sourceId;//病毒的注入节点ID
    uint destination;//病毒的攻击节点ID
    float influence;//病毒影响力
};
//影响机密性的病毒输入
struct virusConfidentiality
{
    QString virusName;//病毒名称
    uint sourceId;//病毒的注入节点ID
    uint destination;//病毒的攻击节点ID
    float influence;//病毒影响力
};

struct virus
{
    uint kind;
    QString virusName;//病毒名称
    uint sourceId;//病毒的注入节点ID
    uint destination;//病毒的攻击节点ID
    float influence;//病毒影响力
    virus(int a,QString b,int c,int d,float e);
};

struct Task
{
    QString strTaskName;    //任务名称
    uint strTaskNum;       //任务编号
    //uint BusinessNumber;
    QList<uint> strPreTask;   //第一个任务没有前驱
    //LogicalRelation eTaskRelation;
    QString eTaskRelation;     //任务之间的逻辑关系
    QList<uint> unExecuteDevice;   //执行该任务的所有设备
    //LogicalRelation eDeviceRelation;
    QString eDeviceRelation;     //设备之间的逻辑关系
    QList<uint> unPreInfoNum;     //该任务需要的前驱信息
};

class  WorkflowTask
{
public:
    WorkflowTask(){}
    WorkflowTask(const QString &strTaskName, const uint strTaskNum);
	~WorkflowTask();
    const QString& GetTaskName();
    uint GetTaskNum();
	
private:
    QString m_strTaskName;    //任务名称
    uint m_strTaskNum;      //任务编号

    QList<uint> m_nPreTaskIndex;  //存储前驱任务的下标
    LogicalRelation m_ePretaskRelation;   //前驱任务之间的逻辑关系
    QList<uint> m_nPostTask;    //后继任务的下标

    QList<uint> m_unPreInfoNum;   //前驱信息的编号

    QList<uint> m_unExecuteDevices;    //存储执行该任务的设备编号
    LogicalRelation m_eDeviceRelation;  //设备之间的逻辑关系

	//double m_dTaskAvail;

    friend class WorkflowCom;
	friend class CWorkflowVizObj;
};

class WorkflowCom
{

public:
    WorkflowCom(const QString &strWorkflowName, const uint strWorkflowNum);
	~WorkflowCom();
    //void PrepareInputInfo(QVector<Task> &oInputTaskInfo);
    //void CreateWorkflowCom(const QString &strSrcFile);
    void CreateWorkflowCom(const QVector<Task> &oInputTaskInfo);
    void CreateWorkflowCom(const QList<uint> &taskId);   //与XML文件对接，通过读取XML文件中的数据初始化工作流
	void CreateWorkflowCom();

    void AssessWorkflowCom();      //根据工作流涉及的设备和信息对工作流可用性进行评估
    void T_AssessWorkflowCom();      //态势仿真中根据工作流涉及的设备和信息对工作流可用性进行评估
    void GraphViz();             //画工作流模型图

    //调试时，用于打印数据的
    void PrintAssessResult();
	void PrintDeviceAssessResult();
	void PrintLogicMap();

    pair<QString, pair<double, double> > GetWorkflowAvail() const;  //返回值分别表示工作流名字，可用性需求，可用性结果

    //display the result of assessment
//    void ResultDisplayByLineChart(QString strTitle, QChartView *pChartView);   //line chart
//    void ResultDisplayByBarChart(QString strTitle, QChartView *pChartView);    //bar chart
//    void ResultDisplayByPieChart(QString strTitle, QChartView *pChartView);    //pie chart
//    void ResultDisplayByPolarChart(QString strTitle, QChartView *pChartView);  //polar chart

    void GetInfoDeviceStaticAvail();          //获取设备和信息的静态可用性
    void SetAvailRequirement(double val);     //设置工作流的可用性需求
    QString GetWorkflowName() const;
    void T_inital();

private:
    //计算设备的动态可用性
	double CalculateDeviceDynamicAvail(unsigned int nCurrentTaskIndex, unsigned int unDeviceNum);
    //计算态势仿真中设备的动态可用性
    double T_CalculateDeviceDynamicAvail(unsigned int nCurrentTaskIndex, unsigned int unDeviceNum);
    //判断任务评估所需要的依赖数据是否准备好
    bool IsAssessAble(unsigned int nCurrentTaskIndex);
    bool T_IsAssessAble(unsigned int nCurrentTaskIndex);

    //将用字符串描述的逻辑关系转换为结构体形式
    LogicalRelation TranslateLogicalRelationFromStringToStruct(const QString &relation);

    //寻找最后一个任务
    QList<int> FindLastTask();

    //通过任务编号寻找任务下标
    int FindTaskByNum(const uint strTaskNum);

    //查表取得设备之间的信息可用性评估结果
    double LookupTableForLinkBetweenDevices(const QString &strLeftDeviceNum, const QString &strRighttDeviceNum);

    //查找两个任务之间是否有链路
    bool FindLinkBetweenDevices(unsigned int nCurrentTaskIndex, unsigned int unLhsDeviceNum, unsigned int unRhsDeviceNum, double &dLinkNum);
    bool T_FindLinkBetweenDevices(unsigned int nCurrentTaskIndex, unsigned int unLhsDeviceNum, unsigned int unRhsDeviceNum, double &dLinkNum,uint &infoNum);

    //没有具体数据
    double LookupTableForStaticAvail(unsigned int unDeviceNum);    //if fail return -1
    double LookupTableForStaticCon(unsigned int unDeviceNum);
    double LookupTableForInfoStaticAvail(unsigned int unInfoNum);  //通过信息编号查询信息的可用性评估结果
    double LookupTableForInfoStaticCon(unsigned int unInfoNum);  //通过信息编号查询信息的机密性评估结果



private:
    //2018.01.01改为public属性，因为在mainwindow处生成PDF报告时不能访问private属性
    //QString m_strWorkflowName;     //工作流的名称
    uint m_strWorkflowNum;   //工作流编号
    //QList<WorkflowTask> m_oWorkflowTask;    //工作流中包含的任务
    //2018.01.01改为public属性，因为在mainwindow处生成PDF报告时不能访问private属性
    //QMap<uint, double> m_oTaskAvail;     //任务的可用性评估结果

    QMap<uint, double> m_T_oTaskAvail;     //态势仿真中任务的可用性评估结果

    double m_AvailRequirement;     //工作流的可用性评估需求
    double m_dWorkflowAvail = -1;   //工作流可用性评估结果

    double m_T_dWorkflowAvail = -1;   //态势仿真工作流可用性评估结果

public:
    QMap<uint, double> m_oDeviceStaticAvail;    //设备的静态可用性评估结果
    QMap<uint, double> m_oInfoStaticAvail;     //信息的静态可用性评估结果
    QMap<uint, double> m_oDeviceStaticCon;    //设备的静态机密性评估结果
    QMap<uint, double> m_oInfoStaticCon;     //信息的静态机密性评估结果
    virus *cur_virus=NULL;
    QMap<uint, double> m_oDeviceDynamicAvail;     //设备动态可用性评估结果

    QMap<uint, double> m_T_oDeviceDynamicAvail;     //态势仿真中设备动态可用性评估结果
    QMap<uint, double> m_oTaskAvail;     //任务的可用性评估结果
    QList<WorkflowTask> m_oWorkflowTask;    //工作流中包含的任务
    QString m_strWorkflowName;     //工作流的名称

private:
    CWorkflowVizObj *m_poWorkflowGraph;        //工作流模型可视化类对应的指针
    CResultDisplay *m_poResultDisplay;        //评估结果可视化类

    friend class CAssessResult;
    friend class CWorkflowAssess;
};



#endif 
