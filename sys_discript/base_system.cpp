#include "sys_discript/base_informa.h"
extern QMap<QString,uint> subsystem;
extern QMap<QString,uint> domain;

QList<unsigned int> StringToVector(QString input_string)
{
    QList<unsigned int> result;
    //QString temp_string(input_string);
    QTextStream ss(&input_string);
    uint aTEMP;
    while((ss>>aTEMP,aTEMP)!=0)
    {
        result.push_back(aTEMP);
    }
    return result;
}
uint min(uint a,uint b)
{
    if(a<=b) return a;
    else return b;
}
uint max(uint a,uint b)
{
    if(a<=b) return b;
    else return a;
}

/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */
Device_Informa::Device_Informa(){m_unDevice_Id=1;}
Device_Informa::Device_Informa(QDomNode &node)
{
    QDomElement e_Temp=node.toElement();
    QDomNode n_Temp=e_Temp.firstChild();
    m_unDevice_Id=n_Temp.toElement().text().toUInt();
    n_Temp=n_Temp.nextSibling();
    m_strDevice_Name=n_Temp.toElement().text();
    n_Temp=n_Temp.nextSibling();
    m_unDevice_Type=n_Temp.toElement().text().toUInt();
    n_Temp=n_Temp.nextSibling();
    QString current_subsystem=n_Temp.toElement().text();
    if(subsystem.find(current_subsystem)!=subsystem.end()) m_unSystem_Type=subsystem[current_subsystem];
    else
    {
        subsystem.insert(current_subsystem,subsystem.size()+1);
        m_unSystem_Type=subsystem[current_subsystem];
    }
    n_Temp=n_Temp.nextSibling();
    QString current_domain=n_Temp.toElement().text();
    if(domain.find(current_domain)!=domain.end()) m_unDomain_Type=domain[current_domain];
    else
    {
        domain.insert(current_domain,domain.size()+1);
        m_unDomain_Type=domain[current_domain];
    }
    n_Temp=n_Temp.nextSibling();
    m_fDevice_MTBF=n_Temp.toElement().text().toFloat();
    n_Temp=n_Temp.nextSibling();
    m_fDevice_MTTR=n_Temp.toElement().text().toFloat();
    n_Temp=n_Temp.nextSibling();
    m_unDevice_Link=StringToVector(n_Temp.toElement().text());
}
uint Device_Informa::_Get_Deviceid()const{return m_unDevice_Id;}
void Device_Informa::_Set_Deviceid(uint un_DeviceId){m_unDevice_Id=un_DeviceId;}
const QList<uint>& Device_Informa::_Get_Devicelink()const{return m_unDevice_Link;}
void Device_Informa::_Set_Devicelink(const QList<uint> &un_Devicelink){m_unDevice_Link=un_Devicelink;}
uint Device_Informa::_Get_DeviceType()const{return m_unDevice_Type;}
void Device_Informa::_Set_DeviceType(uint un_DeviceType){m_unDevice_Type=un_DeviceType;}
uint Device_Informa::_Get_SystemType()const{return m_unSystem_Type;}
void Device_Informa::_Set_SystemType(uint un_SystemType){m_unSystem_Type=un_SystemType;}
uint Device_Informa::_Get_DomainType()const{return m_unDomain_Type;}
void Device_Informa::_Set_DomainType(uint un_DomainType){m_unDomain_Type=un_DomainType;}
const QString& Device_Informa::_Get_DeviceName()const{return m_strDevice_Name;}
void Device_Informa::_Set_DeviceName(QString str_DeviceName){m_strDevice_Name=str_DeviceName;}
float Device_Informa::_Get_DeviceMTTR()const{return m_fDevice_MTTR;}
void Device_Informa::_Set_DeviceMTTR(float f_DeviceMTTR){m_fDevice_MTTR=f_DeviceMTTR;}
float Device_Informa::_Get_DeviceMTBF()const{return m_fDevice_MTBF;}
void Device_Informa::_Set_DeviceMTBF(float f_DeviceMTBF){m_fDevice_MTBF=f_DeviceMTBF;}
float Device_Informa::_Get_DeviceSecurity()const{return m_fDevice_Security;}
void Device_Informa::_Set_DeviceSecurity(float f_Device_Security){m_fDevice_Security=f_Device_Security;}

/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */

