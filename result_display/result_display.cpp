#include "result_display.h"
#include <QProgressBar>

CResultDisplay::CResultDisplay()
{

}

/*
CResultDisplay::CResultDisplay(QList<QString> &keyName, QList<double> &keyValue)
{
    m_vecStrKeyName = keyName;
    m_vecDKeyValue = keyValue;
    //m_strTitle = title;
}*/
/*
CResultDisplay::CResultDisplay(vector<string> &keyName, vector<double> &keyValue,
                               vector<string> vecStrGroupName)
{
    m_vecStrKeyName = keyName;
    m_vecDKeyValue = keyValue;
    m_vecStrGroupName = vecStrGroupName;
}*/
CResultDisplay::CResultDisplay(QList<QString> &keyName)
{
    m_vecStrKeyName = keyName;
}

/*
CResultDisplay::CResultDisplay(vector<double> &keyName, vector<double> &keyValue,
                               vector<string> vecStrGroupName)
{
    m_vecNKeyName = keyName;
    m_vecDKeyValue = keyValue;
    m_vecStrGroupName = vecStrGroupName;
}*/
CResultDisplay::CResultDisplay(QList<double> &keyName)
{
    m_vecNKeyName = keyName;
}

/*
CResultDisplay::CResultDisplay(vector<string> &strKeyName, vector<double> &dKeyName, vector<double> &dKeyValue,
                               vector<string> vecStrGroupName)
{
    m_vecStrKeyName = strKeyName;
    m_vecNKeyName = dKeyName;
    m_vecDKeyValue = dKeyValue;
    m_vecStrGroupName = vecStrGroupName;
}*/

CResultDisplay::CResultDisplay(QList<QString> &strKeyName, QList<double> &dKeyName)
{
    m_vecStrKeyName = strKeyName;
    m_vecNKeyName = dKeyName;
}

void CResultDisplay::BarDisplay(const QString &strTitle, QChartView *pChartView)
{
    uint len = m_vecNKeyName.size();
    //if (len != m_vecDKeyValue.size())
     //   return;
    //QBarSet *set0 = new QBarSet("qingya");
    /*QBarSet *set1 = new QBarSet("John");
    QBarSet *set2 = new QBarSet("Axel");
    QBarSet *set3 = new QBarSet("Mary");
    QBarSet *set4 = new QBarSet("Samantha");*/
    vector<QBarSet *> vecBarSet;
    for (uint i = 0; i < m_vecDKeyValue.size(); ++i)
    {
        QBarSet *pTemp = new QBarSet(m_vecStrGroupName[i]);  //QString::fromLocal8Bit(m_vecStrKeyName[i].c_str())
        vecBarSet.push_back(pTemp);
        for (uint j = 0; j < m_vecDKeyValue[i].size(); ++j)
        {
            *(vecBarSet[i]) << m_vecDKeyValue[i][j];
        }
    }




    QBarSeries *series = new QBarSeries();

    for (uint i = 0; i < vecBarSet.size(); ++i)
    {
        series->append(vecBarSet[i]);
    }

    QChart *chart = new QChart();
    chart->setTheme(QChart::ChartThemeDark);
    chart->addSeries(series);
    chart->setTitle(strTitle);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    //categories << QObject::tr("Jan") << QObject::tr("Feb") << QObject::tr("Mar") << QObject::tr("Apr") << QObject::tr("May") << QObject::tr("Jun");
    //categories << ("Jan") << ("Feb") << ("Mar") << ("Apr") <<("May") << ("Jun");
    for (uint i = 0; i < m_vecStrKeyName.size(); ++i)
    {
        categories << m_vecStrKeyName[i];   //QString::fromLocal8Bit(s.c_str())
    }
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);

    //chart->createDefaultAxes();

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    //chart->legend()->hide();

    pChartView->setChart(chart);
    pChartView->setRenderHint(QPainter::Antialiasing);
}

void CResultDisplay::LineChartDisplay(const QString &strTitle, QChartView *pChartView)
{
    uint len = m_vecNKeyName.size();
    //if (len != m_vecDKeyValue.size())
    //    return;

    QChart *chart1 = new QChart();
    chart1->legend()->setVisible(true);
    chart1->legend()->setAlignment(Qt::AlignTop);

    for (uint i = 0; i < m_vecDKeyValue.size(); ++i)
    {
        QLineSeries *series = new QLineSeries();
        for (uint j = 0; j < m_vecDKeyValue[i].size(); ++j)
        {
            series->setName(m_vecStrGroupName[i]);
            series->append(m_vecNKeyName[j], m_vecDKeyValue[i][j]);
        }
        chart1->addSeries(series);
    }


    chart1->createDefaultAxes();
    //chart1->setTitle(QString::fromLocal8Bit(strTitle.c_str())); //fromLocal8Bit(strStd.c_str())
    chart1->setTitle(strTitle);

    pChartView->setChart(chart1);
    pChartView->setRenderHint(QPainter::Antialiasing);
}

