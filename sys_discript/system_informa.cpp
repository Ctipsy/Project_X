#include "sys_discript/system_informa.h"

/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */


System_Informa::System_Informa(QDomDocument &doc)
{
    Set_System_Informa(doc);
}
System_Informa::System_Informa(QFile &cXmlFile)
{
    QDomDocument doc;
    QTextStream floStream(&cXmlFile);
    QTextCodec *codec=QTextCodec::codecForName("UTF-8");
    //QTextCodec::setCodecForTr(codec);
    //QTextCodec::setCodecForLocale(codec);
    //QTextCodec::setCodecForCStrings(codec);
    floStream.setCodec(codec);
    QString xmlDataStr = floStream.readAll();
    //if(!doc.setContent(xmlDataStr)){file.close();return 0;}
    doc.setContent(xmlDataStr);
    cXmlFile.close();
    Set_System_Informa(doc);
}
void System_Informa::Set_System_Informa(QDomDocument &doc)
{
    if(!m_hashDevice.isEmpty()) m_hashDevice.clear();
    if(!m_hashMessage.isEmpty()) m_hashMessage.clear();
    if(!m_hashTask.isEmpty()) m_hashTask.clear();
    QDomNodeList device_list=doc.elementsByTagName("device");
    for(int i_Temp=0;i_Temp<device_list.count();i_Temp++)
    {
       QDomNode current_device=device_list.at(i_Temp);
       Device_Informa device_temp(current_device);
       m_hashDevice.insert(device_temp._Get_Deviceid(),device_temp);
    }

    QDomNodeList channel_list=doc.elementsByTagName("channel");
    for(int i_Temp=0;i_Temp<channel_list.count();i_Temp++)
    {
        QDomNode current_channel=channel_list.at(i_Temp);
        Channel_Informa channel_temp(current_channel);
        m_hashChannel.insert(channel_temp._Get_ChannelId(),channel_temp);
    }


    QDomNodeList message_list=doc.elementsByTagName("informa");
    for(int i_Temp=0;i_Temp<message_list.count();i_Temp++)
    {
       QDomNode current_message=message_list.at(i_Temp);
       Message_Informa message_temp(current_message);
       m_hashMessage.insert(message_temp._Get_Messageid(),message_temp);
    }

    QDomNodeList business_list=doc.elementsByTagName("business");
    for(int i_Temp=0;i_Temp<business_list.count();i_Temp++)
    {
       QDomNode current_business=business_list.at(i_Temp);
       Business_Informa business_temp(current_business);
       m_hashBusiness.insert(business_temp._Get_BusinessId(),business_temp);
    }

    QDomNodeList task_list=doc.elementsByTagName("task");
    for(int i_Temp=0;i_Temp<task_list.count();i_Temp++)
    {
       QDomNode current_task=task_list.at(i_Temp);
       Task_Informa task_temp(current_task);
       m_hashTask.insert(task_temp._Get_TaskId(),task_temp);
    }
}

QString VectorToString(const QList<uint> &device_link)
{
    QString output;
    int i=0;
    for(;i<device_link.size()-1;i++)
    {
        output+=QString::number(device_link[i]);
        output+=" ";
    }
    if(device_link.size()!=0)
        output+=QString::number(device_link[i]);
    else output+=QString::number(0);
    return output;
}

