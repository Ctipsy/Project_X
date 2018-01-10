#include<fstream>
#include<set>
#include<QProcess>
#include<Windows.h>
#include<WinNT.h>
#include<WinBase.h>
#include"graphviz/chineseletterhelper.h"



#include"graphviz/internet_info_graphviz.h"

using namespace std;

extern System_Informa* sys_inf;
//extern QList<Index*> index_select;

extern QMap<uint,uint> fault_device;
extern QSet<uint> infect_device;
extern QSet<uint> infect_info;


const QString CInfoInternetVizObj::GetDeviceNameByNum(const uint num)
{
    return sys_inf->Get_SingleDevice(num)._Get_DeviceName();
}

void CInfoInternetVizObj::InfoGraphviz(QVector<InfoNode> &vecONodeInfo, const QString &strDotFileName)
{
//	char strName[255];
//	sprintf_s(strName, sizeof(strName), "%s.dot", strDotFileName.c_str());
//	ofstream oOutFileStream(strName);
    //QString strFileName = QString::fromStdString(strDotFileName);
    QString strName = QString("%1.dot").arg(strDotFileName);
    ofstream oOutFileStream(strName.toStdString().c_str());

    oOutFileStream << "graph qing{" << endl;
    oOutFileStream << "\trankdir = LR;" << endl;
        oOutFileStream << "\graph [bgcolor = transparent,autosize=false, size=\"8.0, 10\"]"<< endl;


    set<unsigned int> unHandledDev;

    SetNodeProperty(oOutFileStream, "info_device_node");
    for (unsigned int i = 0; i < vecONodeInfo.size(); ++i)
    {
        unHandledDev.insert(vecONodeInfo[i].devNumber);
        for (unsigned int j = 0; j < vecONodeInfo[i].adjacentNode.size(); ++j)
        {
            if (unHandledDev.find(vecONodeInfo[i].adjacentNode[j].devNumber) == unHandledDev.end())
                oOutFileStream << "\t" << vecONodeInfo[i].devNumber << "--" << vecONodeInfo[i].adjacentNode[j].devNumber <<
                    " [label = \"" << vecONodeInfo[i].adjacentNode[j].info << "\"];" << endl;
        }
    }
    oOutFileStream << "}"	<< endl;


}

void CInfoInternetVizObj::InternetGraphviz(const QVector<InfoNode> &vecONodeInfo, const QString &strDotFileName, bool flag)
{
    char projectAddr[255];
    GetPrivateProfileStringA("projectInfo", "projectAddr", "", projectAddr, sizeof(projectAddr), INI_PATH.toStdString().c_str());
    QString strName = QString("%1.dot").arg(strDotFileName);
    strName = QString(QLatin1String(projectAddr)) + "/dot/" + strName;
    ofstream oOutFileStream(strName.toStdString().c_str());

    oOutFileStream <<"graph qing{"<< endl;
    oOutFileStream <<"\trankdir = LR;" << endl;
    oOutFileStream <<"\graph [bgcolor = transparent,autosize=false, size=\"8.0, 10\"]"<< endl;


    set<unsigned int> unHandledDev;

    if (flag)
        SetNodeProperty(oOutFileStream, "internet_main_system_node");
    else
        SetNodeProperty(oOutFileStream, "internet_device_node");

    for (unsigned int i = 0; i < vecONodeInfo.size(); ++i)
    {
        unHandledDev.insert(vecONodeInfo[i].devNumber);
        oOutFileStream << "\t" << vecONodeInfo[i].devNumber << "[label = \"" <<
            vecONodeInfo[i].devNumber << "\\n" << vecONodeInfo[i].devName << "\"];" << endl;
        for (unsigned int j = 0; j < vecONodeInfo[i].adjacentNode.size(); ++j)
        {
            if (unHandledDev.find(vecONodeInfo[i].adjacentNode[j].devNumber) == unHandledDev.end())
                //oOutFileStream << "\t" << vecONodeInfo[i].devNumber << "--" << vecONodeInfo[i].adjacentNode[j].devNumber <<
                //" [label = \"" << vecONodeInfo[i].adjacentNode[j].info << "\"];" << endl;
            {
                oOutFileStream << "\t" << vecONodeInfo[i].adjacentNode[j].devNumber << "[label = \"" <<
                    vecONodeInfo[i].adjacentNode[j].devNumber << "\\n" << vecONodeInfo[i].adjacentNode[j].devName << "\"];" << endl;
                oOutFileStream << "\t" << vecONodeInfo[i].devNumber << "--" << vecONodeInfo[i].adjacentNode[j].devNumber << ";" << endl;
            }
        }
    }
    oOutFileStream << "}"	<< endl;
}

