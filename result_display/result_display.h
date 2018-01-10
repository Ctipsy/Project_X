#ifndef RESULT_DISPLAY_H
#define RESULT_DISPLAY_H

#include <QtCharts>
#include <string>
#include <vector>
#include <QObject>
#include <QList>


#include <QString>

/*#include <QtCharts/QChartView>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>*/

QT_CHARTS_USE_NAMESPACE

using namespace  std;



class CResultDisplay   //should be the point member of Assessment Component
{
    //Q_OBJECT
public:
    CResultDisplay();
    //CResultDisplay(QList<QString> &keyName, QList<double> &keyValue);

    //CResultDisplay(vector<string> &keyName, vector<double> &keyValue, \
                   vector<string> vecStrGroupName = vector<string>());
    CResultDisplay(QList<QString> &keyName);
    //CResultDisplay(vector<double> &keyName, vector<double> &keyValue,\
                   vector<string> vecStrGroupName = vector<string>());
    CResultDisplay(QList<double> &keyName);
    //CResultDisplay(vector<string> &strKeyName, vector<double> &dKeyName, vector<double> &dKeyValue,\
                   vector<string> vecStrGroupName = vector<string>());

    CResultDisplay(QList<QString> &strKeyName, QList<double> &dKeyName);


    void BarDisplay(const QString &strTitle, QChartView *pChartView);
    void LineChartDisplay(const QString &strTitle, QChartView *pChartView);
    void PieChartDisplay(const QString &strTitle, QChartView *pChartView);
    void PolarChartDisplay(const QString &strTitle, QChartView *pChartView);
    //void ProgressBar();
    void HorizontalBarCharttDisplay(const QString &strTitle, QChartView *pChartView);


    void AddOneGroupData(QList<double> &oneGroupData, const QString &groupName);

private:
    QList<QString> m_vecStrKeyName;   //the X axis scale, maybe the meaning of each data, for string
    QList<double> m_vecNKeyName;     //the same as above,but for numeric type
    QList<QList<double> > m_vecDKeyValue;    //the result data to be displayed
    QList<QString> m_vecStrGroupName;  //the group name of several group datas to be compared
    //QString m_strTitle;

};

class CResultShow
{
public:
    void BarDisplay(QList<QString> &m_vecStrKeyName,
                    QList<double> &m_vecDKeyValue,
                    const QString &strTitle,
                    QChartView *pChartView);

    void BarDisplay(QList<QString> &m_vecStrKeyName,       //柱状图的横坐标，表示每一个圆柱代表的意思
                    QList<QList<double> > &m_vecDKeyValue,  //多组决定圆柱高度的数值，每一组与圆柱关键字的名字对应
                    QList<QString> &m_vecStrGroupName,         //每一组数据的名字
                    const QString &strTitle,                   //整个柱状图的名字
                    QChartView *pChartView);             //用于显示柱状图的视图

    void LineChartDisplay(QList<double> &m_vecNKeyName,
                          QList<double> &m_vecDKeyValue,
                          const QString &strTitle,
                          QChartView *pChartView);

    void LineChartDisplay(QList<double> &m_vecNKeyName,       //折线图的横坐标，表示每一个圆柱代表的意思
                          QList<QList<double> > &m_vecDKeyValue,  //多组决定折线图趋势的数值，每一组与折线图的横坐标对应
                          QList<QString> &m_vecStrGroupName,         //每一组数据的名字
                          const QString &strTitle,                   //整个折线图的名字
                          QChartView *pChartView);             //用于显示折线图的视图


    void PieChartDisplay(QList<QString> &m_vecStrKeyName,
                         QList<double> &m_vecDKeyValue,
                         const QString &strTitle,
                         QChartView *pChartView);

    void PolarChartDisplay(QList<QString> &m_vecStrKeyName,
                          QList<double> &m_vecDKeyValue,
                          QString &strTitle,
                          QChartView *pChartView);

    void PolarChartDisplay(QList<QString> &m_vecStrKeyName,       //雷达图的角坐标，表示每一个圆柱代表的意思
                          QList<QList<double> > &m_vecDKeyValue,  //多组决定雷达图径向的数值，每一组与角坐标的名字对应
                          QList<QString> &m_vecStrGroupName,         //每一组数据的名字
                          const QString &strTitle,                   //整个雷达图的名字
                          QChartView *pChartView);             //用于显示雷达图的视图

    void HorizontalBarCharttDisplay(double dVal, const QString &strTitle, QChartView *pChartView);
};

#endif // RESULT_DISPLAY_H
