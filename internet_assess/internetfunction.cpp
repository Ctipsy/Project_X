#include "ui_mainwindow.h"
#include "internetfunction.h"
#include "sys_discript/base_informa.h"
#include "sys_discript/system_informa.h"
#include "index/index.h"
#include <iostream>
#include <vector>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <QMultimap>
#include <QStringList>
#include <QSqlError>
#include <QSqlIndex>
#include <QSqlRecord>
#include <QtAlgorithms>
#include <QSqlQuery>
#include <QVariant>
#include <QString>
#include <QVector>
#include <QDebug>

extern System_Informa* sys_inf;
extern QList<Index*> index_select;
extern QMap<QString,uint> subsystem;
using namespace std;

QMap<QString,uint> subsystemForInternet;
/**************************************************************
//<>Descrip:体系结构可靠性建模中纯串联系统可靠性计算函数
//<>Call:无
//<>Input:输入为纯串联部分各功能单元可靠性一维数组
//<>Output:输出为串联部分可靠性数值
//<>Return:返回串联部分可靠性数值
//<>Modified:	2017/05/15 10:10
***************************************************************/
double ReliabilityOfSeries(vector<float> &nSer)
{
    if (nSer.size() == 0)
        return 1;//没有串联，那么可靠性为1
    if (nSer.size() == 1)
        return nSer[0];
    double dReliability = 1.0;
    unsigned int i = 0;
    while (i < nSer.size())
    {
        if ((nSer[i] >= 0) && (nSer[i] <= 1))//检查非法输入
        {
            dReliability *= nSer[i];
            ++i;
        }
        else
            return invalidInput;
    }
    return dReliability;
}
/**************************************************************
//<>Descrip:体系结构可靠性建模中并联系统可靠性计算函数
//<>Call:无
//<>Input:输入为并联部分各功能单元可靠性二维数组
//<>Output:输出为并联部分可靠性数值
//<>Return:返回并联部分可靠性数值
//<>Others:并联、混联和表决系统中，输入的可靠性二维矩阵的列数由
每一条并联支路的最大串联功能单元个数决定，其余并联支路串联个数不
足的请补1
//<>Modified:	2017/05/15 10:10
***************************************************************/
double ReliabilityOfParallel(vector<vector<float> > &nPar)
{
    double dReliability = 0;
    if (nPar.size()==0)
        return 1;//并联部分没有并联，可靠性为1
    if (nPar.size() == 1)
        dReliability = ReliabilityOfSeries(nPar[0]);
    vector<double> dData;
    dData.clear();
    double dResult = 1.0;

    for (unsigned int j = 0; j < nPar.size(); ++j)
    {
        dResult = 1.0;
        for (unsigned int i = 0; i < nPar[j].size(); ++i)
        {
            if ((nPar[j][i] >=0) && (nPar[j][i]<=1))//检查非法输入
            {
                dResult *= nPar[j][i];
            }
            else
                return invalidInput;
        }
        dData.push_back(1 - dResult);
    }
    dResult = 1;
    if (dData.size() > 1)
    {
        for (unsigned int i = 0; i < dData.size(); ++i)
            dResult *= dData[i];
    }
    dReliability = 1 - dResult;
    return dReliability;
}
/**************************************************************
//<>Descrip:体系结构可靠性建模中混联系统可靠性计算函数
//<>Call:ReliabilityOfSeries(vector<float> &Ser)
         ReliabilityOfParallel(vector<vector<float> > &Par)
//<>Input:输入为混联部分各功能单元可靠性数组
//<>Output:输出为混联部分可靠性数值
//<>Return:返回混联部分可靠性数值
//<>Others:并联、混联和表决系统中，输入的可靠性二维矩阵的列数由
每一条并联支路的最大串联功能单元个数决定，其余并联支路串联个数不
足的请补1
//<>Modified:	2017/05/15 10:10
***************************************************************/
double ReliabilityOfSeriesAndParallel(vector<float> &dSer, vector<vector<float> > &dPar)
{
    if ((dSer.size() == 0) && (dPar.size() == 0))
        return 1;
    double dReliability = ReliabilityOfSeries(dSer)*ReliabilityOfParallel(dPar);
    return dReliability;
}
/**************************************************************
//<>Descrip:体系结构可靠性建模中表决类型系统可靠性计算函数
//<>Call:无
//<>Input:输入为表决系统各功能单元可靠性二维数组
//<>Output:输出为表决系统部分可靠性数值
//<>Return:返回表决系统部分可靠性数值
//<>Others:并联、混联和表决系统中，输入的可靠性二维矩阵的列数由
每一条并联支路的最大串联功能单元个数决定，其余并联支路串联个数不
足的请补1
//<>Modified:	2017/05/15 10:10
***************************************************************/
double ReliabilityOfVotingSystem(vector<vector<double> > &dVotSys, int m)
{
    int n = dVotSys.size();//有n个表决功能单元
    if (n == 0 || m <= 0 || m>n)
        return invalidInput;

    vector<double> dData;//计算表决单元路线上串联的结果
    dData.clear();
    double dResultTemp = 1;//计算某一表决单元路线上串联单元连乘结果
    for (unsigned int i = 0; i < dVotSys.size(); ++i)
    {
        for (unsigned int j = 0; j < dVotSys[i].size(); ++j)
        {
            if ((dVotSys[i][j] >= 0) && (dVotSys[i][j] <= 1))
                //检验可靠性数值数据来源是否正确
                dResultTemp *= dVotSys[i][j];
            else
                return invalidInput;
        }
        dData.push_back(dResultTemp);
        dResultTemp = 1;
    }
    double dReliability = 0;//最终计算的可靠性结果，是此函数的返回值
    double dReliabilityTemp1 = 1;//计算可靠性过程中的中间临时计算结果
    double dReliabilityTemp2 = 0;//计算可靠性过程中的中间临时计算结果

    //采用二进制组合算法，从n个数中选m个数,m的取值为（m,n）
    bool bFind = false;//标志位，标志着下面代码的某一次循环之后是否找到满足条件的组合
    for (; m <= n;++m)
    {
        int* pTable = new int[n];       //定义标记buf并将其前m个置1
        memset(pTable, 0, sizeof(int)*n);
        for (int i = 0; i < m; ++i)
            pTable[i] = 1;
        dReliabilityTemp1 = 1;
        dReliabilityTemp2 = 0;
        do
        {
            dReliabilityTemp1 = 1;
            for (int i = 0; i < n; i++)
            {
                if (pTable[i])
                    dReliabilityTemp1 *= dData[i];
                else
                    dReliabilityTemp1 *= (1 - dData[i]);
            }
            dReliabilityTemp2 += dReliabilityTemp1;
            bFind = false;
            for (int i = 0; i < n - 1; ++i)
            {
                if (pTable[i] == 1 && pTable[i + 1] == 0)
                {
                    swap(pTable[i], pTable[i + 1]);    //调换10为01
                    bFind = true;
                    //如果第一位为0，则将第i位置之前的1移到最左边，
                    //如为1则第i位置之前的1就在最左边，无需移动
                    if (pTable[0] == 0)
                    {
                        //O(n)复杂度使1在前0在后
                        for (int k = 0, j = 0; k < i; k++)
                        {
                            if (pTable[k])
                            {
                                swap(pTable[k], pTable[j]);
                                j++;
                            }
                        }
                    }
                    break;
                }
            }
        } while (bFind);
        cout << dReliabilityTemp2 << endl;
        dReliability += dReliabilityTemp2;
        delete[] pTable;
        pTable = NULL;
    }
    return dReliability;
}
/**************************************************************
//<>Descrip:网络拓扑结构结点故障传播模型中计算平均距离L的函数
//<>Call:无
//<>Input:AdjacencyMatrix为邻接矩阵，保存各个结点之间的连接关系，
其中0表示自身，1表示相邻，inf表示不相邻
//<>Output:输出为网络结点平均距离
//<>Return:返回网络结点平均距离
//<>Others:
//<>Modified:	2017/05/15 09:40
***************************************************************/
float SimulationOfAverageDistance(vector<vector<unsigned int> > &nAdjacencyMatrix)
{
    int nRows = nAdjacencyMatrix.size();
    int nCols = nAdjacencyMatrix[0].size();
    if (nRows != nCols)
        return invalidInput;//邻接矩阵一定为方阵
    int n = nRows;//n为总结点数目
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            qDebug()<<nAdjacencyMatrix[i][j];

    vector<int> nData;
    vector<vector<int> > nMatrix;
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            nData.push_back(nAdjacencyMatrix[i][j]);
        }
        nMatrix.push_back(nData);
        nData.clear();
    }

    float fL = 0;//L为平均距离
    unsigned int nSum = 0;//计算L时的中间值
    for (int k = 0; k < n; ++k)
        for (int i = 0; i <n; ++i)
            for (int j = 0; j < n; ++j)
                if (nMatrix[i][j] > nMatrix[i][k] + nMatrix[k][j])
                    nMatrix[i][j] = nMatrix[i][k] + nMatrix[k][j];

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
        {
            if(nMatrix[i][j])
                nSum=nSum+nMatrix[i][j];
        }
    fL = (float)nSum / ((float)n*(float)n);
    return fL;
}
//对所有设备ID进行排序，以后访问设备ID都调用这个函数，否则设备ID的读取顺序不一样（因为用到了map）
const QList<uint> GetDeviceId()
{
    QList<uint> unDeviceId=sys_inf->Get_DeviceId_Vector();
    qSort(unDeviceId.begin(),unDeviceId.end());
    return  unDeviceId;
}
//从系统描述中获得邻接矩阵
const vector<vector<unsigned int> > GetAdjacencyMatrix()
{
    QList<uint> unDeviceId=GetDeviceId();
    const unsigned int N=unDeviceId.size();//所有节点的个数，构成N*N的邻接矩阵
    uint unIndex[N];//节点ID是用户命名，无序，例如5,7,9，需要进行索引
    for(unsigned int i=0;i<N;++i)
    {
        unIndex[i]=unDeviceId[i];
    }
    unsigned int unAdjacencyMatrix[N][N];
    for(int i=0;i<N;++i)
        for(int j=0;j<N;++j)
        {
            if(j == i)
                unAdjacencyMatrix[i][j]=0;
            else
                unAdjacencyMatrix[i][j]=8;
        }
    for(unsigned int i=0;i<N;++i)
    {
        Device_Informa Device_Index=sys_inf->Get_SingleDevice(unDeviceId[i]);
        const QList<uint> &devicelinked=Device_Index._Get_Devicelink();
        for(int j=0;j<devicelinked.size();++j)
        {
            unsigned int k=0;
            for(;k<N;++k)
            {
                if(unIndex[k] == devicelinked[j])
                    break;
            }
            unAdjacencyMatrix[i][k]=1;
        }
    }
    vector<uint> column;
    vector<vector<uint> > AdjacencyMatrix;
    for(unsigned int i=0;i<N;++i)
    {
        for(unsigned int j=0;j<N;++j)
            column.push_back(unAdjacencyMatrix[i][j]);
        AdjacencyMatrix.push_back(column);
        column.clear();
    }
    return AdjacencyMatrix;
}

