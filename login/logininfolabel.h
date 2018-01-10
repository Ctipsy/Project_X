#ifndef LOGININFOLABEL_H
#define LOGININFOLABEL_H

#include <QtGui>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDialog>

class LoginInfoLabel : public QDialog
{
    Q_OBJECT

public:
    LoginInfoLabel();

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // LOGININFOLABEL_H
