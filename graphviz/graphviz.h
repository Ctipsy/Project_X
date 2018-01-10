#ifndef _GRAPHVIZ_H
#define _GRAPHVIZ_H

#include<QString>
#include<vector>

using namespace std;




//const QString INI_PATH("./graphviz/graphviz.ini");   //C:\719\2017_06_10_Total\graphviz

const QString INI_PATH("D:/Program Files (x86)/X_Ship/graphviz/graphviz.ini");


//void DealWithTaskRelation(const string &strCurrentTaskName, )


class VizObject
{
public:
	//virtual void GraphViz(const string &strFileName) = 0;

protected:
    void SetNodeProperty(ofstream &oOutFileStream, const QString &nodeType);
    void SetEdgeProperty(ofstream &oOutFileStream, const QString &edgeType);


    void FoutputAndImage(int strCurrentTaskNum, const QString &strObj, ofstream &oOutFileStream);
    void FoutputOrImage(int strCurrentTaskNum, const QString &strObj, ofstream &oOutFileStream);
    void FoutputVoteImage(int strCurrentTaskNum, const QString &strObj, ofstream &oOutFileStream);
};




#endif 
