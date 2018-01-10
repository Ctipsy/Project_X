#ifndef QUESTIONNAIRE_ANALYSIS_H
#define QUESTIONNAIRE_ANALYSIS_H

//问卷解析模块
#include <map>
#include <QVector>
#include <QSqlQuery>
#include <QStringList>

using namespace std;

struct questionnaire
{
    int indexNum;//原子指标号
    int gradeSum;//原子指标号对应的总分数
    int peopleCount;//原子指标对应的总人数
    int averageGrade;//此原子指标的分数
    int questionNum;//这个原子指标对应的题目数量
};

void QuestionnaireAnalysis();
void CalculateQuestionnaire(QSqlQuery &oQueryFast, QSqlQuery &oQuerySlow, int nQuestionnaireNumber, QVector<questionnaire> &oneQustionnaireRes);
QPair<int, int> QuestionnaireAnalysisCore(QSqlQuery &oQuerySlow, int nOptionNumber);
void writeResultToDB(QVector<questionnaire> &oneQustionnaireRes, QString strTbableName);
void clearLjResult();
void resetIndex();
#endif
