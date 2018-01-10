#ifndef LOGINPANEL_H
#define LOGINPANEL_H

#include "login/regpanel.h"
#include "login/findpassword.h"
#include "login/logininfolabel.h"
#include "main/mainwindow.h"
#include "main/admin_config.h"
#include <QtGui>
#include <QDialog>
#include <QPropertyAnimation>
#include <QTime>
#include <QTimer>
#include <QtSql>
#include <QDebug>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QDir>
#include <QSqlError>
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDialog>
#include <QLineEdit>
#include"licence/Licence.h"
namespace Ui{
    class LoginPanel;
}
class LoginPanel : public QDialog
{
    Q_OBJECT

public:
    LoginPanel();



protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    bool eventFilter(QObject *target, QEvent *event);

private slots:
    void registerPanel();//注册
    bool startLogin();//登录
    void findPasswordPanel();//找回密码
    void closeAll();//关闭程序
    void timeOut();//设置一个定时器

private:
    Ui::LoginPanel *ui;
    bool createConnection();//连接数据库
    void createTable();//创建表
    bool judgeLogin();//判断是否登录成
    //QSqlDatabase oDatabase;
    QPoint dragPosition;

    QTimer *timer;
    //MainWindow *w;
    RegPanel *regPanel;//注册界面
    FindPassword *findPassword;//找回密码界面
    LoginInfoLabel *loginInfoLabel;//登录成功显示信息
    QPushButton *closeLogButton;
    QPushButton *minLogButton;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QPushButton *findPasswordButton;


    QLabel *errorLgLabel;
    QLabel *errorLgLabel2;

    QLineEdit *userID;
    QLineEdit *password;
    Licence *lic;
};

#endif // LOGINPANEL_H
