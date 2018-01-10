#include "ui_mainwindow.h"
#include "questionnaire_analysis.h"
#include "sys_discript/base_informa.h"
#include "sys_discript/system_informa.h"
#include <vector>
#include <iostream>
#include <QMap>
#include <stdlib.h>
#include <stdio.h>

#include <QSqlDatabase>
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
using namespace std;
extern QSqlDatabase m_dbConnectPro;//全局变量打开数据库文件
#define invalidInput -1 //用于检测计算问卷时的异常情况
/**************************************************************
//<>Descrip:问卷结果解析，访问并解析Access数据库内容
//<>Call:QuestionnaireAnalysisCore(QSqlQuery &oQuerySlow,int nOptionNumber)
//<>Input:输入为字符串列表tables，通过tables可以访问各问卷结果统计表
//<>Output:输出为每套问卷的最终分数，保存在vector容器中
//<>Return:返回每套问卷的最终分数，保存在vector容器中
//<>Others:一套问卷题目选项个数可以不一致，但是所有问卷中相同选项个数的各选项量化分数一样
//<>Modified:	2017/05/24 21:10
***************************************************************/
void QuestionnaireAnalysis()
{
    //resetIndex();
    QString strTabName="Lj_Result";
    QString strTbableName;//结果存到对应的指标树的表中

    QString strTmp;
    QSqlQuery oQueryFast(m_dbConnectPro);
    QSqlQuery oQuerySlow(m_dbConnectPro);
    strTmp = "select * from " + strTabName+" ORDER BY result_sid ASC,result_index ASC,result_tid ASC";
    oQueryFast.exec(strTmp);
    oQuerySlow.exec(strTmp);
    oQueryFast.next();

    while(oQueryFast.isValid() && oQueryFast.value(1).toInt() > 0 && oQueryFast.value(1).toInt() <= 300)
    {
        qDebug()<<"进入while循环";
        int nQuestionnaireNumber=oQueryFast.value(1).toInt();//获取当前的问卷号
        qDebug()<<nQuestionnaireNumber;
        if(nQuestionnaireNumber>0 && nQuestionnaireNumber<=10)
            strTbableName="服务器设备指标树";
        else if(nQuestionnaireNumber>10 && nQuestionnaireNumber<=20)
            strTbableName="控制设备指标树";
        else if(nQuestionnaireNumber>20 && nQuestionnaireNumber<=30)
            strTbableName="显控设备指标树";
        else if(nQuestionnaireNumber>100 && nQuestionnaireNumber<=200)
            strTbableName="网络架构指标树";
        else if(nQuestionnaireNumber>200 && nQuestionnaireNumber<=300)
            strTbableName="信息流指标树";
        else
            qDebug()<<"没有找到对应的指标树";
        QVector<questionnaire> oneQustionnaireRes;//一套问卷的计算结果
        CalculateQuestionnaire(oQueryFast,oQuerySlow,nQuestionnaireNumber,oneQustionnaireRes);
        writeResultToDB(oneQustionnaireRes,strTbableName);
        if(oQueryFast.isValid() == 0)
            break;
    }
    clearLjResult();
}
//计算一套问卷,返回值为
void CalculateQuestionnaire(QSqlQuery &oQueryFast,QSqlQuery &oQuerySlow,int nQuestionnaireNumber,QVector<questionnaire> &oneQustionnaireRes)
{
    int nZhibiao=oQueryFast.value(2).toInt();//获取当前原子指标
    int nQuestionNumber=oQueryFast.value(3).toInt();//获取当前题目号
    int count=0;//原子指标对应的题目个数
    int nOptionNumber=0;//选项个数
    //oQueryFast.previous();
    questionnaire oneIndexInfo;//定义一个原子指标的信息
    oneIndexInfo.averageGrade=0;
    oneIndexInfo.gradeSum=0;
    oneIndexInfo.peopleCount=0;
    while(oQueryFast.isValid() && (nQuestionnaireNumber == oQueryFast.value(1).toInt()))//问卷号相同
    {
        if(oQueryFast.value(2).toInt() == nZhibiao)//原子指标也相同
        {
            oneIndexInfo.indexNum=nZhibiao;
            qDebug()<<"原子指标号为"<<oneIndexInfo.indexNum;
            if(oQueryFast.value(3).toInt() == nQuestionNumber)//题目号相同
            {
                ++nOptionNumber;
                qDebug()<<"题号相同";
            }
            else//一个原子指标对应多道题目
            {
                qDebug()<<"题号不相同";
                QPair<int,int> oneQuestionRes=QuestionnaireAnalysisCore(oQuerySlow,nOptionNumber);
                if(oneQuestionRes.first != invalidInput)
                {
                    oneIndexInfo.gradeSum += oneQuestionRes.first;
                    oneIndexInfo.peopleCount = oneQuestionRes.second;
                }
                else
                {
                    oneIndexInfo.gradeSum = invalidInput;
                    oneIndexInfo.peopleCount = 0;
                }
                count++;
                nQuestionNumber=oQueryFast.value(3).toInt();
                nOptionNumber=1;
            }
        }
        else
        {
            count++;
            oneIndexInfo.questionNum=count;
            QPair<int,int> oneQuestionRes=QuestionnaireAnalysisCore(oQuerySlow,nOptionNumber);
            if(oneQuestionRes.first != invalidInput)
            {
                oneIndexInfo.gradeSum += oneQuestionRes.first;
                oneIndexInfo.peopleCount = oneQuestionRes.second;
            }
            else
            {
                oneIndexInfo.gradeSum = invalidInput;
                oneIndexInfo.peopleCount = 0;
            }
            if(oneIndexInfo.peopleCount != 0 && oneIndexInfo.questionNum != 0)
            {
                qDebug()<<"总分数"<<oneIndexInfo.gradeSum<<"人数为"<<oneIndexInfo.peopleCount<<"题目个数为"<<oneIndexInfo.questionNum;
                oneIndexInfo.averageGrade=oneIndexInfo.gradeSum/(oneIndexInfo.peopleCount * oneIndexInfo.questionNum);
                qDebug()<<"均分为"<<oneIndexInfo.averageGrade;
            }
            else
            {
                qDebug()<<"人数为"<<oneIndexInfo.peopleCount<<"题目个数为"<<oneIndexInfo.questionNum;
                oneIndexInfo.averageGrade=-1;
                qDebug()<<"均分为"<<oneIndexInfo.averageGrade;
            }
            oneQustionnaireRes.push_back(oneIndexInfo);
            oneIndexInfo.gradeSum = 0;
            nZhibiao=oQueryFast.value(2).toInt();
            count=0;
            nQuestionNumber=oQueryFast.value(3).toInt();
            nOptionNumber=1;
         }
        if(oQueryFast.next());
        else
            break;
    }
    //最后一个原子指标的没有加入到计算结果中
    QPair<int,int> oneQuestionRes=QuestionnaireAnalysisCore(oQuerySlow,nOptionNumber);
    if(oneQuestionRes.first != invalidInput)
    {
        oneIndexInfo.gradeSum += oneQuestionRes.first;
        oneIndexInfo.peopleCount = oneQuestionRes.second;
    }
    else
    {
        oneIndexInfo.gradeSum = invalidInput;
        oneIndexInfo.peopleCount = 0;
    }
    count++;
    oneIndexInfo.questionNum=count;
    if(oneIndexInfo.peopleCount != 0 && oneIndexInfo.questionNum != 0)
        oneIndexInfo.averageGrade=oneIndexInfo.gradeSum/oneIndexInfo.peopleCount/oneIndexInfo.questionNum;
    else
        oneIndexInfo.averageGrade=-1;
    qDebug()<<"原子指标均分为"<<oneIndexInfo.averageGrade;
    oneQustionnaireRes.push_back(oneIndexInfo);

}
//计算并返回每道题目的总分数和人数
QPair<int,int> QuestionnaireAnalysisCore(QSqlQuery &oQuerySlow,int nOptionNumber)
{
    int nPeopleNumber=0;//参与某个题目调查问卷人数
    int nGrade=0;//每个题目的总分数
    //Score为各选项的百分制量化分值
    int nOptionScoreStandard[4][5]={{100,0,0,0,0},{100,50,0,0,0},{100,65,35,0,0},{100,75,50,25,0}};
    qDebug()<<"选项个数为"<<nOptionNumber;
    for(int i=0;i<nOptionNumber;++i)
    {
        oQuerySlow.next();
        QString strOptionAll=oQuerySlow.value(4).toString();
        QString strOption =strOptionAll.mid(0,1);
        qDebug()<<"选项为："<<strOption;
        //由于分数矩阵5*5，不足5个选项的补了0，因此不足5个选项的也不会越界
        if(QString::compare(strOption,"A") == 0)
        {
            nGrade += nOptionScoreStandard[nOptionNumber-2][0]*oQuerySlow.value(5).toInt();
            nPeopleNumber += oQuerySlow.value(5).toInt();
        }
        if(QString::compare(strOption,"B") == 0)
        {
            nGrade += nOptionScoreStandard[nOptionNumber-2][1]*oQuerySlow.value(5).toInt();
            nPeopleNumber += oQuerySlow.value(5).toInt();
        }
        if(QString::compare(strOption,"C") == 0)
        {
            nGrade += nOptionScoreStandard[nOptionNumber-2][2]*oQuerySlow.value(5).toInt();
            nPeopleNumber += oQuerySlow.value(5).toInt();
        }
        if(QString::compare(strOption,"D") == 0)
        {
            nGrade += nOptionScoreStandard[nOptionNumber-2][3]*oQuerySlow.value(5).toInt();
            nPeopleNumber += oQuerySlow.value(5).toInt();
        }
        if(QString::compare(strOption,"E") == 0)
        {
            nGrade += nOptionScoreStandard[nOptionNumber-2][4]*oQuerySlow.value(5).toInt();
            nPeopleNumber += oQuerySlow.value(5).toInt();
        }
    }
    if(nPeopleNumber != 0)
    {
        qDebug()<<"这道题目的总分数为"<<nGrade<<"这道题目人数"<<nPeopleNumber;
        QPair<int,int> res(nGrade,nPeopleNumber);
        return res;
    }
    else
    {
        QPair<int,int> res(invalidInput,0);
        return res;
    }
}
//把一套问卷的计算结果写入数据库文件中
void writeResultToDB(QVector<questionnaire> &oneQustionnaireRes,QString strTbableName)
{
    bool bWrite = true;
    QString sqlquery;

    QSqlQuery queryTmp(m_dbConnectPro);
    for(int i=0;i<oneQustionnaireRes.size();++i)
    {
        sqlquery = QString("UPDATE " + strTbableName + " SET result = %1 WHERE nodeid = %2 ").arg(oneQustionnaireRes[i].averageGrade).arg(oneQustionnaireRes[i].indexNum);
        bWrite = queryTmp.exec(sqlquery);
        if(bWrite == false)
            qDebug()<<"问卷结果存入数据库出错"<<queryTmp.lastError();
    }
}
//清除物理评估和信息流评估部分的答案统计
void clearLjResult()
{
    bool bWrite = true;
    QString sqlquery;

    QSqlQuery queryTmp(m_dbConnectPro);
    QString strTbableName="Lj_Result";
    int count=0;
    int col=0;

    QSqlQuery oQueryFast(m_dbConnectPro);
    QString strTmp = "select result_count,result_sid from "+strTbableName;
    oQueryFast.exec(strTmp);
    oQueryFast.next();
    while(oQueryFast.isValid())
    {
        if((oQueryFast.value(1).toInt() > 0 && oQueryFast.value(1).toInt()<=100)||\
                (oQueryFast.value(1).toInt() > 200 && oQueryFast.value(1).toInt()<=300))
        {
            col=oQueryFast.value(1).toInt();
            sqlquery = QString("UPDATE " + strTbableName + " SET result_count = %1 WHERE result_sid = %2 ").arg(count).arg(col);
            bWrite = queryTmp.exec(sqlquery);
            if(bWrite == false)
                qDebug()<<"清除物理和信息流问卷结果数据库出错"<<queryTmp.lastError();
        }
        oQueryFast.next();
    }
}

