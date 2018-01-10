#ifndef MESSAGEASSESS_H
#define MESSAGEASSESS_H

#include <QWidget>
#include <QObject>
#include <QDebug>
#include <QList>
#include <QComboBox>
#include <QMessageBox>
#include <QTableWidget>
#include <QtAlgorithms>
#include <QtCharts>
#include "sys_discript/system_informa.h"
#include "result_display/result_display.h"
#include "graphviz/internet_info_graphviz.h"
#include "questionnaire_analysis/questionnaire_analysis.h"
#include"svg_show/svgwindow.h"
#include "index/index.h"
#include <QEvent>
#include <QSvgRenderer>
/*#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif*/

void data_analysis(uint unAimDeviceId,float& max_Device_Security,float& max_Device_reli,float& message_total,float& min_Device_Security);

namespace Ui {
class messageassess;
}

class messageassess : public QWidget
{
    Q_OBJECT
public:
    explicit messageassess(QWidget *parent = 0);
    void Set_ModelConstruct();
    void Display_Message1();
    void Display_Message2();
    void Display_Message3();
    void set_protocal();
    void Initial_Read_Receive_Device();
    void Set_ChannelTable(int i, int j);
    void Set_MessageTable(int i, int j);
    void Set_CriticalDevice_S();
    void Get_ProtocalGraph();
    ~messageassess();
    void init_total_message();
    void View_SingleMess(const QString& current_mess);
    void View_TotalMess();
    void low_score_index(const QString &arg1);
    bool eventFilter(QObject *watched, QEvent *event);

    QStringList *Get_ProtocalText();
signals:
    int x(int a,int b);
    void x();
    void y(int a1,int b1,int c1);

private slots:

    void Set_Modified_Informa(QTableWidgetItem *item);

    void Set_Modified_Channel(QTableWidgetItem *item);

    void Set_Modified_InformaType(const QString &current_message);

    void on_Csend_device_currentIndexChanged(int index);

    void on_Creceive_device_currentIndexChanged(int index);

    void Set_Modified_ChannelType(const QString& current_channel);

    void on_protocol_selection_currentIndexChanged(int index);

    void Set_Result_Initial();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_radioBreli_clicked();

    void on_radioBconf_clicked();

    void on_Pinforma_inter_clicked();

    void on_Pchannel_capcity_clicked();

    void on_edit_question_clicked();

    void on_tableWidget_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void on_total_informa_clicked();

    void on_Csubsystem_currentIndexChanged(int index);

    void on_Cdomain_currentIndexChanged(int index);

    void on_message_selection_currentIndexChanged(const QString &arg1);

    void on_total_score_view_clicked();

    void on_single_score_view_clicked();

    void on_radioButton_clicked(bool checked);

    void on_tableWidget_2_itemClicked(QTableWidgetItem *item);

private:
    void change_window();
    void set_message_index();
     QSvgRenderer* m_renderer;
private:
    Ui::messageassess *ui;
    QStringList *m_Protocal_Text;
    int current_page=0;
    SvgWindow *svgWindow;
};

#endif // MESSAGEASSESS_H
