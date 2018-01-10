#ifndef WFLOW_H
#define WFLOW_H
#include <QSvgRenderer>
#include <QWidget>
#include"workflow/workflow.h"
#include "workflow/workflow_assess.h"
#include "workflow/cconfassess.h"
#include "sys_discript/system_informa.h"
#include"svg_show/svgwindow.h"  //8.6添加



class CWorkflowAssess;

namespace Ui {
class WFlow;
}

class WFlow : public QWidget
{
    Q_OBJECT

public:
    explicit WFlow(QWidget *parent = 0);
    ~WFlow();

    void InitWorkflowSet();     //初始化所有业务过程，即创建业务过程，并把业务过程相关任务和设备添加进来
    void Updateinforma();
    void UpdateModel();
    bool eventFilter(QObject *watched, QEvent *event);
private slots:
    //void on_WfModelBtn_clicked();
    void on_WfAssessBtn_clicked();
    void on_WfResultBtn_clicked();
    void on_listWidget_currentRowChanged(int currentRow);

    void on_radioButton_clicked(bool checked);

public:
    QList<WorkflowCom *> m_pWorkflowList;    //用以保存所有的业务过程指针
private:
    Ui::WFlow *ui;
    WorkflowCom *m_poWorkflowCom;          //当前正在操作的工作流指针
    CWorkflowAssess *m_poWorkflowAssessDlg;  //工作流可用性评估结果界面
    CConfAssess *m_poWorkflowConfAssessDlg;   //保密性评估结果界面
    QSvgRenderer* m_renderer;
    friend class CWorkflowAssess;
    friend class CAssessResult;
    SvgWindow *svgWindow;  //8.6添加


};

#endif // WFLOW_H