/**************************************************************
//<>Descrip:网络拓扑结构结点故障传播模型中计算聚类指数C的函数
//<>Call:无
//<>Input:AdjacencyMatrix为网络拓扑邻接矩阵，保存各个结点之间的
连接关系，其中，1表示相邻，0表示不相邻
//<>Output:输出为网络结点聚类指数
//<>Return:返回网络结点聚类指数
//<>Others:
//<>Modified:	2017/05/15 09:40
***************************************************************/
double ClusteringIndex(const vector<vector<unsigned int> > &nAdjacencyMatrix)
{
    int nRows = nAdjacencyMatrix.size();
    int nCols = nAdjacencyMatrix[0].size();
    if (nRows != nCols)
        return invalidInput;//邻接矩阵一定为方阵
    int n = nRows;//结点个数
    vector<int> nSubscript;//存储与某个结点直接相连的k个其它点的下标
    nSubscript.clear();
    vector<vector<int> >e;//存储所有结点直接相连的其它点的下标
    vector<int> K;//存储计算出的所有结点的K值
    K.clear();
    vector<int> E;//存储计算出的所有结点的E值
    int nCountOfK = 0;
    int nCountOfE = 0;

     //求取每个结点的K值
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if ((nAdjacencyMatrix[i][j]&0x1) == 1)
            {
                ++nCountOfK;
                nSubscript.push_back(j);
            }
        }
        K.push_back(nCountOfK);
        nCountOfK = 0;
        e.push_back(nSubscript);
        nSubscript.clear();
    }
    //求取每个结点的E值
    for (int l = 0; l < n; ++l)
    {
        nCountOfE = 0;
        for (unsigned int p = 0; p < e[l].size(); ++p)
        {
            unsigned int h = p;
            for (unsigned int q = e[l][h]; h+1 < e[l].size(); ++h)
            {
                int i = e[l][h + 1];
                if ((nAdjacencyMatrix[q][i]&0x1) == 1)
                    ++nCountOfE;
            }
        }
       E.push_back(nCountOfE);
    }
    //求取每个结点的C值
    double dSum = 0;
    for (int i = 0; i < n; ++i)
    {
        if (K[i] > 1)
        {
            double dTemp1 = K[i];
            double dTemp2 = E[i];
            dSum+= 2 * dTemp2 / (dTemp1*(dTemp1 - 1));
        }
        else
            continue;
    }
    double C = 0;
    C = dSum / (double)n;
    return C;
}

