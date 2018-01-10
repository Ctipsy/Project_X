#ifndef INTERNET_ACCESS_H
#define INTERNET_ACCESS_H

#include <QWidget>
#include <vector>
#include <QPushButton>
#include <QButtonGroup>
#include <string>
#include <QTableWidgetItem>
#include <QSvgRenderer>

#include"index/index.h"
#include"graphviz/internet_info_graphviz.h"
#include"result_display/result_display.h"

#define invalidInput -1 //用于检测输入的可靠性矩阵数值是否正确

namespace Ui {
class InternetAccess;
}

class InternetAccess : public QWidget
{
    Q_OBJECT

public:
    explicit InternetAccess(QWidget *parent = 0);


    void Set_Table(QList<uint> nKIndex);
    QVector<int> Nodes_FromStringToInt(QString str);
    void ShowPicture(QImage *imageTuopu);
    double SubsystemReliability_Table_Ser();//计算串联子系统的可靠性，数值来源于设备的可靠性
    float SubsystemReliability_Table_Par();//计算并联子系统的可靠性，数值来源于设备的可靠性
    void MainSystemReliability_Table_SerAndPar();//计算主系统的可靠性，数值来源于计算出来的子系统的可靠性
    void writeIndexResultToDB(QString strTbableName,QString indexName,float indexResult);
    void change_window();
    bool showQuestionnaireRes(int row,int col,QString result);
    //bool eventFilter(QObject *watched, QEvent *event);
    void low_score_index();//获取低分数的原子指标
    void getPDFInternet();//获取生成PDF文件的内容
    ~InternetAccess();

private slots:

    void showTuopuComboBox();
    void on_pushButton_faultPropagationL_clicked();
    void on_pushButton_faultPropagationC_clicked();
    void on_tableWidget_Par_itemChanged();
    void on_tableWidget_Ser_itemChanged();
    //void on_pushButton_finalResult_clicked();
    void tableWidget_cellChanged(QTableWidgetItem *item);
    void on_pushButton_CheckRedundancy_clicked();
    void on_comboBox_tuopu_currentIndexChanged(int index);
    void on_pushButton_checkGuzhangRes_clicked();
    void on_pushButton_tuopu_clicked();
    void on_pushButton_rongyu_clicked();
    void on_pushButton_guzhang_clicked();
    void on_pushButton_writeQuestionnaire_clicked();
    void on_pushButton_kekuozhan_clicked();
    void on_pushButton_quyu_clicked();
    void on_pushButton_zaibei_clicked();
    void on_pushButton_bianjie_clicked();
    void on_tabWidget_tabBarClicked(int index);
    void on_pushButton_checkQAnswer_clicked();
    void on_tableWidget_selres_itemClicked(QTableWidgetItem *item);
    void on_pushButton_readQuestionnaire_clicked();
    bool eventFilter(QObject *watched, QEvent *event);//将svg图片显示在label上增加的代码


private:
    Ui::InternetAccess *ui;
    float fReliability_System;
    QImage *imageTuopu;
    CInfoInternetVizObj *m_pInternetGraphviz;
    CResultShow *m_pFinalResultShow;
    int count=0;
    Index *index_question;        //7.18添加，用于存储网络架构的所有指标名称和nodeid
    QMap<unsigned int, QString> index_result; //7.18添加，用于存储网络架构的所有指标名称和nodeid
    QList<Index*> index_select; //7.18添加，用于存储网络架构的所有指标名称和nodeid
    int current_page=0;
    CResultShow m_radarPicture;//雷达图
    QSvgRenderer* m_renderer;

};

#endif // INTERNET_ACCESS_H
