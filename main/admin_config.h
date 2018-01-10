#ifndef ADMIN_CONFIG_H
#define ADMIN_CONFIG_H
#include "index/admin_index_conf.h"
#include <QWidget>
#include <QTableWidget>
#include <QMenu>

namespace Ui {
class admin_config;
}

class admin_config : public QWidget
{
    Q_OBJECT

public:
    explicit admin_config(QWidget *parent = 0);
    ~admin_config();


private slots:
    void on_questionnaire_config_clicked();

    void on_account_manage_clicked();

    void on_index_config_clicked();

    void on_knowledgebase_clicked();
    void customMenuRequested(QPoint pos);
    void rightMenu();
    void editTxt(int x, int y);

private:
    Ui::admin_config *ui;
    admin_index_conf *index_conf;
    QTableWidget *account;
    int Selcet_Knowledeg_Base();
    void  creat_index_txt();


};

#endif // ADMIN_CONFIG_H