void System_Informa::SaveToXml(QString save_path)
{
    QDomDocument dom;
    QDomProcessingInstruction instruction;
    instruction=dom.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"");
    dom.appendChild(instruction);
    QDomElement system=dom.createElement("system");
    QDomElement devices=dom.createElement("devices");
    for(auto i_temp=m_hashDevice.begin();i_temp!=m_hashDevice.end();i_temp++)
    {
        QDomElement device=dom.createElement("device");
        QDomElement device_id=dom.createElement("device_id");
        QDomText device_id_text=dom.createTextNode(QString::number(i_temp.value()._Get_Deviceid()));
        device_id.appendChild(device_id_text);
        device.appendChild(device_id);

        QDomElement device_name=dom.createElement("device_name");
        QDomText device_name_text=dom.createTextNode(i_temp.value()._Get_DeviceName());
        device_name.appendChild(device_name_text);
        device.appendChild(device_name);

        QDomElement device_type=dom.createElement("device_type");
        QDomText device_type_text=dom.createTextNode(QString::number(i_temp.value()._Get_DeviceType()));
        device_type.appendChild(device_type_text);
        device.appendChild(device_type);

        QDomElement device_mtbf=dom.createElement("device_mtbf");
        QDomText device_mtbf_text=dom.createTextNode(QString::number(i_temp.value()._Get_DeviceMTBF()));
        device_mtbf.appendChild(device_mtbf_text);
        device.appendChild(device_mtbf);

        QDomElement device_mttr=dom.createElement("device_mttr");
        QDomText device_mttr_text=dom.createTextNode(QString::number(i_temp.value()._Get_DeviceMTTR()));
        device_mttr.appendChild(device_mttr_text);
        device.appendChild(device_mttr);

        QDomElement device_link=dom.createElement("device_link");
        QDomText device_link_text=dom.createTextNode(VectorToString(i_temp.value()._Get_Devicelink()));
        device_link.appendChild(device_link_text);
        device.appendChild(device_link);

        devices.appendChild(device);
    }
    system.appendChild(devices);

    QDomElement channels=dom.createElement("channels");
    for(auto i_temp=m_hashChannel.begin();i_temp!=m_hashChannel.end();i_temp++)
    {
        QDomElement channel=dom.createElement("channel");
        QDomElement channel_id=dom.createElement("channel_id");
        QDomText channel_id_text=dom.createTextNode(QString::number(i_temp.value()._Get_ChannelId()));
        channel_id.appendChild(channel_id_text);
        channel.appendChild(channel_id);

        QDomElement channel_min=dom.createElement("channel_min");
        QDomText channel_min_text=dom.createTextNode(QString::number(i_temp.value()._Get_Channel().first));
        channel_min.appendChild(channel_min_text);
        channel.appendChild(channel_min);

        QDomElement channel_max=dom.createElement("channel_max");
        QDomText channel_max_text=dom.createTextNode(QString::number(i_temp.value()._Get_Channel().second));
        channel_max.appendChild(channel_max_text);
        channel.appendChild(channel_max);

        QDomElement channel_capacity=dom.createElement("channel_capacity");
        QDomText channel_capacity_text=dom.createTextNode(QString::number(i_temp.value()._Get_ChannelCapacity()));
        channel_capacity.appendChild(channel_capacity_text);
        channel.appendChild(channel_capacity);

        QDomElement channel_protocal=dom.createElement("channel_protocal");
        QDomText channel_protocal_text=dom.createTextNode(QString::number(i_temp.value()._Get_ChannelProtocal()));
        channel_protocal.appendChild(channel_protocal_text);
        channel.appendChild(channel_protocal);

        QDomElement channel_media=dom.createElement("channel_media");
        QDomText channel_media_text=dom.createTextNode(QString::number(i_temp.value()._Get_ChannelMedia()));
        channel_media.appendChild(channel_media_text);
        channel.appendChild(channel_media);

        channels.appendChild(channel);
    }
    system.appendChild(channels);

    QDomElement informas=dom.createElement("informas");
    for(auto i_temp=m_hashMessage.begin();i_temp!=m_hashMessage.end();i_temp++)
    {
        QDomElement informa=dom.createElement("informa");
        QDomElement informa_id=dom.createElement("informa_id");
        QDomText informa_id_text=dom.createTextNode(QString::number(i_temp.value()._Get_Messageid()));
        informa_id.appendChild(informa_id_text);
        informa.appendChild(informa_id);

        QDomElement informa_name=dom.createElement("informa_name");
        QDomText informa_name_text=dom.createTextNode(i_temp.value()._Get_MessageName());
        informa_name.appendChild(informa_name_text);
        informa.appendChild(informa_name);

        QDomElement informa_type=dom.createElement("informa_type");
        QDomText informa_type_text=dom.createTextNode(QString::number(i_temp.value()._Get_MessageType()));
        informa_type.appendChild(informa_type_text);
        informa.appendChild(informa_type);

        QDomElement send_device=dom.createElement("send_device");
        QDomText send_device_text=dom.createTextNode(QString::number(i_temp.value()._Get_MessageSendDevice()));
        send_device.appendChild(send_device_text);
        informa.appendChild(send_device);

        QDomElement receive_device=dom.createElement("receive_device");
        QDomText receive_device_text=dom.createTextNode(QString::number(i_temp.value()._Get_MessageReceiveDevice()));
        receive_device.appendChild(receive_device_text);
        informa.appendChild(receive_device);

        QDomElement informa_rate=dom.createElement("informa_rate");
        QDomText informa_rate_text=dom.createTextNode(QString::number(i_temp.value()._Get_MessageRate()));
        informa_rate.appendChild(informa_rate_text);
        informa.appendChild(informa_rate);

        QDomElement informa_size=dom.createElement("informa_size");
        QDomText informa_size_text=dom.createTextNode(QString::number(i_temp.value()._Get_MessageSize()));
        informa_size.appendChild(informa_size_text);
        informa.appendChild(informa_size);

        informas.appendChild(informa);
    }
    system.appendChild(informas);

    QDomElement businesses=dom.createElement("businesses");
    for(auto i_temp=m_hashBusiness.begin();i_temp!=m_hashBusiness.end();i_temp++)
    {
        QDomElement business=dom.createElement("business");
        QDomElement business_id=dom.createElement("business_id");
        QDomText business_id_text=dom.createTextNode(QString::number(i_temp.value()._Get_BusinessId()));
        business_id.appendChild(business_id_text);
        business.appendChild(business_id);

        QDomElement business_name=dom.createElement("business_name");
        QDomText business_name_text=dom.createTextNode(i_temp.value()._Get_BusinessName());
        business_name.appendChild(business_name_text);
        business.appendChild(business_name);

        QDomElement business_Ave_Require=dom.createElement("business_Ave_Require");
        QDomText business_Ave_Require_text=dom.createTextNode(QString::number(i_temp.value()._Get_BusinessReqSecurity()));
        business_Ave_Require.appendChild(business_Ave_Require_text);
        business.appendChild(business_Ave_Require);

        QDomElement relation_task=dom.createElement("relation_task");
        QDomText relation_task_text=dom.createTextNode(VectorToString(i_temp.value()._Get_RelationTask()));
        relation_task.appendChild(relation_task_text);
        business.appendChild(relation_task);

        businesses.appendChild(business);
    }
    system.appendChild(businesses);

    QDomElement tasks=dom.createElement("tasks");
    for(auto i_temp=m_hashTask.begin();i_temp!=m_hashTask.end();i_temp++)
    {
        QDomElement task=dom.createElement("task");
        QDomElement task_id=dom.createElement("task_id");
        QDomText task_id_text=dom.createTextNode(QString::number(i_temp.value()._Get_TaskId()));
        task_id.appendChild(task_id_text);
        task.appendChild(task_id);

        QDomElement task_name=dom.createElement("task_name");
        QDomText task_name_text=dom.createTextNode(i_temp.value()._Get_TaskName());
        task_name.appendChild(task_name_text);
        task.appendChild(task_name);

        QDomElement pre_task=dom.createElement("pre_task");
        QDomText pre_task_text=dom.createTextNode(VectorToString(i_temp.value()._Get_PreTask()));
        pre_task.appendChild(pre_task_text);
        task.appendChild(pre_task);

        QDomElement task_to_task=dom.createElement("task_to_task");
        QDomText task_to_task_text=dom.createTextNode(i_temp.value()._Get_TaskRelation());
        task_to_task.appendChild(task_to_task_text);
        task.appendChild(task_to_task);

        QDomElement relation_device=dom.createElement("relation_device");
        QDomText relation_device_text=dom.createTextNode(VectorToString(i_temp.value()._Get_TaskDevice()));
        relation_device.appendChild(relation_device_text);
        task.appendChild(relation_device);

        QDomElement device_to_task=dom.createElement("device_to_task");
        QDomText device_to_task_text=dom.createTextNode(i_temp.value()._Get_DeviceToTask());
        device_to_task.appendChild(device_to_task_text);
        task.appendChild(device_to_task);

        QDomElement pre_info=dom.createElement("pre_info");
        QDomText pre_info_text=dom.createTextNode(VectorToString(i_temp.value()._Get_PreInfo()));
        pre_info.appendChild(pre_info_text);
        task.appendChild(pre_info);
        tasks.appendChild(task);
    }
    system.appendChild(tasks);

    dom.appendChild(system);
    QFile *file=new QFile(save_path);
    if(file->open(QIODevice::WriteOnly))
    {
        QTextStream stream(file);
        dom.save(stream,4);
        file->close();
    }
}