void CResultDisplay::PieChartDisplay(const QString &strTitle, QChartView *pChartView)
{
    if (m_vecDKeyValue.size() == 0)
        return;

    QPieSeries *series = new QPieSeries();
    for (uint j = 0; j < m_vecDKeyValue[0].size(); ++j)
    {
        series->append(m_vecStrKeyName[j], m_vecDKeyValue[0][j]);
    }

    QPieSlice *slice = series->slices().at(0);
    slice->setLabel("35%");
    slice->setLabelVisible(true);
    slice->setPen(QPen(Qt::darkGreen, 2));
    slice->setBrush(Qt::green);


    QChart *chart = new QChart();
    chart->setTheme(QChart::ChartThemeDark);

    chart->addSeries(series);
    chart->setTitle(strTitle);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    pChartView->setChart(chart);
    pChartView->setRenderHint(QPainter::Antialiasing);
}

void CResultDisplay::PolarChartDisplay(const QString &strTitle, QChartView *pChartView)
{
    double nGap = 360.0 / m_vecStrKeyName.size();

    vector<QLineSeries *> vecSeries;
    for (uint i = 0; i < m_vecDKeyValue.size(); ++i)
    {
        QLineSeries *temp = new QLineSeries();
        temp->setName(m_vecStrGroupName[i]);
        vecSeries.push_back(temp);

        uint j = 0;
        for (; j < m_vecDKeyValue[i].size(); ++j)
        {
            vecSeries[i]->append(nGap * j, m_vecDKeyValue[i][j]);
        }
        vecSeries[i]->append(nGap * j, m_vecDKeyValue[i][0]);
    }

    QPolarChart *chart = new QPolarChart();
    chart->setTheme(QChart::ChartThemeDark);

    for (uint i = 0; i < m_vecDKeyValue.size(); ++i)
    {
        chart->addSeries(vecSeries[i]);
    }

     QCategoryAxis *axis = new QCategoryAxis();
     axis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
     for (uint i = 1; i < m_vecStrKeyName.size(); ++i)
     {
         axis->append(m_vecStrKeyName[i], nGap * i);
     }
     axis->append(m_vecStrKeyName[0], 360);
     chart->addAxis(axis, QPolarChart::PolarOrientationAngular);


     QValueAxis *radialAxis = new QValueAxis();
     radialAxis->setTickCount(6);
     radialAxis->setLabelFormat("%.1f");
     chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

     for (uint i = 0; i < m_vecDKeyValue.size(); ++i)
     {
         vecSeries[i]->attachAxis(radialAxis);
         vecSeries[i]->attachAxis(axis);
     }

     const qreal radialMin = 0;
     const qreal radialMax = 1.0;
     radialAxis->setRange(radialMin, radialMax);

     chart->setTitle(strTitle);
     pChartView->setChart(chart);
     pChartView->setRenderHint(QPainter::Antialiasing);
}

/*
void CResultDisplay::PolarChartDisplay(QString strTitle, QChartView *pChartView)
{
     QLineSeries *series4 = new QLineSeries();
     series4->setName("star inner");

     series4->append(0, 0.8);
     series4->append(60, 0.9);
     series4->append(120, 0.67);
     series4->append(180, 0.88);
     series4->append(240, 0.75);
     series4->append(300, 0.9);
     series4->append(360, 0.8);

    QPolarChart *chart = new QPolarChart();
          chart->addSeries(series4);

     QCategoryAxis *axis = new QCategoryAxis();
     axis->setTickCount(7);
     axis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
     axis->append("Feb", 60);
     axis->append("Mar", 120);
     axis->append("Apr", 180);
     axis->append("May", 240);
     axis->append("Jun", 300);
     axis->append("Jan", 360);
     chart->addAxis(axis, QPolarChart::PolarOrientationAngular);

     QValueAxis *radialAxis = new QValueAxis();
     radialAxis->setTickCount(6);
     radialAxis->setLabelFormat("%.1f");
     chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

     series4->attachAxis(radialAxis);
     series4->attachAxis(axis);

     const qreal radialMin = 0;
     const qreal radialMax = 1.0;
     radialAxis->setRange(radialMin, radialMax);

     pChartView->setChart(chart);
     pChartView->setRenderHint(QPainter::Antialiasing);
}*/

