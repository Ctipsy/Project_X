#ifndef HISTORY_H
#define HISTORY_H

#include <QDialog>
#include <QTableWidget>

namespace Ui {
class History;
}

class History : public QDialog
{
    Q_OBJECT

public:
    explicit History(QWidget *parent = 0);
    ~History();
    void initial();
    void read_project_dat();
    QString xml_path;
private slots:
    void editTxt(int x, int y);

    void on_open_clicked();


    void on_deleter_clicked();

private:
    Ui::History *ui;
    QTableWidget *Project_Info;
    bool DelDir(const QString &path);
    void read_table();

    

};

#endif // HISTORY_H
