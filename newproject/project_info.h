#ifndef PROJECT_INFO_H
#define PROJECT_INFO_H
#include <QTimer>
#include <QWidget>
#include <QSvgRenderer>
#include <QPainter>
#include "licence/Licence.h"


namespace Ui {
class project_info;
}

class project_info : public QWidget
{
    Q_OBJECT

public:
    explicit project_info(QWidget *parent = 0);
    ~project_info();
    bool eventFilter(QObject *watched, QEvent *event);
    void itemsCount();
    void itemsCount_2();


private:
    Ui::project_info *ui;
    QTimer *timer;
    QSvgRenderer* m_renderer;
    int items;
     Licence *lic;

private slots:
    void Show_Time();


};

#endif // PROJECT_INFO_H
