#ifndef INDEX_H
#define INDEX_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QString>
#include <QtSql>
using namespace std;

class Index
{
public:
    Index(const QString &strTableName);
    ~Index();
    void DatabaseRead();
    void DatabaseDelet();
    void DatabaseWrite(uint id);
    void GetAutomicIndex();
    void AddSonNode(QTreeWidgetItem *Current_Item,uint Current_Id);
    void RebuildEvaluationTree(QTreeWidget *tree);
    void Get_TheSecurity();
    void update_access_result(QMap<unsigned int , double> atom_result);
    void Display_Index();
    //************************2017.12.27新加代码***************************************//

    void AssessRecord(QMap<QString, QMap<uint, double> > ID_calculateResult,QString path);
    QMap<QString, QMap<uint,double>> Read_AssessRecord(QString path);
    //************************2017.12.27新加代码***************************************//


    //和指标体系的交互
    //QList<unsigned int> GetLayerNodeId(unsigned int unLayerNum);
    //QMap<unsigned int, float> GetLayerResult(unsigned int unLayerNum = 0);

    //指标体系和算法的交互
    //QMap<unsigned int, float> AHPGroupCalculate(QMap<unsigned int, float> nodeValue,QMap<unsigned int, unsigned int> parentId);
public:
    QMap<unsigned int, QString> m_nodeMark;         //指标描述,  <节点id,节点名称>
    QMap<unsigned int, double> m_calculateResult;    //全局的计算结果
    QMap<QString,QMap<uint,double>> m_ID_calculateResult; //ID（xml导入的，有设备ID 任务ID  信息ID） nodeid（一个设备有很多特性） result;
    QMap<unsigned int, unsigned int> m_nodeParentId;//节点的父节点,<子节点id,父节点id>
    QMap<unsigned int, bool> m_nodeIsAutomic;           //指标是否是原子指标



private:
    void _Get_TheSecurity(int i_temp);
    //添加数据库的相关操作
    //static QSqlDatabase m_dbConnect;
    QSqlQuery     m_dbQuery;
    QSqlRecord   m_dbRecord;
    QString  m_strTableName;
    //uint a;

    //指标体系数据结构形式
    QMap<unsigned int, QMap<unsigned int, double> > m_EvaluationValue;//父子节点关系，前面是父节点，后面是子节点及子节点的value
    //QMap<unsigned int, QString> m_nodeMark;         //指标描述,  <节点id,节点名称>

    uint m_unSnNum;


    //声明为index的友元类，这样wuli.cpp里面的函数就可以访问index的private成员了
    friend class wuli;
    friend class messageassess;
    friend class admin_index_conf;
    friend class internetfunction;


};
#endif // INDEX_H
