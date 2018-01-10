#ifndef INTERNETFUNCTION_H
#define INTERNETFUNCTION_H

#include <QMainWindow>
#include <iostream>
#include <vector>

#include <QStringList>
#include <QVector>
#include <QSqlQuery>

#include"graphviz/internet_info_graphviz.h"

#define invalidInput -1 //用于检测输入的可靠性矩阵数值是否正确
using namespace std;


double ReliabilityOfSeries(vector<float> &dSer);
double ReliabilityOfParallel(vector<vector<float> > &dPar);
double ReliabilityOfSeriesAndParallel(vector<float> &dSer, vector<vector<float> > &dPar);
double ReliabilityOfVotingSystem(vector<vector<float> > &dVotSys, int m);

const QList<uint> GetDeviceId();
const vector<vector<unsigned int> > GetAdjacencyMatrix();
float SimulationOfAverageDistance(vector<vector<unsigned int> > &nAdjacencyMatrix);
double ClusteringIndex(const vector<vector<unsigned int> > &nAdjacencyMatrix);
QList<uint> DegreeDistribution(const vector<vector<unsigned int> > &nAdjacencyMatrix);
QMap<uint, QMap<uint, double> > GetSubSystemID_AndDevIDRel_ForTuopu();



//函数在涛师兄那里，集成软件时，删除这一部分，并添加相应的头文件
//struct InfoRecord
//{
//    std::string devName;
//    unsigned int devNumber;

//};

//struct InfoNode
//{
//    std::string devName;
//    unsigned int devNumber;
//    std::QVector<InfoRecord> adjacentNode;
//};
QMap<QString,QVector<InfoNode> > GetInfoForSubSystem_TuopuGraphviz();
QMap<QString,QVector<InfoNode> > GetInfoForMainSystem_TuopuGraphviz();
#endif // INTERNETFUNCTION_H
