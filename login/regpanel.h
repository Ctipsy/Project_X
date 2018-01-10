#ifndef REGPANEL_H
#define REGPANEL_H

#include <QtGui>
#include <QtSql>
#include <QDateTimeEdit>
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <cstdlib>
#include <time.h>
#include <ctime>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDialog>
#include <QLineEdit>


class RegPanel:public QDialog
{
    Q_OBJECT

public:
    RegPanel(QDialog *dialog = 0);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    bool eventFilter(QObject *target, QEvent *event);

private slots:
    void startReg();//开始注册
    void closeReg();//关闭
    void timeOut();//设置一个定时器
    void regInfoToDb();//把注册信息录入数据库

private:
    bool judgeReg();//判断是否注册成功

    QPoint dragPosition;

    QTimer *timer;
    QTimer *timer2;
    unsigned long int id;//分配账号

    QLabel *errorLabel;
    QLabel *errorLabel0;
    QLabel *errorLabel2;
    QLabel *errorLabel3;
    QLabel *errorLabel4;
    QLabel *infoLabel;
    QLabel *regLabel;

    QLabel *regSuccessLabel;
    QLabel *infoLabel2;
    QLabel *infoLabel3;

    QLabel *userID;

    QPushButton *closeRegButton;
    QPushButton *minRegButton;
    QPushButton *startRegButton;
    QPushButton *okRegButton;

    QLineEdit *userNUM;
    QLineEdit *userRegName;
    QLineEdit *passwordReg;
    QLineEdit *passwordRegOk;
    QLineEdit *safeRegAnswer;

    QDateTimeEdit *regday;
    QButtonGroup *buttonGroup;
    QRadioButton *boyButton;
    QRadioButton *girlButton;

    QComboBox *department;
    QComboBox *safeRegQuestion;

    QStringList departmentList;
    QStringList safeRegQuestionList;

};

#endif // REGPANEL_H