/**************************************************************
//<>Descrip:网络拓扑结构结点冗余性评估模型中计算度K的函数
//<>Call:无
//<>Input:AdjacencyMatrix为网络拓扑邻接矩阵，保存各个结点之间的
连接关系，其中0表示自身，1表示相邻，inf表示不相邻
//<>Output:输出为网络中各个结点的度
//<>Return:返回网络中各个结点的度
//<>Others:
//<>Modified:	2017/05/15 09:40
***************************************************************/
QList<uint> DegreeDistribution(const vector<vector<unsigned int> > &nAdjacencyMatrix)
{
    QList<uint> K;//存储计算出的所有结点的K值
    K.clear();
    int nRows = nAdjacencyMatrix.size();
    int nRols = nAdjacencyMatrix[0].size();
    if (nRows != nRols)
        return K;//邻接矩阵一定为方阵
    int n = nRows;//结点个数
    int nCountOfK = 0;

    //求取每个结点的K值
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if ((nAdjacencyMatrix[i][j] & 0x1) == 1)
            {
                ++nCountOfK;
            }
        }
        K.push_back(nCountOfK);
        nCountOfK = 0;
    }
    //根据数组的内容对数组下标即结点ID进行排序，得出关键结点
    //在函数中用nKIndex，用于存储下标。
    //数组值在排序的过程中，下标值也跟着排序
    QList<uint> nKIndex=GetDeviceId();
     for (int i = 0;i < n-1;++i)
     {
         for(int j = 0;j < n-i-1;++j)
         {
             if (K[j] > K[j+1])//按照从小到大的顺序排列
            {
                int temp = K[j];
                K[j] = K[j+1];
                K[j+1] = temp;

                temp = nKIndex[j];
                nKIndex[j] = nKIndex[j+1];
                nKIndex[j+1] = temp;
             }
         }
     }
    return nKIndex;
}

