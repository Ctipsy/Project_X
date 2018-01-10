#include<Windows.h>
#include<WinNT.h>
#include<WinBase.h>
#include<tchar.h>

#include<string>
#include<fstream>

#include"graphviz/graphviz.h"
using namespace std;


void VizObject::SetNodeProperty(ofstream &oOutFileStream, const QString &nodeType)   //nodeType参数是用来从graphviz.ini文件中读取配置信息，各项参数都在ini文件里
{
	char tmp[100];
    char *sr[] = {"shape", "style", "color", "fontname", "fontsize"};
	int len = sizeof(sr) / sizeof(sr[0]);
    oOutFileStream << "\tnode [";   //GetPrivateProfileStringA:读取配置文件ini
	for (int k = 0; k < len; ++k)
	{
        GetPrivateProfileStringA(nodeType.toStdString().c_str(), sr[k], "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());
        oOutFileStream << sr[k] <<  " = " << "\"" <<tmp <<"\"";
		if (k == len - 1)
			oOutFileStream << "];" << endl;
		else
			oOutFileStream << ", ";
	}
}

void VizObject::SetEdgeProperty(ofstream &oOutFileStream, const QString &edgeType)
{
	char tmp[100];
    char *sr[] = {"style", "color", "fontname", "fontsize", "penwidth"};
	int len = sizeof(sr) / sizeof(sr[0]);

	oOutFileStream << "\tedge [";
	for (int i = 0; i < len; ++i)
	{
        GetPrivateProfileStringA(edgeType.toStdString().c_str(), sr[i], "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());
        oOutFileStream << sr[i] << " = " << "\"" <<tmp <<"\"";

		if (i == len - 1)
			oOutFileStream << "];" << endl;
		else
			oOutFileStream << ", ";
	}
}

void VizObject::FoutputAndImage(int strCurrentTaskNum, const QString &strObj, ofstream &oOutFileStream)
{
    oOutFileStream << "\tand_" << strObj.toStdString() << "_"<< strCurrentTaskNum;
	char tmp[100];
	oOutFileStream << "[label = \"and\", shape = \"none\", image = \"" ;
    GetPrivateProfileStringA("image", "andImage", "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());
	oOutFileStream << tmp << "\"];" << endl;
    oOutFileStream << "\tand_" << strObj.toStdString() << "_"<< strCurrentTaskNum << "->" << strCurrentTaskNum << ";" << endl;
}

void VizObject::FoutputOrImage(int strCurrentTaskNum, const QString &strObj, ofstream &oOutFileStream)
{
    oOutFileStream << "\tor_" << strObj.toStdString() << "_"<< strCurrentTaskNum << "[label = \"or\", shape = \"none\", image = \"";
	char tmp[100];
    GetPrivateProfileStringA("image", "orImage", "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());
	oOutFileStream << tmp << "\"];" << endl;
    oOutFileStream << "\tor_" << strObj.toStdString() << "_"<< strCurrentTaskNum << "->" <<
        strCurrentTaskNum << ";" << endl;
}

void VizObject::FoutputVoteImage(int strCurrentTaskNum, const QString &strObj, ofstream &oOutFileStream)
{
    oOutFileStream << "\tvote_" << strObj.toStdString() << "_"<< strCurrentTaskNum << "[label = \"\", shape = \"none\", image = \"";
	char tmp[100];
    GetPrivateProfileStringA("image", "voteImage", "", tmp, sizeof(tmp), INI_PATH.toStdString().c_str());
	oOutFileStream << tmp << "\"];" << endl;
    oOutFileStream << "\tvote_" << strObj.toStdString() << "_"<< strCurrentTaskNum << "->" << strCurrentTaskNum << ";" << endl;
}