void CInfoInternetVizObj::InternetGraphviz(const QVector<InfoNode> &vecONodeInfo)
{
    char projectAddr[255];
    GetPrivateProfileStringA("projectInfo", "projectAddr", "", projectAddr, sizeof(projectAddr), INI_PATH.toStdString().c_str());

    char dotName[255];
    GetPrivateProfileStringA("dotFile", "internetDotName", "", dotName, sizeof(dotName), INI_PATH.toStdString().c_str());

    QString internetDot = QString(QLatin1String(projectAddr)) + "/dot/" + QString(QLatin1String(dotName));
    InternetGraphviz(vecONodeInfo, internetDot.toLatin1(), false);

    QProcess p;
    p.start("cmd.exe", QStringList() << "/c" << "D:\\Program Files (x86)\\X_Ship\\internetImageJpg.bat");
    if (p.waitForStarted())
    {
       p.waitForFinished();
       qDebug() << p.readAllStandardOutput();
    }
    else
       qDebug() << "Failed to start";
}

void CInfoInternetVizObj::InternetGraphviz(const QMap<QString,QVector<InfoNode> > &subSystemInfo, bool flag)
{
    for (QMap<QString,QVector<InfoNode> >::const_iterator iter = subSystemInfo.begin(); iter != subSystemInfo.end(); ++iter)
    {
        InternetGraphviz(iter.value(), iter.key(), flag);
        QStringList arg;
        arg << iter.key();
        char projectAddr[255];

        qDebug()<<"step 1"<<iter.key();

        GetPrivateProfileStringA("projectInfo", "projectAddr", "", projectAddr, sizeof(projectAddr), INI_PATH.toStdString().c_str());

        QString strName = "D:\\Program Files (x86)\\X_Ship\\internetImageJpg.bat";

        QProcess p;
        p.start(strName, arg);
        qDebug()<<"图片生成测试";
        if (p.waitForStarted())
        {
           p.waitForFinished();
           qDebug()<<"图片生成测试er";
           //qDebug() << p.readAllStandardOutput();
        }
        else
           qDebug() << "Failed to start";
    }
}

void CInfoInternetVizObj::InfoGraphvizAnother(const QHash<pair<uint, uint>, pair<QString, QString> > &linkInfo,
                         const QString &strDotFileName)
{
//	char strName[255];
//	sprintf_s(strName, sizeof(strName), "%s.dot", strDotFileName.c_str());
//	ofstream oOutFileStream(strName);

    //QString strFileName = QString::fromStdString(strDotFileName);
    QString strName = QString("%1.dot").arg(strDotFileName);

    ofstream oOutFileStream(strName.toStdString().c_str());

    oOutFileStream << "graph qing{" << endl;
    oOutFileStream << "\trankdir = LR;" << endl;
    oOutFileStream << "\graph [bgcolor = transparent,autosize=false, size=\"8.0, 10\"]"<< endl;


    SetNodeProperty(oOutFileStream, "info_node");

    for (QHash<pair<uint, uint>, pair<QString, QString> >::const_iterator iter = linkInfo.begin(); iter != linkInfo.end(); ++iter)
    {
        QString strTemp;
        if (iter.value().first.length() == 0 || iter.value().second.length() == 0)
        {
            strTemp = (iter.value().first.length() == 0) ? iter.value().second : iter.value().first;
        }
        strTemp = iter.value().first + "/" + iter.value().second;

        oOutFileStream << "\t" << GetDeviceNameByNum(iter.key().first).toStdString() << "--" << GetDeviceNameByNum(iter.key().second).toStdString() <<
                          "[label = \"" << strTemp.toStdString() << "\"];" << endl;
    }
    oOutFileStream << "}"	<< endl;
}

void CInfoInternetVizObj::InfoGraphvizAnother(const QHash<pair<uint, uint>, pair<QString, QString> > &linkInfo)
{
    char projectAddr[255];
    GetPrivateProfileStringA("projectInfo", "projectAddr", "", projectAddr, sizeof(projectAddr), INI_PATH.toStdString().c_str());

    char dotName[255];
    GetPrivateProfileStringA("dotFile", "infoDotName", "", dotName, sizeof(dotName), INI_PATH.toStdString().c_str());

    QString infoDot = QString(QLatin1String(projectAddr)) + "/dot/" + QString(QLatin1String(dotName));
    InfoGraphvizAnother(linkInfo, infoDot.toLatin1());


    //QProcess::execute("C:/719/2017_06_10_Total/infoMapJpg.bat");

    //GetPrivateProfileStringA("projectInfo", "projectAddr", "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());



    QProcess p;
    p.start("cmd.exe", QStringList() << "/c" << "D:\\Program Files (x86)\\X_Ship\\infoMapJpg.bat");
    if (p.waitForStarted())
    {
       p.waitForFinished();
       qDebug() << p.readAllStandardOutput();
    }
    else
       qDebug() << "Failed to start";
}

