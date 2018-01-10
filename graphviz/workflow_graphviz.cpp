#include <fstream>
#include <QString>

#include <Windows.h>
#include <WinNT.h>
#include <WinBase.h>

using namespace std;

#include"graphviz/workflow_graphviz.h"


void CWorkflowVizObj::WorkflowGraphviz(QList<WorkflowTask> &m_oWorkflowTask, const QString &strDotFileName)
{
//	char strName[255];
//    sprintf_s(strName, sizeof(strName), "%s.dot", strDotFileName.toStdString().c_str());
//	ofstream oOutFileStream(strName);
   // QString strFileName = QString::fromStdString(strDotFileName);
    QString strName = QString("%1.dot").arg(strDotFileName);
    char tmp[255];
    GetPrivateProfileStringA("projectInfo", "projectAddr", "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());
    strName = QString(QLatin1String(tmp)) + "/dot/" + strName;

    ofstream oOutFileStream(strName.toStdString().c_str());


	oOutFileStream << "digraph qing{" << endl;
	oOutFileStream << "\trankdir = LR;" << endl;
    oOutFileStream << "\graph [bgcolor = transparent]"<< endl;

	

    for (unsigned int i = 0; i < m_oWorkflowTask.size(); ++i)
	{
		SetNodeProperty(oOutFileStream, "task_node");
		SetEdgeProperty(oOutFileStream, "task_edge");
        if (m_oWorkflowTask[i].m_nPreTaskIndex.size() == 1 && m_oWorkflowTask[i].m_nPreTaskIndex[0] == -1)
        //if (m_oWorkflowTask[i].m_nPreTaskIndex.size() == 1 && m_oWorkflowTask[i].m_nPreTaskIndex[0] == 0)
        {
			oOutFileStream << "\tstart [shape = \"none\", image = \"" ;

			char tmp[100];
            GetPrivateProfileStringA("image", "startImage", "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());
			oOutFileStream << tmp << "\", labelloc = b];" << endl;
            oOutFileStream << "\t" << m_oWorkflowTask[i].m_strTaskNum << "[label = " <<
                              m_oWorkflowTask[i].m_strTaskName.toStdString() << "];" << endl;
            oOutFileStream << "\tstart -> " << m_oWorkflowTask[i].m_strTaskNum << ";" << endl;
		}
		else
		{
			DealWithTaskRelation(m_oWorkflowTask, i, oOutFileStream);
		}

		SetNodeProperty(oOutFileStream, "device_node");
		SetEdgeProperty(oOutFileStream, "device_edge");
		DealWithTaskDeviceRelation(m_oWorkflowTask, i, oOutFileStream);

		oOutFileStream << endl;	
	}
	oOutFileStream << "}"	<< endl;
}



void CWorkflowVizObj::DealWithTaskRelation(QList<WorkflowTask> &m_oWorkflowTask, unsigned int nCurrentTaskIndex, ofstream &oOutFileStream)
{
	string logicalRelation;
	switch(m_oWorkflowTask[nCurrentTaskIndex].m_ePretaskRelation.rel)
	{
	case AND:
        FoutputAndImage(m_oWorkflowTask[nCurrentTaskIndex].m_strTaskNum, "task", oOutFileStream);
		logicalRelation = "and_task_";
		break;
	case OR:
        FoutputOrImage(m_oWorkflowTask[nCurrentTaskIndex].m_strTaskNum, "task", oOutFileStream);
		logicalRelation = "or_task_";
		break;
	case VOTE:
        FoutputVoteImage(m_oWorkflowTask[nCurrentTaskIndex].m_strTaskNum, "task", oOutFileStream);
		logicalRelation = "vote_task_";
		break;
	case SERIAL:
		if (m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex.size() == 1)
        {
            oOutFileStream << "\t" << m_oWorkflowTask[m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[0]].m_strTaskNum << "[label = " <<
                                                     m_oWorkflowTask[m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[0]].m_strTaskName.toStdString() << "];" << endl;
            oOutFileStream << "\t" << m_oWorkflowTask[nCurrentTaskIndex].m_strTaskNum << "[label = " <<
                              m_oWorkflowTask[nCurrentTaskIndex].m_strTaskName.toStdString() << "];" << endl;
            oOutFileStream << "\t" << m_oWorkflowTask[m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[0]].m_strTaskNum << "->"
            << m_oWorkflowTask[nCurrentTaskIndex].m_strTaskNum << ";" << endl;
        }
		return;
	default:
		break;
	}
	for (int i = 0; i < m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex.size(); ++i)
	{
        oOutFileStream << "\t" << m_oWorkflowTask[m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[i]].m_strTaskNum << "[label = " << m_oWorkflowTask[m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[i]].m_strTaskName.toStdString() << "];" << endl;
        oOutFileStream << "\t" << m_oWorkflowTask[nCurrentTaskIndex].m_strTaskNum << "[label = " << m_oWorkflowTask[nCurrentTaskIndex].m_strTaskName.toStdString() << "];" << endl;
        oOutFileStream << "\t" << m_oWorkflowTask[m_oWorkflowTask[nCurrentTaskIndex].m_nPreTaskIndex[i]].m_strTaskNum << " -> " << logicalRelation
            << m_oWorkflowTask[nCurrentTaskIndex].m_strTaskNum << ";" << endl;
	}

}
void CWorkflowVizObj::DealWithTaskDeviceRelation(QList<WorkflowTask> &m_oWorkflowTask, unsigned int nCurrentTaskIndex, ofstream &oOutFileStream)
{
	string logicalRelation;
	switch(m_oWorkflowTask[nCurrentTaskIndex].m_eDeviceRelation.rel)
	{
	case AND:
        FoutputAndImage(m_oWorkflowTask[nCurrentTaskIndex].m_strTaskNum, "device", oOutFileStream);
		logicalRelation = "and_device_";
		break;
	case OR:
        FoutputOrImage(m_oWorkflowTask[nCurrentTaskIndex].m_strTaskNum, "device", oOutFileStream);
		logicalRelation = "or_device_";
		break;
	case VOTE:
        FoutputVoteImage(m_oWorkflowTask[nCurrentTaskIndex].m_strTaskNum, "device", oOutFileStream);
		logicalRelation = "vote_device_";
		break;
	case SERIAL:
		if (m_oWorkflowTask[nCurrentTaskIndex].m_unExecuteDevices.size() == 1)
        {
            //oOutFileStream << "\t" << "device_" << m_oWorkflowTask[nCurrentTaskIndex].m_unExecuteDevices[0] << ";" << endl;
            oOutFileStream << "\t" << "Dev_" << m_oWorkflowTask[nCurrentTaskIndex].m_unExecuteDevices[0] << "->"
            << m_oWorkflowTask[nCurrentTaskIndex].m_strTaskNum << ";" << endl;
        }

		return;
	default:
		break;
	}
    for (int i = 0; i < m_oWorkflowTask[nCurrentTaskIndex].m_unExecuteDevices.size(); ++i)

	{
        oOutFileStream << "\t" << "Dev_" << m_oWorkflowTask[nCurrentTaskIndex].m_unExecuteDevices[i] << " -> " << logicalRelation
            << m_oWorkflowTask[nCurrentTaskIndex].m_strTaskNum << ";" << endl;

	}
}
