#include<vector>
#include<string>
#include<iostream>
#include<fstream>
#include<QTime>
#include<ctime>
#include<Windows.h>
#include<WinNT.h>
#include<WinBase.h>
#include"workflow.h"
#include"logicalRealtionship.h"
#include"graphviz.h"

extern System_Informa* sys_inf;
extern Index* m_message_index;
extern QList<Index*> index_select;


extern QMap<uint,uint> m_is_fault_device;   //故障设备的标志位
extern QVector<virusFunctionality>  m_availability_virus;
extern QVector<virusConfidentiality> m_confidentiality_virus;
extern QSet<pair<uint,uint>> m_secu_gateway;
QMap<uint,uint> fault_device;
QSet<uint> infect_device;
QSet<uint> infect_info;


using namespace std;
double MySrand()
{
    //QTime time;
    //time= QTime::currentTime();
    //qsrand(time.msec()+time.second()*1000);

    int nTemp = rand();
    if (nTemp > 0 && (nTemp % 100 == 0))
        return 1.0;
    return (nTemp % 100) / 100.0;
}

virus::virus(int a,QString b,int c,int d,float e):kind(a),virusName(b),sourceId(c),destination(d),influence(e){}

WorkflowTask::WorkflowTask(const QString &strTaskName, const uint strTaskNum)
{
    SYSTEMTIME sys;
    GetLocalTime(&sys);
    srand(sys.wMilliseconds);

	m_strTaskName = strTaskName;
	m_strTaskNum = strTaskNum;
}

WorkflowTask::~WorkflowTask()
{

}

const QString &WorkflowTask::GetTaskName()
{
    return m_strTaskName;
}

uint WorkflowTask::GetTaskNum()
{
    return m_strTaskNum;
}

WorkflowCom::WorkflowCom(const QString &strWorkflowName, const uint strWorkflowNum)
{
	m_strWorkflowName = strWorkflowName;
	m_strWorkflowNum = strWorkflowNum;

	m_poWorkflowGraph = new CWorkflowVizObj();

    //CreateWorkflowCom();
}