//提供给显示图片函数的接口函数,画每个子系统的图片，返回子系统的名称和子系统中每个节点的信息
QMap<QString,QVector<InfoNode> > GetInfoForSubSystem_TuopuGraphviz()
{
    InfoNode node;
    InfoRecord nodeLinked;
    QVector<InfoRecord> LinkedNodes;
    QVector<InfoNode> nodeInfo;//每个子系统中所有节点的信息
    QMap<QString,QVector<InfoNode> > result;

    //子系统的名称为中文，但是生成图片的名称不能为中文，因此对应子系统编号重新定义子系统图片名称
    for(QMap<QString,uint>::iterator iter=subsystem.begin();iter!=subsystem.end();iter++)
    {
        //以子系统编号进行区分
        QString name="tuopuImage"+QString::number(iter.value());
        subsystemForInternet.insert(name,iter.value());
    }

    QString subSystemName;//子系统的名字
    QList<uint> unDeviceId;
    if(sys_inf!=NULL)
        unDeviceId=sys_inf->Get_DeviceId_Vector();

    for(int i=0;i<unDeviceId.size();)
    {
        Device_Informa Device_Index=sys_inf->Get_SingleDevice(unDeviceId[i]);
        uint subSystemNum=Device_Index._Get_SystemType();//此设备所在的子系统

        while(i<unDeviceId.size() && (subSystemNum == sys_inf->Get_SingleDevice(unDeviceId[i])._Get_SystemType()))
        {
            Device_Index=sys_inf->Get_SingleDevice(unDeviceId[i]);
            node.devNumber=unDeviceId[i];

            const QString &nodeName=Device_Index._Get_DeviceName();
            node.devName=nodeName.toStdString();
            const QList<uint> &devicelinked=Device_Index._Get_Devicelink();

            for(int j=0;j<devicelinked.size();++j)
            {
                uint linkedNodeSubSystem=sys_inf->Get_SingleDevice(devicelinked[j])._Get_SystemType();
                //相邻节点所在子系统也在该节点所在子系统内，则在邻接节点中添加该节点
                if(linkedNodeSubSystem == subSystemNum)
                {
                    nodeLinked.devNumber=devicelinked[j];
                    const QString& deviceName=sys_inf->Get_SingleDevice(devicelinked[j])._Get_DeviceName();
                    nodeLinked.devName=deviceName.toStdString();
                    LinkedNodes.push_back(nodeLinked);
                }
            }
            node.adjacentNode=LinkedNodes;
            for(QMap<QString,uint>::iterator iter=subsystemForInternet.begin();iter!=subsystemForInternet.end();iter++)
            {
                if(iter.value() == subSystemNum)
                {
                    subSystemName=iter.key();
                    break;
                }
            }
            nodeInfo.push_back(node);
            LinkedNodes.clear();
            i++;
        }
        result.insert(subSystemName,nodeInfo);
        nodeInfo.clear();
    }

    for(QMap<QString,QVector<InfoNode> >::iterator it=result.begin();it!=result.end();++it)
    {
        //qDebug()<<it.key();
        QVector<InfoNode> a=it.value();
        for(int i=0;i<a.size();++i)
        {
            qDebug()<<a[i].devNumber;
            QVector<InfoRecord> b=a[i].adjacentNode;
            for(int j=0;j<b.size();++j)
                qDebug()<<b[j].devNumber;

        }

    }
    return result;
}