void CResultDisplay::HorizontalBarCharttDisplay(const QString &strTitle, QChartView *pChartView)
{
    QBarSet *set0 = new QBarSet("");
    double dValue = 0.6;
    *set0 << dValue;
    if (dValue >= 0.8)
        set0->setColor(QColor(0,255,0));
    else
        set0->setColor(QColor(255,0,0));


    /*QPainter painter(pChartView);//创建了一个QPainter对象实例
    painter.setRenderHint(QPainter::Antialiasing, true);//消除锯齿
    QLinearGradient linearGradient(20,20, 150, 150);  //创建了一个QLinearGradient对象实例，参数为起点和终点坐标
    linearGradient.setColorAt(0.2, Qt::white);
    linearGradient.setColorAt(0.4, Qt::blue);
    linearGradient.setColorAt(0.6, Qt::red);
    linearGradient.setColorAt(1.0,Qt::yellow);  //上面的四行分别设置渐变的颜色和路径比例
    painter.setBrush(QBrush(linearGradient));//将linearGradient对象传递给画刷
    painter.drawEllipse(10,10, 150, 150);//在相应的坐标画出来*/


    QHorizontalBarSeries *series = new QHorizontalBarSeries();
    series->append(set0);

    QChart *chart = new QChart();
    chart->setTheme(QChart::ChartThemeDark);

    chart->addSeries(series);

    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(0, 1.0);
    QValueAxis *axisY = new QValueAxis();
    //axisY->setTickCount(2);
    axisY->hide();
    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);

    axisX->applyNiceNumbers();

    chart->legend()->hide();

    pChartView->setChart(chart);
    pChartView->setRenderHint(QPainter::Antialiasing);
}




void CResultDisplay::AddOneGroupData(QList<double> &oneGroupData, const QString &groupName)
{
    m_vecDKeyValue.push_back(oneGroupData);
    m_vecStrGroupName.push_back(groupName);
}

//***************************************************************************************

void CResultShow::BarDisplay(QList<QString> &m_vecStrKeyName,
                             QList<double> &m_vecDKeyValue,
                             const QString &strTitle,
                             QChartView *pChartView)
{
    QBarSet *pTemp = new QBarSet("");
    for (uint i = 0; i < m_vecDKeyValue.size(); ++i)
    {
         *pTemp << m_vecDKeyValue[i];
    }

    QBarSeries *series = new QBarSeries();
    series->append(pTemp);


    QChart *chart = new QChart();
    chart->setTheme(QChart::ChartThemeDark);

    chart->addSeries(series);
    chart->setTitle(strTitle);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;

    for (uint i = 0; i < m_vecStrKeyName.size(); ++i)
    {
        categories << m_vecStrKeyName[i];   //QString::fromLocal8Bit(s.c_str())
    }
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);

    //chart->createDefaultAxes();

    //chart->legend()->setVisible(true);
    //chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->hide();

    pChartView->setChart(chart);
    pChartView->setRenderHint(QPainter::Antialiasing);
}

void CResultShow::BarDisplay(QList<QString> &m_vecStrKeyName,       //柱状图的横坐标，表示每一个圆柱代表的意思
                QList<QList<double> > &m_vecDKeyValue,  //多组决定圆柱高度的数值，每一组与圆柱关键字的名字对应
                QList<QString> &m_vecStrGroupName,         //每一组数据的名字
                const QString &strTitle,                   //整个柱状图的名字
                QChartView *pChartView)             //用于显示柱状图的视图
{
    uint len = m_vecStrKeyName.size();

    vector<QBarSet *> vecBarSet;
    for (uint i = 0; i < m_vecDKeyValue.size(); ++i)
    {
        QBarSet *pTemp = new QBarSet(m_vecStrGroupName[i]);  //QString::fromLocal8Bit(m_vecStrKeyName[i].c_str())
        vecBarSet.push_back(pTemp);
        for (uint j = 0; j < m_vecDKeyValue[i].size(); ++j)
        {
            *(vecBarSet[i]) << m_vecDKeyValue[i][j];
        }
    }

    QBarSeries *series = new QBarSeries();

    for (uint i = 0; i < vecBarSet.size(); ++i)
    {
        series->append(vecBarSet[i]);
    }

    QChart *chart = new QChart();
    chart->setTheme(QChart::ChartThemeDark);

    chart->addSeries(series);
    chart->setTitle(strTitle);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;

    for (uint i = 0; i < m_vecStrKeyName.size(); ++i)
    {
        categories << m_vecStrKeyName[i];   //QString::fromLocal8Bit(s.c_str())
    }
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);

    //chart->createDefaultAxes();

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    //chart->legend()->hide();

    pChartView->setChart(chart);
    pChartView->setRenderHint(QPainter::Antialiasing);
}