Message_Informa::Message_Informa(){m_unMessage_Id=1;}
Message_Informa::Message_Informa(QDomNode &node)
{
    QDomElement e_Temp=node.toElement();
    QDomNode n_Temp=e_Temp.firstChild();
    m_unMessage_Id=n_Temp.toElement().text().toUInt();
    n_Temp=n_Temp.nextSibling();
    m_strMessage_Name=n_Temp.toElement().text();
    n_Temp=n_Temp.nextSibling();
    m_unMessage_Type=n_Temp.toElement().text().toUInt();
    n_Temp=n_Temp.nextSibling();
    m_unSend_Device=n_Temp.toElement().text().toUInt();
    n_Temp=n_Temp.nextSibling();
    m_unReceive_Device=n_Temp.toElement().text().toUInt();
    n_Temp=n_Temp.nextSibling();
    m_fMessage_Rate=n_Temp.toElement().text().toFloat();
    n_Temp=n_Temp.nextSibling();
    m_unMessage_Size=n_Temp.toElement().text().toUInt();
    m_unAimMessage_Reliability=1;
    m_unAimMessage_Security=1;
}
uint Message_Informa::_Get_Messageid()const{return m_unMessage_Id;}
void Message_Informa::_Set_Messageid(uint un_Messageid){m_unMessage_Id=un_Messageid;}
const QString& Message_Informa::_Get_MessageName()const{return m_strMessage_Name;}
void Message_Informa::_Set_MessageName(QString str_MessageName){m_strMessage_Name=str_MessageName;}
uint Message_Informa::_Get_MessageType()const{return m_unMessage_Type;}
void Message_Informa::_Set_MessageType(uint un_MessageType){m_unMessage_Type=un_MessageType;}
uint Message_Informa::_Get_MessageSendDevice()const{return m_unSend_Device;}
void Message_Informa::_Set_MessageSendDevice(uint un_MessageSendDevice){m_unSend_Device=un_MessageSendDevice;}
uint Message_Informa::_Get_MessageReceiveDevice()const{return m_unReceive_Device;}
void Message_Informa::_Set_MessageReceiveDevice(uint un_MessageReceiveDevice){m_unReceive_Device=un_MessageReceiveDevice;}
float Message_Informa::_Get_MessageRate()const{return m_fMessage_Rate;}
void Message_Informa::_Set_MessageRate(float f_MessageRate){m_fMessage_Rate=f_MessageRate;}
uint Message_Informa::_Get_MessageSize()const{return m_unMessage_Size;}
void Message_Informa::_Set_MessageSize(uint un_MessageSize){m_unMessage_Size=un_MessageSize;}

float Message_Informa::_Get_AimMessageSecurity()const{return m_unAimMessage_Security;}
void Message_Informa::_Set_AimMessageSecurity(float un_AimMessage_Security){m_unAimMessage_Security=un_AimMessage_Security;}
float Message_Informa::_Get_MessageSecurity()const{return m_fMessage_Security;}
void Message_Informa::_Set_MessageSecurity(float f_MessageSecurity){m_fMessage_Security=f_MessageSecurity;}

float Message_Informa::_Get_AimMessageReliability()const{return m_unAimMessage_Reliability;}
void Message_Informa::_Set_AimMessageReliability(float un_AimMessage_Reliability){m_unAimMessage_Reliability=un_AimMessage_Reliability;}
float Message_Informa::_Get_MessageReliability()const{return m_fMessage_Reliability;}
void Message_Informa::_Set_MessageReliability(float f_Message_Reliability){m_fMessage_Reliability=f_Message_Reliability;}

/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */
Business_Informa::Business_Informa(){m_unBusiness_Id=1;}
Business_Informa::Business_Informa(QDomNode &node)
{
    QDomElement e_Temp=node.toElement();
    QDomNode n_Temp=e_Temp.firstChild();
    m_unBusiness_Id=n_Temp.toElement().text().toUInt();
    n_Temp=n_Temp.nextSibling();
    m_strBusiness_name=n_Temp.toElement().text();
    n_Temp=n_Temp.nextSibling();
    m_fBusiness_Req_Security=n_Temp.toElement().text().toFloat();
    n_Temp=n_Temp.nextSibling();
    m_unRelation_Task=StringToVector(n_Temp.toElement().text());
}
uint Business_Informa::_Get_BusinessId()const{return m_unBusiness_Id;}
void Business_Informa::_Set_BusinessId(uint un_BusinessId){m_unBusiness_Id=un_BusinessId;}
const QString& Business_Informa::_Get_BusinessName()const{return m_strBusiness_name;}
void Business_Informa::_Set_BusinessName(QString str_BusinessName){m_strBusiness_name=str_BusinessName;}
float Business_Informa::_Get_BusinessReqSecurity()const{return m_fBusiness_Req_Security;}
void Business_Informa::_Set_BusinessReqSecurity(float f_BussinessReqSecurity){m_fBusiness_Req_Security=f_BussinessReqSecurity;}
const QList<uint>& Business_Informa::_Get_RelationTask() const{return m_unRelation_Task;}
void Business_Informa::_Set_RelationTask(const QList<uint> &un_RelationTask){m_unRelation_Task=un_RelationTask;}
float Business_Informa::_Get_BusinessSecurity()const{return m_fBusiness_Security;}
void Business_Informa::_Set_BusinessSecurity(float f_Business_Security){m_fBusiness_Security=f_Business_Security;}