//提供给显示图片函数的接口函数,画主系统的图片，返回主系统的名称和主系统中每个子系统的信息
QMap<QString,QVector<InfoNode> > GetInfoForMainSystem_TuopuGraphviz()
{
    QString mainSystemName="zhuxitong";
    InfoNode subSystem;
    InfoRecord subSystemLinked;
    QVector<InfoRecord> LinkedsubSystem;
    QVector<InfoNode> subSystemInfo;//每个子系统中所有节点的信息
    QMap<QString,QVector<InfoNode> > result;

    QList<uint> unChannelId=sys_inf->Get_ChannelId_Vector();//得到所有链路的集合
    uint subSystemNum1,subSystemNum2;
    QString subSystemName1,subSystemName2;
    for(int i=0;i<unChannelId.size();i++)
    {
        const QPair<uint,uint>& eachChannel=sys_inf->Get_SingleChannel(unChannelId[i])._Get_Channel();
        uint node1=eachChannel.first;//设备ID
        uint node2=eachChannel.second;
        subSystemNum1=sys_inf->Get_SingleDevice(node1)._Get_SystemType();
        subSystemNum2=sys_inf->Get_SingleDevice(node2)._Get_SystemType();
        for(QMap<QString,uint>::iterator iter=subsystem.begin();iter!=subsystem.end();iter++)
        {
            if(iter.value() == subSystemNum1)
                subSystemName1=iter.key();
            if(iter.value() == subSystemNum2)
                subSystemName2=iter.key();
        }
        if(subSystemNum1 != subSystemNum2)
        {
            //是否已经有此子系统的节点
            bool hasSubSystem=false;
            for(int j=0;j<subSystemInfo.size();j++)
            {
                //有此子系统的节点
                if(subSystemNum1 == subSystemInfo[j].devNumber)
                {
                    hasSubSystem=true;
                    LinkedsubSystem=subSystemInfo[j].adjacentNode;
                    bool find=false;
                    for(int k=0;k<LinkedsubSystem.size();k++)
                    {
                        //uint subSystemNumTemp=sys_inf->Get_SingleDevice(LinkedsubSystem[k].devNumber)._Get_SystemType();
                        //有此子系统的节点，且有其关联子系统的节点
                        if(subSystemNum2 == LinkedsubSystem[k].devNumber)
                        {find=true;break;}
                    }
                    //有此子系统的节点，但没有其关联子系统的节点
                    if(!find)
                    {
                        subSystemLinked.devNumber=subSystemNum2;
                        subSystemLinked.devName=subSystemName2.toStdString();
                        subSystemInfo[j].adjacentNode.push_back(subSystemLinked);
                    }
                }
            }
            //没有此子系统的节点
            if(!hasSubSystem)
            {
                subSystem.devNumber=subSystemNum1;
                subSystem.devName=subSystemName1.toStdString();
                subSystemLinked.devNumber=subSystemNum2;
                subSystemLinked.devName=subSystemName2.toStdString();
                subSystem.adjacentNode.push_back(subSystemLinked);
                subSystemInfo.push_back(subSystem);
            }
        }
    }
    result.insert(mainSystemName,subSystemInfo);

    for(QMap<QString,QVector<InfoNode> >::iterator it=result.begin();it!=result.end();++it)
    {
        //qDebug()<<it.key();
        QVector<InfoNode> a=it.value();
        for(int i=0;i<a.size();++i)
        {
            //qDebug()<<"子系统"<<a[i].devNumber;
            QVector<InfoRecord> b=a[i].adjacentNode;
            for(int j=0;j<b.size();++j)
                qDebug()<<"子系统的关联系统"<<b[j].devNumber;

        }
    }
    return result;
}

