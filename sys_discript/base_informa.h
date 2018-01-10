#ifndef BASE_INFORMA_H
#define BASE_INFORMA_H

#include <QtXml>
//#include <QTextStream>
#include <QList>
//#include <sstream>

QList<unsigned int> StringToVector(QString input_string);
uint min(uint a,uint b);
uint max(uint a,uint b);
/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */
class Device_Informa
{
public:
    Device_Informa();
    Device_Informa(QDomNode &node);
    uint _Get_Deviceid()const;        //返回该设备的设备号
    void _Set_Deviceid(uint un_DeviceId);
    const QList<uint>& _Get_Devicelink()const;       //返回该设备的连接设备，以设备号的形式体现
   void _Set_Devicelink(const QList<uint> &un_Devicelink);
    uint _Get_DeviceType()const;       //返回该设备的设备类型
    void _Set_DeviceType(uint un_DeviceType);
    uint _Get_SystemType()const;       //返回该设备所在的子系统
    void _Set_SystemType(uint un_SystemType);
    uint _Get_DomainType()const;
    void _Set_DomainType(uint un_DomainType);
    const QString& _Get_DeviceName()const;       //返回设备名
    void _Set_DeviceName(QString str_DeviceName);
    float _Get_DeviceMTTR()const;      //返回设备的MTTR值
    void _Set_DeviceMTTR(float f_DeviceMTTR);
    float _Get_DeviceMTBF()const;      //返回设备的MTBF值
    void _Set_DeviceMTBF(float f_DeviceMTBF);
    float _Get_DeviceSecurity()const;  //返回设备的安全量化值
    void _Set_DeviceSecurity(float f_Device_Security);    //设置设备的安全量化值
private:
    uint m_unDevice_Id;
    QString m_strDevice_Name;
    uint m_unDevice_Type;
    uint m_unSystem_Type;
    uint m_unDomain_Type;
    float m_fDevice_MTBF;
    float m_fDevice_MTTR;
    QList<uint> m_unDevice_Link;
    float m_fDevice_Security=-1;
};


/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */
class Message_Informa
{
public:
    Message_Informa();
    Message_Informa(QDomNode &node);
    uint _Get_Messageid()const;       //返回报文的报文编号
    void _Set_Messageid(uint un_Messageid);
    const QString& _Get_MessageName()const;     //返回报文的报文名
    void _Set_MessageName(QString str_MessageName);
    uint _Get_MessageType()const;       //返回报文类型
    void _Set_MessageType(uint un_MessageType);
    uint _Get_MessageSendDevice()const;     //返回报文的发送设备的设备编号
    void _Set_MessageSendDevice(uint un_MessageSendDevice);
    uint _Get_MessageReceiveDevice()const;      //返回报文的接收设备编号
    void _Set_MessageReceiveDevice(uint un_MessageReceiveDevice);
    float _Get_MessageRate()const;    //返回报文的发送频率
    void _Set_MessageRate(float f_MessageRate);
    uint _Get_MessageSize()const;     //返回报文的报文大小
    void _Set_MessageSize(uint un_MessageSize);
    float _Get_AimMessageSecurity()const;
    void _Set_AimMessageSecurity(float un_AimMessage_Security);
    float _Get_MessageSecurity()const;
    void _Set_MessageSecurity(float f_MessageSecurity);
    float _Get_AimMessageReliability()const;
    void _Set_AimMessageReliability(float un_AimMessage_Reliability);
    float _Get_MessageReliability()const;
    void _Set_MessageReliability(float f_Message_Reliability);

private:
    uint m_unMessage_Id;
    QString m_strMessage_Name;
    uint m_unMessage_Type;
    uint m_unSend_Device;
    uint m_unReceive_Device;
    float m_fMessage_Rate;
    uint m_unMessage_Size;
    float m_fChannel_Capacity;
    float m_unAimMessage_Security;
    float m_fMessage_Security=-1;
    float m_unAimMessage_Reliability;
    float m_fMessage_Reliability=-1;
};

/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */
class Business_Informa
{
public:
    Business_Informa();
    Business_Informa(QDomNode &node);
    uint _Get_BusinessId()const;    //返回该业务的ID
    void _Set_BusinessId(uint un_BusinessId);
    const QString& _Get_BusinessName()const;   //返回该业务的业务名
    void _Set_BusinessName(QString str_BusinessName);
    float _Get_BusinessReqSecurity()const;     //返回该业务的安全需求
    void _Set_BusinessReqSecurity(float f_BussinessReqSecurity);
    const QList<uint>& _Get_RelationTask()const;  //返回业务相关的任务编号
    void _Set_RelationTask(const QList<uint>& un_RelationTask);
    float _Get_BusinessSecurity()const;      //返回业务安全的评估量化值（未评估时为-1）
    void _Set_BusinessSecurity(float f_Business_Security);   //设置业务安全的评估量化值
private:
    uint m_unBusiness_Id;
    QString m_strBusiness_name;
    float m_fBusiness_Req_Security;
    QList<uint> m_unRelation_Task;
    float m_fBusiness_Security=-1;
};
/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */
class Task_Informa
{
public:
    Task_Informa();
    Task_Informa(QDomNode &node);
    uint _Get_TaskId()const;        //返回任务的任务编号
    void _Set_TaskId(uint un_TaskId);
    const QString& _Get_TaskName()const;     //返回任务的任务名
    void _Set_TaskName(QString str_TaskName);
    const QList<uint>& _Get_PreTask()const;       //返回任务的前置任务
    void _Set_PreTask(const QList<uint>& un_PreTask);
    const QList<uint>& _Get_TaskDevice()const;     //返回任务的关联设备
    void _Set_TaskDevice(const QList<uint>& un_TaskDevice);
    const QString& _Get_TaskRelation()const;         //返回前置任务与该任务的逻辑关系
    void _Set_TaskRelation(QString str_TaskRelation);
    const QString& _Get_DeviceToTask()const;       //返回设备与任务的逻辑关系
    void _Set_DeviceToTask(QString str_DeviceToTask);
    const QList<uint>& _Get_PreInfo()const;     //返回该任务需要的信息交互，以编号形式表示
    void _Set_PreInfo(const QList<uint>& un_PreInfo);
    float _Get_TaskSecurity()const;          //返回该工作流的安全量化值
    void _Set_TaskSecurity(float f_Task_Security); //设置该工作流的安全量化值
private:
    uint m_unTask_Id;
    QString m_strTask_Name;
    QList<uint> m_unPre_Task;
    QString m_strTask_Relation;
    QList<uint> m_unTask_Device;
    QString m_strDevice_To_Task;
    QList<uint> m_unPre_Info;
    float m_fTask_Security=-1;
};




/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */
class Channel_Informa
{
public:
    Channel_Informa();
    Channel_Informa(QDomNode &node);
    const QPair<uint,uint>& _Get_Channel()const;
    void _Set_Channel(QPair<uint,uint> Channel);
    uint _Get_ChannelId()const;
    void _Set_ChannelId(uint un_ChannelId);
    float _Get_ChannelCapacity()const;
    void _Set_ChannelCapacity(float f_CurrentChannelCapacity);
    uint _Get_ChannelMedia()const;
    void _Set_ChannelMedia(uint un_CurrentChannelMedia);
    uint _Get_ChannelProtocal()const;
    void _Set_ChannelProtocal(uint un_CurrentChannelProtocal);
private:
    uint m_unChannel_Id;
    QPair<uint,uint> m_Channel;
    float m_fChannel_Capacity;
    uint m_unChannel_Protocal;
    uint m_unChannel_Media;
};



#endif // BASE_INFORMA_H
