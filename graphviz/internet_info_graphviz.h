#ifndef _INTERNET_INFO_GRAPHVIZ
#define _INTERNET_INFO_GRAPHVIZ

#include<string>
#include<vector>
#include <QHash>
#include "index/index.h"

#include "graphviz/graphviz.h"
#include "sys_discript/system_informa.h"

using namespace std;

struct InfoRecord
{
	string devName;
	unsigned int devNumber;
    string info;
};

struct InfoNode
{
	string devName;
	unsigned int devNumber;
    QVector<InfoRecord> adjacentNode;
};

struct link_info
{
    pair<uint,uint> link;//连接的两个设备的编号
    pair<QString,QString> link_message;//总共信道容量，已用信道容量
    pair<QString,uint> style;//信道颜色设置
};

class CInfoInternetVizObj: public VizObject
{
public:
    void SituationWithProperty(QMap<uint, double> &Device,QMap<uint, double> &Info);
    void InfoGraphvizAnother(const QHash<pair<uint, uint>, pair<QString, QString> > &linkInfo);
    void InfoGraphvizWithProperty(const QList<link_info> &infoNode, bool flag);
    void IndexGraphvizWithProperty(const QList<link_info> &infoNode, Index *current_index);
    void InternetGraphviz(const QVector<InfoNode> &vecONodeInfo);
    void InternetGraphviz(const QMap<QString,QVector<InfoNode> > &subSystemInfo, bool flag);   //画主系统图时flag=true，子系统flag = false
private:
    const QString GetDeviceNameByNum(const uint num);
    void InfoGraphviz(QVector<InfoNode> &vecONodeInfo, const QString &strDotFileName);
    void InternetGraphviz(const QVector<InfoNode> &vecONodeInfo, const QString &strDotFileName, bool flag);
    void InfoGraphvizAnother(const QHash<pair<uint, uint>, pair<QString, QString> > &linkInfo, const QString &strDotFileName);
};

//提供给显示图片函数的接口函数,画每个子系统的图片，返回子系统的名称和子系统中每个节点的信息
//QMap<QString,QVector<InfoNode> > GetInfoForSubSystem_TuopuGraphviz()
//提供给显示图片函数的接口函数,画主系统的图片，返回主系统的名称和主系统中每个子系统的信息
//QMap<QString,QVector<InfoNode> > GetInfoForMainSystem_TuopuGraphviz()



#endif

