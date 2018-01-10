#ifndef WULI_H
#define WULI_H
#include <QLabel>
#include <QWidget>
#include <QChartView>
#include "index/index.h"
#include "result_display/result_display.h"
#include "sys_discript/system_informa.h"
#include"svg_show/svgwindow.h"  //8.6添加

#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QTextStream> //文件流
#include <QMessageBox>

namespace Ui {
class wuli;
}
class wuli : public QWidget
{
    Q_OBJECT

public:
    explicit wuli(QWidget *parent = 0);
    ~wuli();

    //void ReliabilityCalculate();
    QString index_number_judge();
    void show_information();
private slots:
    void on_answer_questionnaire_clicked();
    void on_equip_index_select_currentIndexChanged(int index);
    void on_Type_result_show_currentIndexChanged(int index);//评估界面的设备大类选择下拉框
    void on_device_result_show_currentIndexChanged(const QString &arg1);
    void on_tabWidget_tabBarClicked(int index);  //重新定位当前指标体系所对应的表
    void on_tabWidget_2_tabBarClicked(int index);    

    void on_radioButton_clicked(bool checked);

    void on_tableWidget_itemClicked(QTableWidgetItem *item);
    void low_score_index(const QString &arg1);


private:
    Ui::wuli *ui;                   //定义的主窗口
    unsigned int m_unDeviceNum;
    //TreeViewDialog m_treeDialog;    //定义的指标树显示窗口
    //TreeViewDialog m_treeDialog;
    //QMap(unsigned int, QMap(float fMtbf, float fMttr)) m_mapDeviceInfo;
    CResultShow m_radarPicture;
    CResultShow m_barPicture;
    QLabel *name[8];
    QLabel *score[8];
    QMap<QString,double> device_total_score;
    SvgWindow *svgWindow;  //8.6添加
    QMap<QString,QMap<uint,double>> all_calculateResult; //ID（xml导入的，有设备ID 任务ID  信息ID） nodeid（一个设备有很多特性） result;
    QString xml_path;

};

#endif // WULI_H