//拓扑安全性评估中，把输入的数据进行处理，得到每个子系统所有设备id及其可靠性
QMap<uint,QMap<uint,double> > GetSubSystemID_AndDevIDRel_ForTuopu()
{
    QMap<uint,double> map_DevIDRel;
    QMap<uint,QMap<uint,double> > map_SubSystemID_AndDevIDRel;
    QMap<uint,uint> subsys_count;
    QList<uint> unDeviceId;
    if(sys_inf!=NULL)
        unDeviceId=sys_inf->Get_DeviceId_Vector();

    for(int i=0;i<unDeviceId.size();i++)  //判断子系统的个数，分开存储
    {
        Device_Informa Device_Index=sys_inf->Get_SingleDevice(unDeviceId[i]);
        uint subSystemNum=Device_Index._Get_SystemType();//此设备所在的子系统
        //qDebug()<<"此设备编号是"<<unDeviceId[i]<<"所在子系统是"<<subSystemNum;
        subsys_count.insert(subSystemNum,1);
     }

    for(int count=1;count<subsys_count.size()+1;count++)  //按子系统类别，分开查找存储
    {
        for(int i=0;i<unDeviceId.size();i++)
        {
            Device_Informa Device_Index=sys_inf->Get_SingleDevice(unDeviceId[i]);
            uint subSystemNum=Device_Index._Get_SystemType();//此设备所在的子系统
            double temp=1;//每个设备的可用性
            if(subSystemNum==count)
            {
                Device_Index=sys_inf->Get_SingleDevice(unDeviceId[i]);
                QString deviceName=Device_Index._Get_DeviceName();
                bool flag=false;
                for(int j=0;j!=3;j++)
                {
                    //qDebug()<<"进入查找设备--"<<deviceName;
                    if(index_select[j]->m_ID_calculateResult.find(deviceName)!=index_select[j]->m_ID_calculateResult.end())
                    {
                        temp= index_select[j]->m_ID_calculateResult[deviceName][2]/100.0;
                        flag=true;
                        //qDebug()<<"找到设备--"<<deviceName<<"--是："<<index_select[j]->m_ID_calculateResult[deviceName][2]<<"分";
                        break;
                    }
                }
                map_DevIDRel.insert(Device_Index._Get_Deviceid(),temp);
            }
       }
       map_SubSystemID_AndDevIDRel.insertMulti(count,map_DevIDRel);
       map_DevIDRel.clear();
       qDebug()<<"输出结果为： "<<map_SubSystemID_AndDevIDRel;
    }

    /* for(QMap<uint,QMap<uint,double> >::iterator iter=map_SubSystemID_AndDevIDRel.begin();iter!=map_SubSystemID_AndDevIDRel.end();iter++)
     {

         for(QMap<uint,double>::iterator i=iter.value().begin();i!=iter.value().end();i++)
             qDebug()<<"第二个QMap--"<<i.key()<<"---"<<i.value();
     }*/
    return map_SubSystemID_AndDevIDRel;

}






