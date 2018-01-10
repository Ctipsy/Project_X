#ifndef WORKFLOW_ASSESS_H
#define WORKFLOW_ASSESS_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>
#include <QImage>
#include <QThread>

#include "workflow/wflow.h"
#include "result_display/result_display.h"

namespace Ui {
class CWorkflowAssess;
}

class WorkThread: public QThread
{
    Q_OBJECT
    protected:
};

class CWorkflowAssess : public QWidget
{
    Q_OBJECT

    public:
    explicit CWorkflowAssess(QWidget *parent = 0);
    ~CWorkflowAssess();

    //void ShowModel();

    void MySleep(unsigned long ulMs);

    //void show();
    void UpdateTable();

    //设备评估结果更新显示
    void ShowDeviceResult();
    void UpdateDeviceResultTable();

    //任务评估结果更新显示
    void UpdateTaskResultTable();
    void ShowTaskResult();

    //显示多个工作流评估结果的对比图
    void ShowWorkflowResult();

    void total_reset();

    private:

    void BoolAlgebraAssessWidgetShow(bool flag);

    void BayesAssessWidgetShow(bool flag);

    void BoolAlgebraAssessInit(); //如果选择布尔代数评估推理算法，显示对应的所需要的数据

    void BayesAssessInit();  //如果选择贝叶斯评估推理算法，就对应显示评估所需要的数据

    private slots:


    void on_algorithmCcomboBox_currentTextChanged(const QString &arg1);

    //void on_algorithmCcomboBox_currentIndexChanged(int index);


    void on_assessResultBtn_clicked();

    void on_statisticalAnalysisBtn_clicked();

    void on_multiWorkflowCompareBtn_clicked();

    void on_AvailResultBtn_clicked();

    void on_returnBtn_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::CWorkflowAssess *ui;

    QImage *image;//=new QImage("./image/chuzhongtao.jpg");
    //QLabel *label;//=new QLabel(this);
    WorkThread *m_poThread;
    //WorkflowCom *&m_poWorkflowCom;
    QTableWidget *m_pBayesTable;
    CResultShow *m_pResultShow;
public:
    QMap<QString, pair<double, double>> m_lstWorkflowAvail;//2018.01.03改为public属性，因为生成PDF文档时不能调用
};



#endif // WORKFLOW_ASSESS_H