void WorkflowCom::GraphViz()
{
    char tmp[100];
    GetPrivateProfileStringA("dotFile", "name", "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());
    m_poWorkflowGraph->WorkflowGraphviz(m_oWorkflowTask, tmp);
}

WorkflowCom::~WorkflowCom()
{

}


void WorkflowCom::CreateWorkflowCom(const QVector<Task> &oInputTaskInfo)
{
	for (int i = 0; i < oInputTaskInfo.size(); ++i)
	{
		WorkflowTask oTemp(oInputTaskInfo[i].strTaskName, oInputTaskInfo[i].strTaskNum);
		m_oWorkflowTask.push_back(oTemp);
	}

	for (int i = 0; i < oInputTaskInfo.size(); ++i)
	{
        if (((oInputTaskInfo[i].strPreTask).size() == 1) && (oInputTaskInfo[i].strPreTask[0] == -1 ))
		{
			m_oWorkflowTask[i].m_nPreTaskIndex.push_back(-1);
			m_oWorkflowTask[i].m_unExecuteDevices = oInputTaskInfo[i].unExecuteDevice;
            m_oWorkflowTask[i].m_eDeviceRelation  = TranslateLogicalRelationFromStringToStruct(oInputTaskInfo[i].eDeviceRelation);
            m_oWorkflowTask[i].m_unPreInfoNum.push_back(-1);
		}
		else
		{
			for (int j = 0; j < (oInputTaskInfo[i].strPreTask).size(); ++j)
			{
				int nIndex = FindTaskByNum(oInputTaskInfo[i].strPreTask[j]);
				if (nIndex != -1)
				{
					m_oWorkflowTask[i].m_nPreTaskIndex.push_back(nIndex);
					m_oWorkflowTask[nIndex].m_nPostTask.push_back(i);
				}
			}
            m_oWorkflowTask[i].m_ePretaskRelation = TranslateLogicalRelationFromStringToStruct(oInputTaskInfo[i].eTaskRelation);
			m_oWorkflowTask[i].m_unExecuteDevices = oInputTaskInfo[i].unExecuteDevice;
            m_oWorkflowTask[i].m_eDeviceRelation  = TranslateLogicalRelationFromStringToStruct(oInputTaskInfo[i].eDeviceRelation);
            m_oWorkflowTask[i].m_unPreInfoNum = oInputTaskInfo[i].unPreInfoNum;
		}
	}
}

void WorkflowCom::CreateWorkflowCom()
{
    QVector<Task> oTaskInputInfo(6);
	oTaskInputInfo[0].strTaskName = "Task1";
    oTaskInputInfo[0].strTaskNum = 1;  //"T_1"  45621
    oTaskInputInfo[0].strPreTask.push_back(-1);
	oTaskInputInfo[0].unExecuteDevice.push_back(1011);
	oTaskInputInfo[0].unExecuteDevice.push_back(1012);
	oTaskInputInfo[0].unExecuteDevice.push_back(1013);
	oTaskInputInfo[0].unExecuteDevice.push_back(1014);
    //oTaskInputInfo[0].eDeviceRelation.nVoteM = 4;
    //oTaskInputInfo[0].eDeviceRelation.nVoteN = 3;
    //oTaskInputInfo[0].eDeviceRelation.rel = VOTE;
    oTaskInputInfo[0].eDeviceRelation = "VOTE, 4, 3";
    oTaskInputInfo[0].unPreInfoNum.push_back(-1);

	oTaskInputInfo[1].strTaskName = "Task2";
    oTaskInputInfo[1].strTaskNum = 2;  //"T_2" 5412
    oTaskInputInfo[1].strPreTask.push_back(1);  //45621
    //oTaskInputInfo[1].eTaskRelation.rel = SERIAL;
    oTaskInputInfo[1].eTaskRelation = "SERIAL";
	oTaskInputInfo[1].unExecuteDevice.push_back(1003);
    //oTaskInputInfo[1].eDeviceRelation.rel = SERIAL;
    oTaskInputInfo[1].eDeviceRelation = "SERIAL";
    oTaskInputInfo[1].unPreInfoNum.push_back(2001);
    oTaskInputInfo[1].unPreInfoNum.push_back(2002);
    oTaskInputInfo[1].unPreInfoNum.push_back(2003);
    oTaskInputInfo[1].unPreInfoNum.push_back(2004);

	oTaskInputInfo[2].strTaskName = "Task3";
    oTaskInputInfo[2].strTaskNum = 3;   //"T_3" 528
    oTaskInputInfo[2].strPreTask.push_back(1);  //45621
    //oTaskInputInfo[2].eTaskRelation.rel = SERIAL;
    oTaskInputInfo[2].eTaskRelation = "SERIAL";
	oTaskInputInfo[2].unExecuteDevice.push_back(1002);
    //oTaskInputInfo[2].eDeviceRelation.rel = SERIAL;
    oTaskInputInfo[2].eDeviceRelation = "SERIAL";
    oTaskInputInfo[2].unPreInfoNum.push_back(2005);


	oTaskInputInfo[3].strTaskName = "Task4";
    oTaskInputInfo[3].strTaskNum = 4;   //"T_4" 623
    oTaskInputInfo[3].strPreTask.push_back(2);  //5412
    //oTaskInputInfo[3].eTaskRelation.rel = SERIAL;
    oTaskInputInfo[3].eTaskRelation = "SERIAL";
	oTaskInputInfo[3].unExecuteDevice.push_back(1004);
    //oTaskInputInfo[3].eDeviceRelation.rel = SERIAL;
    oTaskInputInfo[3].eDeviceRelation = "SERIAL";
    oTaskInputInfo[3].unPreInfoNum.push_back(2006);

	oTaskInputInfo[4].strTaskName = "Task5";
    oTaskInputInfo[4].strTaskNum = 5;   //"T_5" 547
    oTaskInputInfo[4].strPreTask.push_back(2);  //5412
    //oTaskInputInfo[4].eTaskRelation.rel = SERIAL;
    oTaskInputInfo[4].eTaskRelation = "SERIAL";
	oTaskInputInfo[4].unExecuteDevice.push_back(1002);
    //oTaskInputInfo[4].eDeviceRelation.rel = SERIAL;
    oTaskInputInfo[4].eDeviceRelation = "SERIAL";
    oTaskInputInfo[4].unPreInfoNum.push_back(2007);

	oTaskInputInfo[5].strTaskName = "Task6";
    oTaskInputInfo[5].strTaskNum = 6;//"T_6"  //325
    oTaskInputInfo[5].strPreTask.push_back(3);  //528
    oTaskInputInfo[5].strPreTask.push_back(4);  //623
    oTaskInputInfo[5].strPreTask.push_back(5); //547
    //oTaskInputInfo[5].eTaskRelation.rel = AND;
    oTaskInputInfo[5].eTaskRelation = "AND";
	oTaskInputInfo[5].unExecuteDevice.push_back(1005);
    //oTaskInputInfo[5].eDeviceRelation.rel = SERIAL;
    oTaskInputInfo[5].eDeviceRelation = "SERIAL";
    oTaskInputInfo[5].unPreInfoNum.push_back(2008);
    oTaskInputInfo[5].unPreInfoNum.push_back(2009);


	CreateWorkflowCom(oTaskInputInfo);
}

/**
 * @brief WorkflowCom::CreateWorkflowCom
 * @param taskId
 * description:根据任务编号的集合，生成对应的工作流
 */
void WorkflowCom::CreateWorkflowCom(const QList<uint> &taskId)
{
    for (uint j = 0; j < taskId.size(); ++j)
    {
        Task_Informa taskInfo = sys_inf->Get_SingleTask(taskId[j]);
        WorkflowTask oTemp(taskInfo._Get_TaskName(), taskInfo._Get_TaskId());
        oTemp.m_strTaskName = taskInfo._Get_TaskName();
        oTemp.m_strTaskNum = taskInfo._Get_TaskId();
        oTemp.m_nPreTaskIndex = taskInfo._Get_PreTask();   //前驱任务集中存储的是前驱任务的编号
        //QList<uint> preTaskNum = taskInfo._Get_PreTask();
        //for (uint i = 0; i < preTaskNum.size(); ++i)]
        //{
        //    oTemp.m_nPreTaskIndex.push_back(FindTaskByNum(preTaskNum[i]));
        //}

        oTemp.m_ePretaskRelation = TranslateLogicalRelationFromStringToStruct(taskInfo._Get_TaskRelation());
        oTemp.m_unExecuteDevices = taskInfo._Get_TaskDevice();
        oTemp.m_eDeviceRelation  = TranslateLogicalRelationFromStringToStruct(taskInfo._Get_DeviceToTask());
        oTemp.m_unPreInfoNum = taskInfo._Get_PreInfo();

        m_oWorkflowTask.push_back(oTemp);
    }

    //将前驱任务集中的编号转换为对应的下标
    for (uint i = 0; i < m_oWorkflowTask.size(); ++i)
    {
        for (uint j = 0; j < m_oWorkflowTask[i].m_nPreTaskIndex.size(); ++j)
        {
            m_oWorkflowTask[i].m_nPreTaskIndex[j] = FindTaskByNum(m_oWorkflowTask[i].m_nPreTaskIndex[j]);
        }
    }
}

void WorkflowCom::AssessWorkflowCom()
{
    m_oTaskAvail.clear();
    //qDebug()<<"this one";
	bool flag = true;
	while (flag)
	{
		flag = false;
		for (int i = 0; i < m_oWorkflowTask.size(); ++i)
		{
            if (IsAssessAble(i))
			{
				if ((m_oTaskAvail.find(i) == m_oTaskAvail.end()))
				{
                    QVector<double> dDeviceAvail;
					int len = (m_oWorkflowTask[i].m_unExecuteDevices).size();
					for (int j = 0; j < len; ++j)
					{
                        //qDebug()<<"the device number"<<m_oDeviceStaticAvail[m_oWorkflowTask[i].m_unExecuteDevices[j]];
                        double dTemp = CalculateDeviceDynamicAvail(i, m_oWorkflowTask[i].m_unExecuteDevices[j]);  //是否应该是m_oWorkflowTask[i]，而不是i
						dDeviceAvail.push_back(dTemp);
					}
					double dTaskAvail = LogicalRelationship(dDeviceAvail, m_oWorkflowTask[i].m_eDeviceRelation);
                    //m_oTaskAvail.insert(make_pair(i, dTaskAvail));
                    m_oTaskAvail.insert(i/*m_oWorkflowTask[i].GetTaskNum()*/, dTaskAvail);
				}	
			}
			else
				flag = true;
		}
	}
    //如果一个工作流有多个终止任务，就选择多个终止任务中的可用性最低的任务作为工作流的可用性评估结果
    QList<int> lstIndex = FindLastTask();
    if (lstIndex.size() > 0)
        m_dWorkflowAvail = m_oTaskAvail[lstIndex[0]];
    for (int i = 1; i < lstIndex.size(); ++i)
    {
        if (m_dWorkflowAvail > m_oTaskAvail[lstIndex[i]])
            m_dWorkflowAvail = m_oTaskAvail[lstIndex[i]];
    }
}

void WorkflowCom::PrintAssessResult()
{
    for (QMap<unsigned int, double>::iterator iter = m_oTaskAvail.begin(); iter != m_oTaskAvail.end(); ++iter)
	{
        //cout << m_oWorkflowTask[iter->first].m_strTaskName << ": " << iter->second << endl;
        //cout << m_oWorkflowTask[iter.key()].m_strTaskName.toStdString() << ": " << iter.value() << endl;
        qDebug() << m_oWorkflowTask[iter.key()].m_strTaskName << ": " << iter.value() << endl;
        qDebug() << "pre task:";
        for (int i = 0; i < m_oWorkflowTask[iter.key()].m_nPreTaskIndex.size(); ++i)
		{
            qDebug() << m_oWorkflowTask[iter.key()].m_nPreTaskIndex[i] << " ";
		}
        qDebug() << endl;
        qDebug() << "post task:";
        for (int i = 0; i < m_oWorkflowTask[iter.key()].m_nPostTask.size(); ++i)
		{
            qDebug() << m_oWorkflowTask[iter.key()].m_nPostTask[i] << " ";
		}
        qDebug() << endl << endl;
    }
}

void WorkflowCom::PrintLogicMap()
{
    for (QMap<unsigned int, double>::iterator iter = m_oTaskAvail.begin(); iter != m_oTaskAvail.end(); ++iter)
	{
        qDebug() << "pre task:";
        for (int i = 0; i < m_oWorkflowTask[iter.key()].m_nPreTaskIndex.size(); ++i)
		{
            qDebug() << m_oWorkflowTask[iter.key()].m_nPreTaskIndex[i] << " ";
		}
        qDebug() << endl;
        qDebug() << "post task:";
        for (int i = 0; i < m_oWorkflowTask[iter.key()].m_nPostTask.size(); ++i)
		{
            qDebug() << m_oWorkflowTask[iter.key()].m_nPostTask[i] << " ";
		}
        qDebug() << endl << endl;
    }
}



void WorkflowCom::PrintDeviceAssessResult()
{
    for (QMap<unsigned int, double>::iterator iter = m_oDeviceDynamicAvail.begin(); iter != m_oDeviceDynamicAvail.end(); ++iter)
	{
        qDebug() << iter.key() << ": " << iter.value() << endl;
	}
}

double WorkflowCom::CalculateDeviceDynamicAvail(unsigned int nCurrentTaskIndex, unsigned int unDeviceNum)
{
	double dDeviceDynamicAvail = -1;
    dDeviceDynamicAvail = m_oDeviceStaticAvail[unDeviceNum];
    if ((m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex.size() == 1) && (m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[0] == -1))   //这里处理是否是有问题的
	{
        m_oDeviceDynamicAvail.insert(unDeviceNum, dDeviceDynamicAvail);
        return dDeviceDynamicAvail;
	}
	else 
	{
        QVector<double> dVecDeviceLinkByTask; //device*link for every task
		for (int i = 0; i < m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex.size(); ++i)
		{
            QVector<double> dVecDeviceLink;
			for (int j = 0; j < m_oWorkflowTask[m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[i]].m_unExecuteDevices.size(); ++j)
			{
				//unsigned int unLinkNum = UINT_MAX;
				double dLinkAvail = -1;
				unsigned int unDeviceIndex = m_oWorkflowTask[m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[i]].m_unExecuteDevices[j];
                if (FindLinkBetweenDevices(nCurrentTaskIndex, unDeviceIndex, unDeviceNum,dLinkAvail))
				{
					dVecDeviceLink.push_back(m_oDeviceDynamicAvail[unDeviceIndex] * dLinkAvail);
                    //m_oInfoStaticAvail.insert()
				}
			}
			dVecDeviceLinkByTask.push_back(LogicalRelationship(dVecDeviceLink, m_oWorkflowTask[m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[i]].m_eDeviceRelation));
		}
        dDeviceDynamicAvail *= LogicalRelationship(dVecDeviceLinkByTask, m_oWorkflowTask[nCurrentTaskIndex].m_ePretaskRelation);
	}
	if (dDeviceDynamicAvail != -1)
	{
        m_oDeviceDynamicAvail.insert(unDeviceNum, dDeviceDynamicAvail);
	}
    return dDeviceDynamicAvail;
}

bool WorkflowCom::FindLinkBetweenDevices(unsigned int nCurrentTaskIndex, 
										 unsigned int nLhsDeviceNum, 
										 unsigned int nRhsDeviceNum, 
										 double &dLinkNum)
{
	for (int i = 0; i < m_oWorkflowTask[nCurrentTaskIndex].m_unPreInfoNum.size(); ++i)
	{
        int current=m_oWorkflowTask[nCurrentTaskIndex].m_unPreInfoNum[i];
		//judge two devices whether if linked by the link, then give back the result of the link
        if (sys_inf->Get_SingleMessage(current)._Get_MessageSendDevice()==nLhsDeviceNum&&
            sys_inf->Get_SingleMessage(current)._Get_MessageReceiveDevice()==nRhsDeviceNum)
		{
            QString current_name=sys_inf->Get_SingleMessage(current)._Get_MessageName();
            if(m_message_index->m_ID_calculateResult.find(current_name)==m_message_index->m_ID_calculateResult.end()) dLinkNum=1;
            else dLinkNum=m_oInfoStaticAvail[current];
			//nLinkNum = m_oWorkflowTask[nCurrentTaskIndex].m_unPreInfoNum[i];
			//if the link hasn't been assessed, return false
			return true;
		}
	}
	return false;
}

bool WorkflowCom::IsAssessAble(unsigned int nCurrentTaskIndex)
{
	if (m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex.size() == 1 && (m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[0] == -1))
		return true;
	for (int i = 0; i < m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex.size(); ++i)
	{
        QMap<unsigned int, double>::iterator iter = m_oTaskAvail.find(m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[i]);
		if (iter == m_oTaskAvail.end())
			return false;
	}
	return true;
}


bool WorkflowCom::T_IsAssessAble(unsigned int nCurrentTaskIndex)
{
    if (m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex.size() == 1 && (m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[0] == -1))
        return true;
    for (int i = 0; i < m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex.size(); ++i)
    {
        QMap<unsigned int, double>::iterator iter = m_T_oTaskAvail.find(m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[i]);
        if (iter == m_T_oTaskAvail.end())
            return false;
    }
    return true;
}

LogicalRelation WorkflowCom::TranslateLogicalRelationFromStringToStruct(const QString &relation)
{
    LogicalRelation logicalRel;
    if (relation == "and" || relation == "AND")
    {
        logicalRel.rel = AND;
    }
    else if (relation == "or" || relation == "OR")
    {
        logicalRel.rel = OR;
    }
    else if (relation == "serial" || relation == "SERIAL")
    {
        logicalRel.rel = SERIAL;
    }
    else
    {
        QStringList strList;
        strList = relation.split(",", QString::SkipEmptyParts);
        if (strList.size() == 3)
        {
            if (strList[0] == "VOTE" || strList[0] == "vote")
            {
                logicalRel.rel = VOTE;
                bool oka, okb;
                int a = strList[1].toInt(&oka, 10);
                int b = strList[2].toInt(&okb, 10);
                if (oka && okb)
                {
                    logicalRel.nVoteM = a > b ? a : b;
                    logicalRel.nVoteN = a > b ? b : a;
                }
                else
                {
                    logicalRel.nVoteM = -1;
                    logicalRel.nVoteN = -1;
                }
            }
        }
    }
    return logicalRel;
}

QList<int> WorkflowCom::FindLastTask()
{
    QList<int> lstTaskNum;
    for (int i = 0; i < m_oWorkflowTask.size(); ++i)
	{
		if (m_oWorkflowTask[i].m_nPostTask.size() == 0)
            lstTaskNum.push_back(i);
	}
    return lstTaskNum;
}

int WorkflowCom::FindTaskByNum(const uint strTaskNum)
{
	for (int i = 0; i < m_oWorkflowTask.size(); ++i)
	{
		if (strTaskNum == m_oWorkflowTask[i].m_strTaskNum)
			return i;
	}
	return -1;
}

void WorkflowCom::GetInfoDeviceStaticAvail()
{
    for (uint i = 0; i < m_oWorkflowTask.size(); ++i)
    {
        for (uint j = 0; j < m_oWorkflowTask[i].m_unExecuteDevices.size(); ++j)
        {
            m_oDeviceStaticAvail.insert(m_oWorkflowTask[i].m_unExecuteDevices[j],
                                        LookupTableForStaticAvail(m_oWorkflowTask[i].m_unExecuteDevices[j]));
            m_oDeviceStaticCon.insert(m_oWorkflowTask[i].m_unExecuteDevices[j],
                                        LookupTableForStaticCon(m_oWorkflowTask[i].m_unExecuteDevices[j]));
        }
        for (uint j = 0; j < m_oWorkflowTask[i].m_unPreInfoNum.size(); ++j)
        {
            m_oInfoStaticAvail.insert(m_oWorkflowTask[i].m_unPreInfoNum[j],
                                      LookupTableForInfoStaticAvail(m_oWorkflowTask[i].m_unPreInfoNum[j]));
            m_oInfoStaticCon.insert(m_oWorkflowTask[i].m_unPreInfoNum[j],
                                      LookupTableForInfoStaticCon(m_oWorkflowTask[i].m_unPreInfoNum[j]));
        }
    }
}

void WorkflowCom::SetAvailRequirement(double val)
{
    m_AvailRequirement = val;
}

QString WorkflowCom::GetWorkflowName() const
{
    return m_strWorkflowName;
}

double WorkflowCom::LookupTableForLinkBetweenDevices(const QString &strLeftDeviceNum, const QString &strRighttDeviceNum)
{
    //return 1.0;
    return MySrand();
}


double WorkflowCom::LookupTableForStaticAvail(unsigned int unDeviceNum)    //if fail return false
{
    //return MySrand();
    QString current_device=sys_inf->Get_SingleDevice(unDeviceNum)._Get_DeviceName();
    for(int i=0;i!=3;i++)
    {
        if(index_select[i]->m_ID_calculateResult.find(current_device)!=index_select[i]->m_ID_calculateResult.end())
            return index_select[i]->m_ID_calculateResult[current_device][2]/100.0;
    }
    return 1;
}

double WorkflowCom::LookupTableForStaticCon(unsigned int unDeviceNum)    //if fail return false
{
    //return MySrand();
    QString current_device=sys_inf->Get_SingleDevice(unDeviceNum)._Get_DeviceName();
    for(int i=0;i!=3;i++)
    {
        if(index_select[i]->m_ID_calculateResult.find(current_device)!=index_select[i]->m_ID_calculateResult.end())
        {
            if(index_select[i]->m_ID_calculateResult[current_device][8]<0) return 0;
            return index_select[i]->m_ID_calculateResult[current_device][8]/100.0;
        }
    }
    return 0;
}


double WorkflowCom::LookupTableForInfoStaticAvail(unsigned int unInfoNum)
{
    //return 1.0;
    QString current_mess=sys_inf->Get_SingleMessage(unInfoNum)._Get_MessageName();
    if(m_message_index->m_ID_calculateResult.find(current_mess)==m_message_index->m_ID_calculateResult.end())
        return 1;
    else return m_message_index->m_ID_calculateResult[current_mess][2]/100.0;
}

double WorkflowCom::LookupTableForInfoStaticCon(unsigned int unInfoNum)
{
    //return 1.0;
    QString current_mess=sys_inf->Get_SingleMessage(unInfoNum)._Get_MessageName();
    if(m_message_index->m_ID_calculateResult.find(current_mess)==m_message_index->m_ID_calculateResult.end())
        return 0;
    else return m_message_index->m_ID_calculateResult[current_mess][4]/100.0;
}

pair<QString, pair<double, double> > WorkflowCom::GetWorkflowAvail() const
{
    pair<QString, pair<double, double> > workflowAvail;
    workflowAvail.first = m_strWorkflowName;
    workflowAvail.second.first = m_AvailRequirement;
    //qDebug()<<"begin:"<<m_AvailRequirement<<" "<<m_dWorkflowAvail<<endl;
    workflowAvail.second.second = m_dWorkflowAvail;

    return workflowAvail;
}

//void WorkflowCom::ResultDisplayByLineChart(QString strTitle, QChartView *pChartView)
//{

//}

//void WorkflowCom::ResultDisplayByBarChart(QString strTitle, QChartView *pChartView)
//{

//}

//void WorkflowCom::ResultDisplayByPieChart(QString strTitle, QChartView *pChartView)
//{

//}

//void WorkflowCom::ResultDisplayByPolarChart(QString strTitle, QChartView *pChartView)
//{

//}


void WorkflowCom::T_inital()
{
    fault_device.clear();
    infect_device.clear();
    infect_info.clear();
    fault_device=m_is_fault_device;
    AssessWorkflowCom();
    if(m_availability_virus.size()==0)
    {
        if(m_confidentiality_virus.size()==0)
        {

        }
        else
        {
            cur_virus=new virus(1,m_confidentiality_virus[0].virusName,m_confidentiality_virus[0].sourceId,m_confidentiality_virus[0].destination
                    ,m_confidentiality_virus[0].influence);
            infect_device.insert(m_confidentiality_virus[0].sourceId);
        }
    }
    else
    {
        cur_virus=new virus(0,m_availability_virus[0].virusName,m_availability_virus[0].sourceId,m_availability_virus[0].destination
                ,m_availability_virus[0].influence);
        infect_device.insert(m_availability_virus[0].sourceId);
    }

    T_AssessWorkflowCom();


    CInfoInternetVizObj *a=new CInfoInternetVizObj();
    a->SituationWithProperty(m_oDeviceStaticAvail,m_oInfoStaticAvail);
    delete a;
}




void WorkflowCom::T_AssessWorkflowCom()
{
    m_T_oTaskAvail.clear();
    m_T_oDeviceDynamicAvail.clear();
    bool flag = true;
    while (flag)
    {
        flag = false;
        for (int i = 0; i < m_oWorkflowTask.size(); ++i)
        {
            //qDebug()<<"测试任务名"<<m_oWorkflowTask[i].GetTaskName()<<endl;
            if (T_IsAssessAble(i))
            {
                if ((m_T_oTaskAvail.find(i) == m_T_oTaskAvail.end()))
                {
                    QVector<double> dDeviceAvail;
                    int len = (m_oWorkflowTask[i].m_unExecuteDevices).size();
                    for (int j = 0; j < len; ++j)
                    {
                        //qDebug()<<"the device number"<<m_oWorkflowTask[i].m_unExecuteDevices[j];
                        double dTemp = T_CalculateDeviceDynamicAvail(i, m_oWorkflowTask[i].m_unExecuteDevices[j]);  //是否应该是m_oWorkflowTask[i]，而不是i
                        dDeviceAvail.push_back(dTemp);
                    }
                    double dTaskAvail = LogicalRelationship(dDeviceAvail, m_oWorkflowTask[i].m_eDeviceRelation);
                    //m_oTaskAvail.insert(make_pair(i, dTaskAvail));
                    m_T_oTaskAvail.insert(i/*m_oWorkflowTask[i].GetTaskNum()*/, dTaskAvail);
                }
            }
            else
                flag = true;
        }
    }
    //如果一个工作流有多个终止任务，就选择多个终止任务中的可用性最低的任务作为工作流的可用性评估结果
    QList<int> lstIndex = FindLastTask();
    if (lstIndex.size() > 0)
        m_T_dWorkflowAvail = m_T_oTaskAvail[lstIndex[0]];
    for (int i = 1; i < lstIndex.size(); ++i)
    {
        if (m_T_dWorkflowAvail > m_T_oTaskAvail[lstIndex[i]])
            m_T_dWorkflowAvail = m_T_oTaskAvail[lstIndex[i]];
    }
}


double WorkflowCom::T_CalculateDeviceDynamicAvail(unsigned int nCurrentTaskIndex, unsigned int unDeviceNum)
{
    //qDebug()<<"LRU"<<endl;
    if(m_T_oDeviceDynamicAvail.find(unDeviceNum)!=m_T_oDeviceDynamicAvail.end()) return m_T_oDeviceDynamicAvail[unDeviceNum];
    if(m_is_fault_device[unDeviceNum]==1) {
        m_T_oDeviceDynamicAvail.insert(unDeviceNum, 0);
        return 0;
    }
    //qDebug()<<"here is calculation device dynamic"<<endl;
    double dDeviceDynamicAvail = -1;
    bool flag=false;   //标志是否有故障传播的可能性
    dDeviceDynamicAvail = m_oDeviceStaticAvail[unDeviceNum];
    if ((m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex.size() == 1) && (m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[0] == -1))   //这里处理是否是有问题的
    {
        m_T_oDeviceDynamicAvail.insert(unDeviceNum, dDeviceDynamicAvail);
        return dDeviceDynamicAvail;
    }
    else
    {
        QVector<double> dVecDeviceLinkByTask; //device*link for every task
        for (int i = 0; i < m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex.size(); ++i)
        {
            QVector<double> dVecDeviceLink;
            for (int j = 0; j < m_oWorkflowTask[m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[i]].m_unExecuteDevices.size(); ++j)
            {
                //unsigned int unLinkNum = UINT_MAX;
                double dLinkAvail = -1;
                uint infoNum=0;
                unsigned int unDeviceIndex = m_oWorkflowTask[m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[i]].m_unExecuteDevices[j];
                //qDebug()<<unDeviceIndex<<" "<<unDeviceNum<<endl;
                if (T_FindLinkBetweenDevices(nCurrentTaskIndex, unDeviceIndex, unDeviceNum,dLinkAvail,infoNum))
                {
                    //qDebug()<<"here is come in"<<endl;
                    if(fault_device[unDeviceIndex]==1&&fault_device[unDeviceNum]==0&&flag==false)  flag=true;
                    if(infect_device.find(unDeviceIndex)!=infect_device.end()&&infect_device.find(unDeviceNum)==infect_device.end())
                    {
                        if((m_secu_gateway.find(pair<uint,uint>(unDeviceIndex,unDeviceNum))==m_secu_gateway.end())
                                &&(m_oInfoStaticCon[infoNum]<cur_virus->influence))
                        {
                            infect_info.insert(infoNum);
                            if(m_oDeviceStaticCon[unDeviceNum]<cur_virus->influence)
                            {
                                if(cur_virus->destination==unDeviceNum&&cur_virus->kind==0)
                                {
                                    m_T_oDeviceDynamicAvail.insert(unDeviceNum, 0);
                                    infect_device.insert(unDeviceNum);
                                    return 0;
                                }
                                else if(cur_virus->destination==unDeviceNum&&cur_virus->kind==1) {
                                     infect_device.insert(unDeviceNum);
                                }
                                else infect_device.insert(unDeviceNum);
                            }
                        }
                    }
                    dVecDeviceLink.push_back(m_T_oDeviceDynamicAvail[unDeviceIndex] * dLinkAvail);
                    //m_oInfoStaticAvail.insert()
                }
            }
            dVecDeviceLinkByTask.push_back(LogicalRelationship(dVecDeviceLink, m_oWorkflowTask[m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[i]].m_eDeviceRelation));
        }
        dDeviceDynamicAvail *= LogicalRelationship(dVecDeviceLinkByTask, m_oWorkflowTask[nCurrentTaskIndex].m_ePretaskRelation);
    }
    if (dDeviceDynamicAvail != -1)
    {
        m_T_oDeviceDynamicAvail.insert(unDeviceNum, dDeviceDynamicAvail);
        if(flag==true&&m_T_oDeviceDynamicAvail[unDeviceNum]!=m_oDeviceDynamicAvail[unDeviceNum])
        {
            fault_device[unDeviceNum]=1;
        }
    }
    return dDeviceDynamicAvail;
}

bool WorkflowCom::T_FindLinkBetweenDevices(unsigned int nCurrentTaskIndex,
                                         unsigned int nLhsDeviceNum,
                                         unsigned int nRhsDeviceNum,
                                         double &dLinkNum,uint &infoNum)
{
    for (int i = 0; i < m_oWorkflowTask[nCurrentTaskIndex].m_unPreInfoNum.size(); ++i)
    {
        int current=m_oWorkflowTask[nCurrentTaskIndex].m_unPreInfoNum[i];
        //judge two devices whether if linked by the link, then give back the result of the link
        if (sys_inf->Get_SingleMessage(current)._Get_MessageSendDevice()==nLhsDeviceNum&&
            sys_inf->Get_SingleMessage(current)._Get_MessageReceiveDevice()==nRhsDeviceNum)
        {
            QString current_name=sys_inf->Get_SingleMessage(current)._Get_MessageName();
            if(m_message_index->m_ID_calculateResult.find(current_name)==m_message_index->m_ID_calculateResult.end()) dLinkNum=1;
            else dLinkNum=m_oInfoStaticAvail[current];
            infoNum=current;
            //nLinkNum = m_oWorkflowTask[nCurrentTaskIndex].m_unPreInfoNum[i];
            //if the link hasn't been assessed, return false
            return true;
        }
    }
    return false;
}