/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */
QList<uint> System_Informa::Get_DeviceId_Vector()const
{
    QList<uint> result;
    for(QMap<uint,Device_Informa>::const_iterator tem=m_hashDevice.begin();tem!=m_hashDevice.end();tem++)
        result.push_back((*tem)._Get_Deviceid());
    return result;
}
Device_Informa& System_Informa::Get_SingleDevice(uint tem_device_id)
{
    if(m_hashDevice.find(tem_device_id)!=m_hashDevice.end())
    return m_hashDevice[tem_device_id];
    else
    {
        //qDebug()<<"can't find this device"<<endl;
        return m_hashDevice[0];
    }
}


/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */
QList<uint> System_Informa::Get_MessageId_Vector()const
{
    QList<uint> result;
    for(QMap<uint,Message_Informa>::const_iterator tem=m_hashMessage.begin();tem!=m_hashMessage.end();tem++)
        result.push_back((*tem)._Get_Messageid());
    return result;
}
Message_Informa& System_Informa::Get_SingleMessage(uint tem_message_id)
{
    if(m_hashMessage.find(tem_message_id)!=m_hashMessage.end())
        return m_hashMessage[tem_message_id];
    else
    {
        //qDebug()<<"can't find this message"<<endl;
        return m_hashMessage[0];
    }
}


