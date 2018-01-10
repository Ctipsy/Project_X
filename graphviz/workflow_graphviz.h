#ifndef _WORKFLOW_GRAPHVIZ
#define _WORKFLOW_GRAPHVIZ

#include <QString>
#include <QVector>

#include"graphviz/graphviz.h"
#include"workflow/workflow.h"

class WorkflowTask;

class CWorkflowVizObj:public VizObject
{
public:
	
	//void GraphViz(const string &strFileName = "workflowModel");
    void WorkflowGraphviz(QList<WorkflowTask> &m_oWorkflowTask, const QString &strDotFileName);
private:
	
    void DealWithTaskRelation(QList<WorkflowTask> &m_oWorkflowTask,
								unsigned int nCurrentTaskIndex, 
								ofstream &oOutFileStream);
    void DealWithTaskDeviceRelation(QList<WorkflowTask> &m_oWorkflowTask,
									unsigned int nCurrentTaskIndex, 
									ofstream &oOutFileStream);

	
};
#endif 
