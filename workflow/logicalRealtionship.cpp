#include<QVector>


#include "workflow/logicalRealtionship.h"

using namespace std;


double SelectOneFromGroup(const QVector<double> &dVal)
{
    double ans = 1.0;
//    for (int i = 0; i < dVal.size(); ++i)
//    {
//        if (dVal[i] > 1 || dVal[i] < 0)
//            return -1;

//        double temp = dVal[i];
//        for (int j = 0; j < dVal.size(); ++j)
//        {
//            if (j != i)
//            temp *= (1 - dVal[j]);
//        }
//        ans += temp;
//    }
    for (int i = 0; i < dVal.size(); ++i)
    {
        ans *= (1 - dVal[i]);
    }
    return 1 - ans;
}

double VoteRelation(const QVector<double> &dVal, int n)
{
    double ans = 0;
    if (dVal.size() <= 0 || n > dVal.size() || n == 0)
        return -1;
    if (n == 1)
    {
        return SelectOneFromGroup(dVal);
    }
    if (n == dVal.size())
    {
        ans = 1.0;
        for (int i = 0; i < dVal.size(); ++i)
            ans *= dVal[i];
        return ans;
    }

    QVector<double> dTemp(dVal);
    dTemp.pop_front();

    return dVal[0] * VoteRelation(dTemp, n - 1) + (1 - dVal[0]) * VoteRelation(dTemp, n);
}


double LogicalAndRelationship(const QVector<double> &dArray)
{
//	double dAns = 1.0;
//	for (int i = 0; i < dArray.size(); ++i)
//	{
//        if (dArray[i] >= 0 && dArray[i] <= 1.0)
//            dAns *= dArray[i];
//        else
//            return -1;
//	}
//	return dAns;
    return VoteRelation(dArray, dArray.size());
}

double LogicalSelectNFromM(const QVector<double> &dArray, int nSelect)
{
    return VoteRelation(dArray, nSelect);
}

double LogicalOrRelationship(const QVector<double> &dArray)
{
    return VoteRelation(dArray, 1);
}

double LogicalVoteRelationship(const QVector<double> &dArray, int nSelect)
{
    return VoteRelation(dArray, nSelect);
}




double LogicalRelationship(const QVector<double> &dArray, LogicalRelation oRelation)
{
	switch (oRelation.rel)
	{
	case AND:
		return LogicalAndRelationship(dArray);
	case OR:
		return LogicalOrRelationship(dArray);
	case VOTE:
		return LogicalVoteRelationship(dArray, oRelation.nVoteN);
	default:
		if (dArray.size() > 0)
			return dArray[0];
	}
}
