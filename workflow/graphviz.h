#ifndef _GRAPHVIZ_H
#define _GRAPHVIZ_H

#include<QString>
#include<vector>

using namespace std;




const QString INI_PATH("./graphviz/graphviz.ini");



//void DealWithTaskRelation(const string &strCurrentTaskName, )


class VizObject
{
public:
	//virtual void GraphViz(const string &strFileName) = 0;

protected:
    void SetNodeProperty(ofstream &oOutFileStream, const QString &nodeType);
    void SetEdgeProperty(ofstream &oOutFileStream, const QString &edgeType);

    void FoutputAndImage(const QString &strCurrentTaskName, const QString &strObj, ofstream &oOutFileStream);
    void FoutputOrImage(const QString &strCurrentTaskName, const QString &strObj, ofstream &oOutFileStream);
    void FoutputVoteImage(const QString &strCurrentTaskName, const QString &strObj, ofstream &oOutFileStream);
};




#endif 
