#ifndef PROJECT_GUIDE_H
#define PROJECT_GUIDE_H

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QTableWidget>
#include <QVector>



namespace Ui {
class project_guide;
}

class project_guide : public QDialog
{
    Q_OBJECT

public:
   // explicit project_guide(bool item[], QWidget *parent = 0);
    project_guide(const bool *item,int length, QWidget *parent = 0);

    ~project_guide();
    void get_select_info(bool item[]);

    //void SaveToXml(QString save_path);
    void read_table();
    void creat_xml();
    void workCount();



private slots:
    void on_before_clicked();

    void on_next_clicked();

    void on_OK_clicked();

    //void on_pushButton_clicked();

    void on_addRow_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::project_guide *ui;
    int windowsID;
    void window_judge();
    QTableWidget *Xml_Info;
    void initial();
    bool select_Item[5];  //新建工程时，选择的评估组件 状态标记
	void items_judge();
    int  items;
	void xml_wuli();
    void xml_link();
	void xml_message();
	void xml_yewu();
    void xml_renwu();
    QVector<QVector<QString> > read_wuli;  //类似与二维数组，不过大小是可变动的
    QVector<QVector<QString> > read_link;
    QVector<QVector<QString> > read_message;
    QVector<QVector<QString> > read_renwu;
    QVector<QVector<QString> > read_yewu;
};

#endif // PROJECT_GUIDE_H
