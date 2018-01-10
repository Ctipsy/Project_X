#include<Windows.h>
#include<WinNT.h>
#include<WinBase.h>
#include<tchar.h>


#include<string>
#include<fstream>



#include"graphviz/graphviz.h"

using namespace std;



void VizObject::SetNodeProperty(ofstream &oOutFileStream, const QString &nodeType)
{
	char tmp[100];
	char *sr[] = {"shape", "style", "color"};
	int len = sizeof(sr) / sizeof(sr[0]);
	//oOutFileStream << "node [shape = ellipse, style = filled, color = lightblue];" << endl;
	oOutFileStream << "\tnode [";
	for (int k = 0; k < len; ++k)
	{
        GetPrivateProfileStringA(nodeType.toStdString().c_str(), sr[k], "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());
		oOutFileStream << sr[k] << " = " << tmp;
		if (k == len - 1)
			oOutFileStream << "];" << endl;
		else
			oOutFileStream << ", ";
	}
}

void VizObject::SetEdgeProperty(ofstream &oOutFileStream, const QString &edgeType)
{
	char tmp[100];
	char *sr[] = {"style", "color"};
	int len = sizeof(sr) / sizeof(sr[0]);

	oOutFileStream << "\tedge [";
	for (int i = 0; i < len; ++i)
	{
        GetPrivateProfileStringA(edgeType.toStdString().c_str(), sr[i], "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());
		oOutFileStream << sr[i] << " = " << tmp;

		if (i == len - 1)
			oOutFileStream << "];" << endl;
		else
			oOutFileStream << ", ";
	}
}

void VizObject::FoutputAndImage(const QString &strCurrentTaskName, const QString &strObj, ofstream &oOutFileStream)
{
    oOutFileStream << "\tand_" << strObj.toStdString() << "_"<< strCurrentTaskName.toStdString();
	char tmp[100];
	oOutFileStream << "[label = \"and\", shape = \"none\", image = \"" ;
    GetPrivateProfileStringA("image", "andImage", "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());
	oOutFileStream << tmp << "\"];" << endl;
    oOutFileStream << "\tand_" << strObj.toStdString() << "_"<< strCurrentTaskName.toStdString() << "->" << strCurrentTaskName.toStdString() << ";" << endl;
}

void VizObject::FoutputOrImage(const QString &strCurrentTaskName, const QString &strObj, ofstream &oOutFileStream)
{
    oOutFileStream << "\tor_" << strObj.toStdString() << "_"<< strCurrentTaskName.toStdString() << "[label = \"or\", shape = \"none\", image = \"";
	char tmp[100];
    GetPrivateProfileStringA("image", "orImage", "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());
	oOutFileStream << tmp << "\"];" << endl;
    oOutFileStream << "\tor_" << strObj.toStdString() << "_"<< strCurrentTaskName.toStdString() << "->" <<
        strCurrentTaskName.toStdString() << ";" << endl;
}

void VizObject::FoutputVoteImage(const QString &strCurrentTaskName, const QString &strObj, ofstream &oOutFileStream)
{
    oOutFileStream << "\tvote_" << strObj.toStdString() << "_"<< strCurrentTaskName.toStdString() << "[label = \"\", shape = \"none\", image = \"";
	char tmp[100];
    GetPrivateProfileStringA("image", "voteImage", "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());
	oOutFileStream << tmp << "\"];" << endl;
    oOutFileStream << "\tvote_" << strObj.toStdString() << "_"<< strCurrentTaskName.toStdString() << "->" << strCurrentTaskName.toStdString() << ";" << endl;
}
