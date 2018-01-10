#ifndef SYSTEM_INFORMA_H
#define SYSTEM_INFORMA_H

#include <QtXml>
#include <QList>
#include <QTextStream>
#include <QFile>
#include "sys_discript/base_informa.h"

QString VectorToString(const QList<uint> &device_link);

class System_Informa
{
public:
    System_Informa(QDomDocument &doc);   //以DOM树为参数调用构造函数
    System_Informa(QFile &cXmlFile);       //以打开的xml文件为参数调用构造函数
    void Set_System_Informa(QDomDocument &doc);    //构造函数的内在实现

    void SaveToXml(QString save_path);

    QList<uint> Get_DeviceId_Vector()const;       //返回对象系统中所有设备的设备ID集合
    Device_Informa& Get_SingleDevice(uint tem_device_id);

    QList<uint> Get_MessageId_Vector()const;        //保存对象系统中输入的所有报文集合
    Message_Informa& Get_SingleMessage(uint tem_message_id);

    QList<uint> Get_ChannelId_Vector()const;
    float Get_ChannelCapacity(uint tem_senddevice_id,
                                    uint tem_receivedevice_id);    //返回两个关联设备间的信道容量
    Channel_Informa& Get_SingleChannel(uint tem_senddevice_id,
                                       uint tem_receivedevice_id);
    Channel_Informa& Get_SingleChannel(uint tem_channel_id);

    QList<uint> Get_BusinessId_Vector()const;   //获取对象系统中所有业务的集合
    Business_Informa Get_SingleBusiness(uint tem_business_id);

    QList<uint> Get_TaskId_Vector()const;      //获取对象系统中所有任务的集合
    Task_Informa& Get_SingleTask(uint tem_task_id);

private:
    QMap<uint,Device_Informa> m_hashDevice;     //设备ID与设备信息的映射关系
    QMap<uint,Message_Informa> m_hashMessage;   //报文ID与报文信息的映射关系
    QMap<uint,Task_Informa> m_hashTask;       //任务ID与任务信息的映射关系
    QMap<uint,Channel_Informa> m_hashChannel;      //信道信息
    QMap<uint,Business_Informa> m_hashBusiness;
};


/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */

#endif // SYSTEM_INFORMA_H