//struct link_info
//{
//    pair<uint,uint> link;
//    pair<QString,QString> link_message;
//    pair<QString,uint> style;
//};
void CInfoInternetVizObj::InfoGraphvizWithProperty(const QList<link_info> &infoNode, bool flag)
{
    char projectAddr[255];
    GetPrivateProfileStringA("projectInfo", "projectAddr", "", projectAddr, sizeof(projectAddr), INI_PATH.toStdString().c_str());

    char dotName[255];
    GetPrivateProfileStringA("dotFile", "infoDotName", "", dotName, sizeof(dotName), INI_PATH.toStdString().c_str());

    QString infoDot = QString(QLatin1String(projectAddr)) + "/dot/" + QString(QLatin1String(dotName));

    QString strName = QString("%1.dot").arg(infoDot);
    ofstream oOutFileStream(strName.toStdString().c_str());

    QString direction;
    if (flag)
    {
        direction = "->";
        oOutFileStream << "digraph qing{" << endl;
		
    }
    else
    {
        direction = "--";
        oOutFileStream << "graph qing{" << endl;
    }
    oOutFileStream << "\trankdir = LR;" << endl;
    oOutFileStream << "\graph [bgcolor = transparent,autosize=false, size=\"8.0, 10\"]"<< endl;


    SetNodeProperty(oOutFileStream, "info_node");
    for (int i = 0; i < infoNode.size(); ++i)
    {
        QString strTemp;
        if (infoNode[i].link_message.first.length() == 0 || infoNode[i].link_message.second.length() == 0)
        {
            strTemp = (infoNode[i].link_message.first.length() == 0) ? infoNode[i].link_message.second : infoNode[i].link_message.first;
        }
        strTemp = infoNode[i].link_message.first + "/" + infoNode[i].link_message.second;


        oOutFileStream << "\t" << GetDeviceNameByNum(infoNode[i].link.first).toStdString() << direction.toStdString() << GetDeviceNameByNum(infoNode[i].link.second).toStdString() <<
                          "[label = \"" << strTemp.toStdString() << "\", color = " << infoNode[i].style.first.toStdString() << ", penwidth = " << infoNode[i].style.second << ", fontname = \"FangSong\"];" << endl;
    }
    oOutFileStream << "}"	<< endl;

    QProcess p;
    p.start("cmd.exe", QStringList() << "/c" <<"D:\\Program Files (x86)\\X_Ship\\infoMapJpg.bat");
    if (p.waitForStarted())
    {
       p.waitForFinished();
       qDebug() << p.readAllStandardOutput();
    }
    else
       qDebug() << "Failed to start";
}


void CInfoInternetVizObj::IndexGraphvizWithProperty(const QList<link_info> &infoNode,Index* current_index)
{
    char projectAddr[255];
    GetPrivateProfileStringA("projectInfo", "projectAddr", "", projectAddr, sizeof(projectAddr), INI_PATH.toStdString().c_str());

    char dotName[255];
    GetPrivateProfileStringA("dotFile", "indexDotName", "index", dotName, sizeof(dotName), INI_PATH.toStdString().c_str());
                                                      //dot文件名称
    QString infoDot = QString(QLatin1String(projectAddr)) + "/dot/" + QString(QLatin1String(dotName));

    QString strName = QString("%1.dot").arg(infoDot);
    ofstream oOutFileStream(strName.toStdString().c_str());

    QString direction;

    direction = "->";
    oOutFileStream << "digraph index{" << endl;
    //oOutFileStream << "\trankdir = LR;" << endl;
    oOutFileStream << "\graph [bgcolor = transparent,autosize=false, size=\"8.0, 10\"]"<< endl;



    SetNodeProperty(oOutFileStream, "index_node");
    for (int i = 0; i < infoNode.size(); ++i)
    {
        QString strTemp;
        if (infoNode[i].link_message.first.length() == 0 || infoNode[i].link_message.second.length() == 0)
        {
            //qDebug()<<"linkmessage"<<infoNode[i].link_message.first<<"第二个"<<infoNode[i].link_message.second;
            strTemp = (infoNode[i].link_message.first.length() == 0) ? infoNode[i].link_message.second : infoNode[i].link_message.first;
        }
        else
            strTemp = infoNode[i].link_message.first + "/" + infoNode[i].link_message.second;

        if(current_index->m_nodeMark[infoNode[i].link.first]==""||current_index->m_nodeMark[infoNode[i].link.second]=="")
            continue;
        else
            oOutFileStream << "\t" << current_index->m_nodeMark[infoNode[i].link.second].toStdString() << direction.toStdString() << current_index->m_nodeMark[infoNode[i].link.first].toStdString() <<
                             "[label = \"" << strTemp.toStdString() << "\", color = " << infoNode[i].style.first.toStdString() << ", penwidth = " << infoNode[i].style.second << ", fontname = \"FangSong\"];" << endl;
    }
    oOutFileStream << "}"	<< endl;


    QProcess p;
    p.start("cmd.exe", QStringList() << "/c" << "D:\\Program Files (x86)\\X_Ship\\indexMapJpg.bat");
    if (p.waitForStarted())
    {
       p.waitForFinished();
       qDebug() << p.readAllStandardOutput();
    }
    else
       qDebug() << "Failed to start";


}


