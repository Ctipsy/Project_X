#ifndef _LOGICAL_RELATIONSHIP
#define _LOGICAL_RELATIONSHIP

#include<vector>
#include <QVector>

using namespace std;

enum Relation
{
	SERIAL, AND, OR, VOTE
};

struct LogicalRelation
{
	Relation rel;
	int nVoteM;
	int nVoteN;
};

double SelectOneFromGroup(const QVector<double> &dVal);    //当且仅当有一个有效，其余的都是失效

double VoteRelation(const QVector<double> &dVal, int n);    //表决模型，至少有n个有效

double LogicalAndRelationship(const QVector<double> &dArray);   //逻辑与模型，所有都必须有效


double LogicalOrRelationship(const QVector<double> &dArray);     //逻辑或模型，至少有一个有效


double LogicalVoteRelationship(const QVector<double> &dArray, int nSelect);

double LogicalRelationship(const QVector<double> &dArray, LogicalRelation oRelation);  //将所有逻辑模型统一成一个接口函数

#endif