/*QMap<QString,QVector<InfoNode> > GetInfoForMainSystem_TuopuGraphviz()
{
    QString mainSystemName="zhuxitong";
    InfoNode subSystem;
    InfoRecord subSystemLinked;
    QVector<InfoRecord> LinkedsubSystem;
    QVector<InfoNode> subSystemInfo;//每个子系统中所有节点的信息
    QMap<QString,QVector<InfoNode> > result;

    QMap<QString,QVector<InfoNode> > subSystemNameAndInfo=GetInfoForSubSystem_TuopuGraphviz();
    for(QMap<QString,uint>::iterator iter=subsystem.begin();iter!=subsystem.end();iter++)
    {
        for(QMap<QString,QVector<InfoNode> >::iterator it=subSystemNameAndInfo.begin();it!=subSystemNameAndInfo.end();it++)
        {
            if(iter.key()==it.key())//子系统名字相同
            {
                //遍历子系统中每个节点相邻节点所在的子系统
                for(int i=0;i<it.value().size();++i)
                {
                    //每个节点的相邻节点
                    const QList<uint> &devicelinked=sys_inf->Get_SingleDevice(it.value()[i].devNumber)._Get_Devicelink();

                    //遍历相邻节点所在子系统
                    for(int k=0;k<devicelinked.size();++k)
                    {
                        uint subSystemNum=sys_inf->Get_SingleDevice(devicelinked[k])._Get_SystemType();

                        if(subSystemNum != iter.value())//所在子系统不相同
                        {

                            subSystem.devNumber=iter.value();
                            subSystem.devName=iter.key().toStdString();
                            subSystemLinked.devNumber=subSystemNum;
                            for(QMap<QString,uint>::iterator iter1=subsystem.begin();iter1!=subsystem.end();iter1++)
                            {
                                if(iter1.value() == subSystemNum)
                                    subSystemLinked.devName=iter1.key().toStdString();
                            }
                            LinkedsubSystem.push_back(subSystemLinked);
                        }
                    }
                    subSystem.adjacentNode=LinkedsubSystem;
                    LinkedsubSystem.clear();
                }
                subSystemInfo.push_back(subSystem);
            }
        }
    }
    result.insert(mainSystemName,subSystemInfo);

    for(QMap<QString,QVector<InfoNode> >::iterator it=result.begin();it!=result.end();++it)
    {
        qDebug()<<it.key();
        QVector<InfoNode> a=it.value();
        for(int i=0;i<a.size();++i)
        {
            qDebug()<<"子系统"<<a[i].devNumber;
            QVector<InfoRecord> b=a[i].adjacentNode;
            for(int j=0;j<b.size();++j)
                qDebug()<<"子系统的关联系统"<<b[j].devNumber;

        }

    }
    return result;
}*/