void CInfoInternetVizObj::SituationWithProperty(QMap<uint, double> &Device,QMap<uint, double> &Info)
{
    char projectAddr[255];
    GetPrivateProfileStringA("projectInfo", "projectAddr", "", projectAddr, sizeof(projectAddr), INI_PATH.toStdString().c_str());

    char dotName[255];
    GetPrivateProfileStringA("dotFile", "SituationDotName", "", dotName, sizeof(dotName), INI_PATH.toStdString().c_str());
                                                      //dot文件名称
    QString infoDot = QString(QLatin1String(projectAddr)) + "/dot/" + QString(QLatin1String(dotName));
    QString strName = QString("%1.dot").arg(infoDot);
    ofstream oOutFileStream(strName.toStdString().c_str());

    oOutFileStream << "digraph qing{" << endl;
    oOutFileStream << "rankdir = LR" << endl;
    oOutFileStream << "\graph [bgcolor = transparent,autosize=false, size=\"8.0, 10\"]"<< endl;

    SetNodeProperty(oOutFileStream, "Situ_node");
    for (auto i = Device.begin(); i != Device.end(); i++)
    {
        QString cur_shape;
        QString cur_color;
        QString cur_style;
        if(fault_device[i.key()]==1)
        {
            cur_shape="circle\"";
            cur_color="\"red\"";
            cur_style="\"filled";
        }
        else
        {
            cur_shape="none\"";
            cur_color="\"\"";
            cur_style="\"";
        }

       //根据设备名称加载图片，有病毒的，名称-1.png,  没有病毒的名称png
        QString pic_addr,device_name;
        QString device_name_old=sys_inf->Get_SingleDevice(i.key())._Get_DeviceName();
        device_name = device_name_old.left(device_name_old.length() - 1);
        QString pinyin = ChineseLetterHelper::GetPinyins(device_name);
        pinyin=pinyin.replace(QRegExp("\\'"), ""); //移除'字符


        if(infect_device.find(i.key())!=infect_device.end()) pic_addr="D:/Program Files (x86)/X_Ship/image/pic/"+pinyin+"-1.png";
        else pic_addr="D:/Program Files (x86)/X_Ship/image/pic/"+pinyin+".png";
        oOutFileStream << device_name_old.toStdString() << "[shape = \""<<cur_shape.toStdString()<<",color="<<cur_color.toStdString()<<",style="<<cur_style.toStdString()<<"\",image = \""<<pic_addr.toStdString()<<"\",labelloc = b ,height=1.2];"<<endl;

    }

    for(auto i=Info.begin();i!=Info.end();i++)
    {
        QString cur_color;
        if(infect_info.find(i.key())!=infect_info.end()) cur_color="red";
        else cur_color="green";
        oOutFileStream << sys_inf->Get_SingleDevice(sys_inf->Get_SingleMessage(i.key())._Get_MessageSendDevice())._Get_DeviceName().toStdString() << "->"<<
                          sys_inf->Get_SingleDevice(sys_inf->Get_SingleMessage(i.key())._Get_MessageReceiveDevice())._Get_DeviceName().toStdString() << "[ color = "<<cur_color.toStdString()<<" ] "<<endl;
    }
    oOutFileStream << "}"<< endl;
    strName = QString(QLatin1String(projectAddr))+"./SituMapJpg.bat";

    QProcess p;
    p.start("cmd.exe", QStringList() << "/c" << "D:\\Program Files (x86)\\X_Ship\\SituMapJpg.bat");
    if (p.waitForStarted())
    {
       p.waitForFinished();
       qDebug() << p.readAllStandardOutput();
    }
    else
       qDebug() << "Failed to start";

    //qDebug()<<"执行bat完毕";

}