/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */
Task_Informa::Task_Informa(){m_unTask_Id=1;}
Task_Informa::Task_Informa(QDomNode &node)
{
    QDomElement e_Temp=node.toElement();
    QDomNode n_Temp=e_Temp.firstChild();
    m_unTask_Id=n_Temp.toElement().text().toUInt();
    n_Temp=n_Temp.nextSibling();
    m_strTask_Name=n_Temp.toElement().text();
    n_Temp=n_Temp.nextSibling();
    m_unPre_Task=StringToVector(n_Temp.toElement().text());
    n_Temp=n_Temp.nextSibling();
    m_strTask_Relation=n_Temp.toElement().text();
    n_Temp=n_Temp.nextSibling();
    m_unTask_Device=StringToVector(n_Temp.toElement().text());
    n_Temp=n_Temp.nextSibling();
    m_strDevice_To_Task=n_Temp.toElement().text();
    n_Temp=n_Temp.nextSibling();
    m_unPre_Info=StringToVector(n_Temp.toElement().text());
}
uint Task_Informa::_Get_TaskId()const{return m_unTask_Id;}
void Task_Informa::_Set_TaskId(uint un_TaskId){m_unTask_Id=un_TaskId;}
const QString& Task_Informa::_Get_TaskName()const{return m_strTask_Name;}
void Task_Informa::_Set_TaskName(QString str_TaskName){m_strTask_Name=str_TaskName;}
const QList<uint>& Task_Informa::_Get_PreTask()const{return m_unPre_Task;}
void Task_Informa::_Set_PreTask(const QList<uint> &un_PreTask){m_unPre_Task=un_PreTask;}
const QString& Task_Informa::_Get_TaskRelation()const{return m_strTask_Relation;}
void Task_Informa::_Set_TaskRelation(QString str_TaskRelation){m_strTask_Relation=str_TaskRelation;}
const QList<uint>& Task_Informa::_Get_TaskDevice()const{return m_unTask_Device;}
void Task_Informa::_Set_TaskDevice(const QList<uint> &un_TaskDevice){m_unTask_Device=un_TaskDevice;}
const QString& Task_Informa::_Get_DeviceToTask()const{return m_strDevice_To_Task;}
void Task_Informa::_Set_DeviceToTask(QString str_DeviceToTask){m_strDevice_To_Task=str_DeviceToTask;}
const QList<uint>& Task_Informa::_Get_PreInfo()const{return m_unPre_Info;}
void Task_Informa::_Set_PreInfo(const QList<uint> &un_PreInfo){m_unPre_Info=un_PreInfo;}
float Task_Informa::_Get_TaskSecurity()const{return m_fTask_Security;}
void Task_Informa::_Set_TaskSecurity(float f_Task_Security){m_fTask_Security=f_Task_Security;}



/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */
Channel_Informa::Channel_Informa(){m_unChannel_Id=1;}
Channel_Informa::Channel_Informa(QDomNode &node)
{
    QDomElement e_Temp=node.toElement();
    QDomNode n_Temp=e_Temp.firstChild();
    m_unChannel_Id=n_Temp.toElement().text().toUInt();
    n_Temp=n_Temp.nextSibling();
    uint i_temp=n_Temp.toElement().text().toUInt();
    n_Temp=n_Temp.nextSibling();
    uint j_temp=n_Temp.toElement().text().toUInt();
    m_Channel=qMakePair(min(i_temp,j_temp),max(i_temp,j_temp));
    n_Temp=n_Temp.nextSibling();
    m_fChannel_Capacity=n_Temp.toElement().text().toFloat();
    n_Temp=n_Temp.nextSibling();
    m_unChannel_Protocal=n_Temp.toElement().text().toUInt();
    n_Temp=n_Temp.nextSibling();
    m_unChannel_Media=n_Temp.toElement().text().toUInt();
}
uint Channel_Informa::_Get_ChannelId()const{return m_unChannel_Id;}
void Channel_Informa::_Set_ChannelId(uint un_ChannelId){m_unChannel_Id=un_ChannelId;}
const QPair<uint, uint> &Channel_Informa::_Get_Channel()const{return m_Channel;}
void Channel_Informa::_Set_Channel(QPair<uint,uint> Channel){m_Channel=Channel;}
float Channel_Informa::_Get_ChannelCapacity()const{return m_fChannel_Capacity;}
void Channel_Informa::_Set_ChannelCapacity(float f_CurrentChannelCapacity){m_fChannel_Capacity=f_CurrentChannelCapacity;}
uint Channel_Informa::_Get_ChannelMedia()const{return m_unChannel_Media;}
void Channel_Informa::_Set_ChannelMedia(uint un_CurrentChannelMedia){m_unChannel_Media=un_CurrentChannelMedia;}
uint Channel_Informa::_Get_ChannelProtocal()const{return m_unChannel_Protocal;}
void Channel_Informa::_Set_ChannelProtocal(uint un_CurrentChannelProtocal){m_unChannel_Protocal=un_CurrentChannelProtocal;}
