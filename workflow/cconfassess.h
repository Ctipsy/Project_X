#ifndef CCONFASSESS_H
#define CCONFASSESS_H

#include <QWidget>
#include "sys_discript/system_informa.h"
#include "workflow/workflow.h"

//#include "workflow/workflow_model.h"
//#include "workflow/wflow.h"
//#include "result_display/result_display.h"

namespace Ui {
class CConfAssess;
}

class CConfAssess : public QWidget
{
    Q_OBJECT

public:
    explicit CConfAssess(QWidget *parent = 0);
    ~CConfAssess();
    void input_rela_device();
    void input_rela_mess();
    void Updateinfor();
    void result_rela_info();
    void result_rela_device();
    //void total_reset();

private slots:
    //void on_pushButton_2_clicked();

    void on_ini_date_clicked();
    void on_assess_result_clicked();    
private:
    Ui::CConfAssess *ui;
};

#endif // CCONFASSESS_H