void CResultShow::LineChartDisplay(QList<double> &m_vecNKeyName,
                      QList<double> &m_vecDKeyValue,
                      const QString &strTitle,
                      QChartView *pChartView)
{
    uint len = m_vecNKeyName.size();

    QChart *chart1 = new QChart();
    chart1->legend()->setVisible(true);
    chart1->legend()->setAlignment(Qt::AlignTop);

    QLineSeries *series = new QLineSeries();
    for (uint i = 0; i < m_vecDKeyValue.size(); ++i)
    {
         series->append(m_vecNKeyName[i], m_vecDKeyValue[i]);
    }
    chart1->addSeries(series);


    chart1->createDefaultAxes();
    //chart1->setTitle(QString::fromLocal8Bit(strTitle.c_str())); //fromLocal8Bit(strStd.c_str())
    chart1->setTitle(strTitle);

    pChartView->setChart(chart1);
    pChartView->setRenderHint(QPainter::Antialiasing);
}

void CResultShow::LineChartDisplay(QList<double> &m_vecNKeyName,       //折线图的横坐标，表示每一个点代表的意思
                      QList<QList<double> > &m_vecDKeyValue,  //多组决定折线图趋势的数值，每一组与折线图的横坐标对应
                      QList<QString> &m_vecStrGroupName,         //每一组数据的名字
                      const QString &strTitle,                   //整个折线图的名字
                      QChartView *pChartView)             //用于显示折线图的视图
{
    uint len = m_vecNKeyName.size();
    //if (len != m_vecDKeyValue.size())
    //    return;

    QChart *chart1 = new QChart();
    chart1->legend()->setVisible(true);
    chart1->legend()->setAlignment(Qt::AlignTop);

    for (uint i = 0; i < m_vecDKeyValue.size(); ++i)
    {
        QLineSeries *series = new QLineSeries();
        for (uint j = 0; j < m_vecDKeyValue[i].size(); ++j)
        {
            series->setName(m_vecStrGroupName[i]);
            series->append(m_vecNKeyName[j], m_vecDKeyValue[i][j]);
        }
        chart1->addSeries(series);
    }


    chart1->createDefaultAxes();
    //chart1->setTitle(QString::fromLocal8Bit(strTitle.c_str())); //fromLocal8Bit(strStd.c_str())
    chart1->setTitle(strTitle);

    pChartView->setChart(chart1);
    pChartView->setRenderHint(QPainter::Antialiasing);
}


void CResultShow::PieChartDisplay(QList<QString> &m_vecStrKeyName,
                     QList<double> &m_vecDKeyValue,
                     const QString &strTitle,
                     QChartView *pChartView)
{
    if (m_vecDKeyValue.size() == 0)
        return;

    QPieSeries *series = new QPieSeries();
    for (uint j = 0; j < m_vecStrKeyName.size(); ++j)
    {
        series->append(m_vecStrKeyName[j], m_vecDKeyValue[j]);
    }

    for (int i = 0; i < m_vecStrKeyName.size(); ++i)
    {
        QPieSlice *slice = series->slices().at(i);
        slice->setLabel(slice->label() + ":" + QString::number((slice->percentage()) * 100) + "%");
        slice->setLabelVisible(true);
    }

    //slice->setPen(QPen(Qt::darkGreen, 2));
    //slice->setBrush(Qt::green);


    QChart *chart = new QChart();
    chart->setTheme(QChart::ChartThemeDark);

    chart->addSeries(series);
    chart->setTitle(strTitle);

    //chart->legend()->setVisible(true);
    chart->legend()->hide();
    chart->legend()->setAlignment(Qt::AlignRight);

    pChartView->setChart(chart);
    pChartView->setRenderHint(QPainter::Antialiasing);
}

