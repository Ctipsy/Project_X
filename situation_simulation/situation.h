#ifndef SITUATION_H
#define SITUATION_H

#include <QWidget>
#include <QComboBox>
#include "workflow/wflow.h"
#include "sys_discript/system_informa.h"
#include <QSvgRenderer>
#include <result_display/result_display.h>
#include "svg_show/svgwindow.h"

namespace Ui {
class Situation;
}

class Situation : public QWidget
{
    Q_OBJECT

public:
    Situation(WFlow *m_poWFlowDlg,QWidget *parent = 0);
    ~Situation();
    void res_show();
    void addworkflow();
    void my_updata();

private:
    bool eventFilter(QObject *watched, QEvent *event);
    void updatepic();


private slots:
    void on_pb_additem1_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void Set_fault_Device(const QString& tem_fault_device);
    void Set_virusFunction_Device(const QString& tem_fault_device);
    void Set_virusConfidentiality_Device(const QString& tem_fault_device);
    void Set_gataway_Device(const QString& tem_fault_device);

    void on_choose_event1_clicked();

    void on_choose_event2_clicked();

    void on_choose_event3_clicked();

    void on_choose_event4_clicked();

    void on_pb_additem2_clicked();

    void on_pb_additem3_clicked();

    void on_pb_additem4_clicked();

    void on_radioButton_clicked(bool checked);

private:
    WFlow *out_poWFlowDlg;
    QSignalMapper *device_list_signallist1;
    QSignalMapper *device_list_signallist2;
    QSignalMapper *device_list_signallist3;
    QSignalMapper *device_list_signallist4;
    QList<uint> m_device_num;        //设备按顺序地编号值
    QSvgRenderer* m_renderer;
    Ui::Situation *ui;
    SvgWindow *svgWindow;

};

#endif // SITUATION_H
