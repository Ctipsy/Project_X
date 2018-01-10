#include "cconfassess.h"
#include "ui_cconfassess.h"

extern System_Informa* sys_inf;
extern WorkflowCom *pCurrrentWorkflow;

CConfAssess::CConfAssess(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CConfAssess)
{
    ui->setupUi(this);
    move(0,50);

    setAutoFillBackground(true);
    QPalette pale1=palette();
    pale1.setColor(QPalette::Window,QColor(255,255,255));
    setPalette(pale1);

    input_rela_device();

    ui->stack_con->setCurrentIndex(0);
    ui->rela_device_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->rela_device_table->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空

    ui->rela_mess_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->rela_mess_table->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空

    ui->res_info_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->res_info_table->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空

    ui->res_device_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->res_device_table->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空





}
void CConfAssess::Updateinfor()
{
    input_rela_device();
    input_rela_mess();
    result_rela_info();
    result_rela_device();
}


/*           *****************************************                    */
/*           **************************初始信息********                    */
/*           *****************************************                    */

void CConfAssess::input_rela_device()
{
    if(pCurrrentWorkflow==NULL) return;
    ui->rela_device_table->setRowCount(0);
    QMap<uint,double> device_group=pCurrrentWorkflow->m_oDeviceStaticCon;
    for(auto i_temp=device_group.begin();i_temp!=device_group.end();i_temp++)
    {
        int currentRow=ui->rela_device_table->rowCount();
        ui->rela_device_table->insertRow(currentRow);
        ui->rela_device_table->setItem(currentRow,0,new QTableWidgetItem(QString::number(i_temp.key())));
        ui->rela_device_table->setItem(currentRow,1,new QTableWidgetItem(sys_inf->Get_SingleDevice(i_temp.key())._Get_DeviceName()));
        ui->rela_device_table->setItem(currentRow,2,new QTableWidgetItem(QString::number(i_temp.value())));
    }
}

void CConfAssess::input_rela_mess()
{
    if(pCurrrentWorkflow==NULL) return;
    ui->rela_mess_table->setRowCount(0);
    QMap<uint,double> mess_group=pCurrrentWorkflow->m_oInfoStaticCon;
    for(auto i_temp=mess_group.begin();i_temp!=mess_group.end();i_temp++)
    {
        int currentRow=ui->rela_mess_table->rowCount();
        ui->rela_mess_table->insertRow(currentRow);
        ui->rela_mess_table->setItem(currentRow,0,new QTableWidgetItem(QString::number(i_temp.key())));
        ui->rela_mess_table->setItem(currentRow,1,new QTableWidgetItem(sys_inf->Get_SingleMessage(i_temp.key())._Get_MessageName()));
        ui->rela_mess_table->setItem(currentRow,2,new QTableWidgetItem(QString::number(i_temp.value())));
    }
}


/*           *****************************************                    */
/*           **************************结果信息********                    */
/*           *****************************************                    */


void CConfAssess::result_rela_info()
{
    if(pCurrrentWorkflow==NULL) return;
    ui->res_info_table->setRowCount(0);
    QMap<uint,double> mess_group=pCurrrentWorkflow->m_oInfoStaticCon;
    for(auto i_temp=mess_group.begin();i_temp!=mess_group.end();i_temp++)
    {
        int current_Row=ui->res_info_table->rowCount();
        ui->res_info_table->insertRow(current_Row);
        Message_Informa current_mess=sys_inf->Get_SingleMessage(i_temp.key());
        ui->res_info_table->setItem(current_Row,0,new QTableWidgetItem(current_mess._Get_MessageName()));
        ui->res_info_table->setItem(current_Row,1,new QTableWidgetItem(QString::number(current_mess._Get_AimMessageSecurity())));
        ui->res_info_table->setItem(current_Row,2,new QTableWidgetItem(QString::number(i_temp.value())));
        ui->res_info_table->setItem(current_Row,3,new QTableWidgetItem(QString::number
                                                                      ((i_temp.value()>=current_mess._Get_AimMessageSecurity()))));
    }
}

void CConfAssess::result_rela_device()
{
    if(pCurrrentWorkflow==NULL) return;
    ui->res_device_table->setRowCount(0);
    QMap<uint,double> device_group=pCurrrentWorkflow->m_oDeviceStaticCon;
    QMap<uint,double> mess_group=pCurrrentWorkflow->m_oInfoStaticCon;
    for(auto i_temp=device_group.begin();i_temp!=device_group.end();i_temp++)
    {
        Device_Informa current_device=sys_inf->Get_SingleDevice(i_temp.key());
        int current_Row=ui->res_device_table->rowCount();
        ui->res_device_table->insertRow(current_Row);
        ui->res_device_table->setItem(current_Row,0,new QTableWidgetItem(current_device._Get_DeviceName()));
        QString mid;
        float max_con=0;
        for(auto j_temp=mess_group.begin();j_temp!=mess_group.end();j_temp++)
        {
            Message_Informa current_mess=sys_inf->Get_SingleMessage(j_temp.key());
            if(current_mess._Get_MessageSendDevice()==i_temp.key()||
                    current_mess._Get_MessageReceiveDevice()==i_temp.key())
            {
                if(current_mess._Get_AimMessageSecurity()>max_con)  max_con=current_mess._Get_AimMessageSecurity();
                mid+=" ";
                mid+=QString::number(j_temp.key());
            }
        }
        ui->res_device_table->setItem(current_Row,1,new QTableWidgetItem(mid));
        ui->res_device_table->setItem(current_Row,2,new QTableWidgetItem(QString::number(max_con)));
        ui->res_device_table->setItem(current_Row,3,new QTableWidgetItem(QString::number(i_temp.value())));
        ui->res_device_table->setItem(current_Row,4,new QTableWidgetItem(QString::number
                                                                         ((i_temp.value()>=max_con))));
    }
}

CConfAssess::~CConfAssess()
{
    delete ui;
}

void CConfAssess::on_ini_date_clicked()
{
    ui->stack_con->setCurrentIndex(0);
}


void CConfAssess::on_assess_result_clicked()
{
    ui->stack_con->setCurrentIndex(1);
}