/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */
QList<uint> System_Informa::Get_ChannelId_Vector()const
{
    QList<uint> result;
    for(QMap<uint,Channel_Informa>::const_iterator tem=m_hashChannel.begin();tem!=m_hashChannel.end();tem++)
        result.push_back((*tem)._Get_ChannelId());
    return result;
}

float System_Informa::Get_ChannelCapacity(uint tem_senddevice_id,uint tem_receivedevice_id)
{
    QPair<uint,uint> pair_temp=qMakePair(min(tem_senddevice_id,tem_receivedevice_id),max(tem_senddevice_id,tem_receivedevice_id));
    QList<uint> channel_vector=Get_ChannelId_Vector();
    for(int i=0;i!=channel_vector.size();i++)
    {
        Channel_Informa temp_channel=Get_SingleChannel(channel_vector[i]);
        if(temp_channel._Get_Channel()==pair_temp)
            return temp_channel._Get_ChannelCapacity();
    }
    return 0;
}

Channel_Informa& System_Informa::Get_SingleChannel(uint tem_senddevice_id,uint tem_receivedevice_id)
{
    QPair<uint,uint> pair_temp=qMakePair(min(tem_senddevice_id,tem_receivedevice_id),max(tem_senddevice_id,tem_receivedevice_id));
    QList<uint> channel_vector=Get_ChannelId_Vector();
    for(int i=0;i!=channel_vector.size();i++)
    {
        Channel_Informa &temp_channel=Get_SingleChannel(channel_vector[i]);
        if(temp_channel._Get_Channel()==pair_temp)
            return temp_channel;
    }
    return m_hashChannel[0];
}
Channel_Informa& System_Informa::Get_SingleChannel(uint tem_channel_id)
{
    if(m_hashChannel.find(tem_channel_id)==m_hashChannel.end()) return m_hashChannel[0];
    return m_hashChannel[tem_channel_id];
}

/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */
QList<uint> System_Informa::Get_BusinessId_Vector()const
{
    QList<uint> result;
    for(QMap<uint,Business_Informa>::const_iterator tem=m_hashBusiness.begin();tem!=m_hashBusiness.end();tem++)
        result.push_back((*tem)._Get_BusinessId());
    return result;
}
Business_Informa System_Informa::Get_SingleBusiness(uint tem_business_id)
{
    if(m_hashBusiness.find(tem_business_id)!=m_hashBusiness.end())
        return m_hashBusiness[tem_business_id];
    else
    {
        //qDebug()<<"can't find this business";
        return m_hashBusiness[0];
    }
}


/*           ********************************                    */
/*           ********************************                    */
/*           ********************************                    */
QList<uint> System_Informa::Get_TaskId_Vector()const
{
    QList<uint> result;
    for(QMap<uint,Task_Informa>::const_iterator tem=m_hashTask.begin();tem!=m_hashTask.end();tem++)
        result.push_back((*tem)._Get_TaskId());
    return result;
}
Task_Informa& System_Informa::Get_SingleTask(uint tem_task_id)
{
    if(m_hashTask.find(tem_task_id)!=m_hashTask.end())
        return m_hashTask[tem_task_id];
    else
    {
        //qDebug()<<"can't find this task"<<endl;
        return m_hashTask[0];
    }
}
