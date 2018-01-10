#ifndef FINDPASSWORD_H
#define FINDPASSWORD_H

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
#include <QFont>
#include <QMessageBox>
class FindPassword:public QDialog
{
    Q_OBJECT

public:
    FindPassword(QDialog *dialog = 0);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    bool eventFilter(QObject *target, QEvent *event);
private slots:
    void closeFd();
    void findPwd();//找回密码
private:
    QPoint dragPosition;
    QTimer *timer;
    QPushButton *closeFdButton;
    QPushButton *minFdButton;
    QPushButton *okFdButton;

    QLineEdit *safeFdAnswer;
    QLineEdit *userFdID;



    QLabel *infoFdLabel;
    QLabel *passwordLabel;

    QComboBox *safeFdQuestion;
    QStringList safeFdQuestionList;
};

#endif // FINDPASSWORD_H