void CResultShow::PolarChartDisplay(QList<QString> &m_vecStrKeyName,
                      QList<double> &m_vecDKeyValue,
                      QString &strTitle,
                      QChartView *pChartView)
{
    double nGap = 360.0 / m_vecStrKeyName.size();

    //vector<QLineSeries *> vecSeries;
    QLineSeries *temp = new QLineSeries();
    //temp->setName(m_vecStrGroupName[i]);
    uint i = 0;
    for (; i < m_vecDKeyValue.size(); ++i)
    {
        temp->append(nGap * i, m_vecDKeyValue[i]);
    }
    temp->append(nGap * i, m_vecDKeyValue[0]);

    QPolarChart *chart = new QPolarChart();
    chart->setTheme(QChart::ChartThemeDark);

    chart->addSeries(temp);

     QCategoryAxis *axis = new QCategoryAxis();
     axis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
     for (uint i = 1; i < m_vecStrKeyName.size(); ++i)
     {
         axis->append(m_vecStrKeyName[i], nGap * i);
     }
     axis->append(m_vecStrKeyName[0], 360);
     chart->addAxis(axis, QPolarChart::PolarOrientationAngular);


     QValueAxis *radialAxis = new QValueAxis();
     radialAxis->setTickCount(6);
     radialAxis->setLabelFormat("%.1f");
     chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

     temp->attachAxis(radialAxis);
     temp->attachAxis(axis);


     const qreal radialMin = 0;
     const qreal radialMax = 100.0;  //修改雷达图最大值
     radialAxis->setRange(radialMin, radialMax);

     chart->setTitle(strTitle);
     pChartView->setChart(chart);

     pChartView->setRenderHint(QPainter::Antialiasing);
}

void CResultShow::PolarChartDisplay(QList<QString> &m_vecStrKeyName,       //雷达图的角坐标，表示每一个圆柱代表的意思
                      QList<QList<double> > &m_vecDKeyValue,  //多组决定雷达图径向的数值，每一组与角坐标的名字对应
                      QList<QString> &m_vecStrGroupName,         //每一组数据的名字
                      const QString &strTitle,                   //整个雷达图的名字
                      QChartView *pChartView)             //用于显示雷达图的视图
{
    double nGap = 360.0 / m_vecStrKeyName.size();

    vector<QLineSeries *> vecSeries;
    for (uint i = 0; i < m_vecDKeyValue.size(); ++i)
    {
        QLineSeries *temp = new QLineSeries();
        temp->setName(m_vecStrGroupName[i]);
        vecSeries.push_back(temp);

        uint j = 0;
        for (; j < m_vecDKeyValue[i].size(); ++j)
        {
            vecSeries[i]->append(nGap * j, m_vecDKeyValue[i][j]);
        }
        vecSeries[i]->append(nGap * j, m_vecDKeyValue[i][0]);
    }

    QPolarChart *chart = new QPolarChart();
    chart->setTheme(QChart::ChartThemeDark);

    for (uint i = 0; i < m_vecDKeyValue.size(); ++i)
    {
        chart->addSeries(vecSeries[i]);
    }

     QCategoryAxis *axis = new QCategoryAxis();
     axis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
     for (uint i = 1; i < m_vecStrKeyName.size(); ++i)
     {
         axis->append(m_vecStrKeyName[i], nGap * i);
     }
     axis->append(m_vecStrKeyName[0], 360);
     chart->addAxis(axis, QPolarChart::PolarOrientationAngular);


     QValueAxis *radialAxis = new QValueAxis();
     radialAxis->setTickCount(6);
     radialAxis->setLabelFormat("%.1f");
     chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

     for (uint i = 0; i < m_vecDKeyValue.size(); ++i)
     {
         vecSeries[i]->attachAxis(radialAxis);
         vecSeries[i]->attachAxis(axis);
     }

     const qreal radialMin = 0;
     const qreal radialMax = 1.0;
     radialAxis->setRange(radialMin, radialMax);

     chart->setTitle(strTitle);
     pChartView->setChart(chart);
     pChartView->setRenderHint(QPainter::Antialiasing);
}

void CResultShow::HorizontalBarCharttDisplay(double dVal, const QString &strTitle, QChartView *pChartView)
{
    QBarSet *set0 = new QBarSet("");
    //double dValue = 0.6;
    *set0 << dVal;
    if (dVal >= 0.8)
        set0->setColor(QColor(0,255,0));
    else
        set0->setColor(QColor(255,0,0));

    QHorizontalBarSeries *series = new QHorizontalBarSeries();
    series->append(set0);

    QChart *chart = new QChart();
    chart->setTheme(QChart::ChartThemeDark);

    chart->addSeries(series);

    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(0, 1.0);
    QValueAxis *axisY = new QValueAxis();
    //axisY->setTickCount(2);
    axisY->hide();
    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);

    axisX->applyNiceNumbers();

    chart->legend()->hide();

    pChartView->setChart(chart);
    pChartView->setRenderHint(QPainter::Antialiasing);
}

