#include "index/index.h"
#include <QDebug>
#include <QMessageBox>
#include "graphviz/internet_info_graphviz.h"
#include "QtXml"
using namespace std;
//QSqlDatabase Index::m_dbConnect=Index::DatabaseConnect();
extern QSqlDatabase m_dbConnect;

Index::Index(const QString &strTableName):m_unSnNum(2)
{
    m_strTableName=strTableName;
    DatabaseRead();
}
Index::~Index()
{}


void Index:: DatabaseDelet()
{
    QString strTmp;
    QSqlQuery queryTmp(m_dbConnect);
    strTmp = "delete * from " + m_strTableName;
    queryTmp.exec(strTmp);
}
void Index::DatabaseRead()
{
    m_EvaluationValue.clear();
    m_nodeIsAutomic.clear();
    m_nodeMark.clear();
    m_nodeParentId.clear();
    m_calculateResult.clear();
    QString strTmp;
    QSqlQuery queryTmp(m_dbConnect);
    strTmp = "select * from " + m_strTableName + " ORDER BY nodeid";
    qDebug() << strTmp;
    queryTmp.exec(strTmp);
    while(queryTmp.next())
    {
        if(queryTmp.value(1).toUInt()+1>m_unSnNum)
            m_unSnNum=queryTmp.value(1).toUInt()+1;
        //解析原子指标项
        if(queryTmp.value(5).toBool() == true)//数据库第6(=5+1)列,判断原子指标，
        {
            m_nodeIsAutomic.insert(queryTmp.value(1).toUInt(), true);//读取原子指标的nodeid,标记为true
            //m_nodeAutomic.insert(queryTmp.value(1).toUInt(), queryTmp.value(4).toString());//读取原子指标的nodeid和名称
        }
        else
        {
            m_nodeIsAutomic.insert(queryTmp.value(1).toUInt(), false);//读取原子指标的nodeid，标记为false
        }

        //解析所有指标项
        m_nodeMark.insert(queryTmp.value(1).toUInt(), queryTmp.value(4).toString());//读取指标的nodeid和名称
        m_nodeParentId.insert(queryTmp.value(1).toUInt(), queryTmp.value(2).toUInt());//读取指标的nodeid和ParentId
        m_calculateResult.insert(queryTmp.value(1).toInt(), queryTmp.value(6).toDouble());

        if(m_EvaluationValue.find(queryTmp.value(2).toUInt())==m_EvaluationValue.end())//遍历，如果之前m_Evaluation里面没有存储过该节点的父-子节点关系，则添加该存储
        {
            QMap<unsigned int, double> mapTmp;
            mapTmp.insert(queryTmp.value(1).toUInt(), queryTmp.value(3).toDouble());//读取nodeid和nodevalue
            m_EvaluationValue.insert(queryTmp.value(2).toUInt(), mapTmp);//插入parentid  nodeid和nodevalue
        }
        else m_EvaluationValue[queryTmp.value(2).toUInt()].insert(queryTmp.value(1).toUInt(), queryTmp.value(3).toDouble());//如果有存储过该父节点，则往该父节点的子节点序列里添加新的子节点数据
    }
}



void Index::AddSonNode(QTreeWidgetItem *Current_Item,uint Current_Id)
{
    if(m_EvaluationValue.find(Current_Id)==m_EvaluationValue.end())//找父节点为1的map
        return;
    for(auto i_temp=m_EvaluationValue[Current_Id].begin();i_temp!=m_EvaluationValue[Current_Id].end();i_temp++) //父节点为1的所有子节点
    {
        //qDebug() << m_nodeMark[i_temp.key()];//返回所找到的节点的description   //i_temp是个map类型，包含《nodeid,value》，i_temp.key()返回的是节点；   i_temp.value()返回的是节点对应的描述
        QTreeWidgetItem *son=new QTreeWidgetItem(Current_Item,QStringList()<<m_nodeMark[i_temp.key()]<<QString::number(i_temp.value()));
        Current_Item->addChild(son);
        AddSonNode(son,i_temp.key());//迭代过程
    }
}

void Index::RebuildEvaluationTree(QTreeWidget *tree)
{
    tree->clear();
    if(m_nodeMark.find(1)==m_nodeMark.end())
    {
        QTreeWidgetItem *root=new QTreeWidgetItem(tree,QStringList()<<" 当前指标体系内容为空！请删除本行，再添加数据 ");
        return;
    }
    else
    {
        QTreeWidgetItem *root=new QTreeWidgetItem(tree,QStringList()<<m_nodeMark[1]);  //根指标，因为跟指标是存在数据库里的第一条数据，所以其nodeid 为1，并且其没有权值
        AddSonNode(root,1);
    }
}

void Index::DatabaseWrite(unsigned int id)
{
    QString strWrite;
    QSqlQuery queryTmp(m_dbConnect);
    bool bWrite = true;
    strWrite = "INSERT INTO "
            + m_strTableName +
            " (nodeid,parentid,nodevalue,description,atomic,"
            "result) "
            + "VALUES(:nodeid,:parentid,:nodevalue,:description,:atomic,"
              ":result)";
    queryTmp.prepare(strWrite);
   // qDebug() << id << m_nodeParentId[id] << m_nodeMark[id];
    queryTmp.bindValue(":nodeid", id);
    queryTmp.bindValue(":parentid", m_nodeParentId[id]);
    queryTmp.bindValue(":nodevalue", m_EvaluationValue[m_nodeParentId[id]][id]);
    queryTmp.bindValue(":description", m_nodeMark[id]);

    if(m_nodeIsAutomic[id] == true)
    {
        queryTmp.bindValue(":atomic", 1);
    }
    else
    {
        queryTmp.bindValue(":atomic", 0);
    }
    queryTmp.bindValue(":result", m_calculateResult[id]);


    bWrite = queryTmp.exec();
    if(bWrite == false)
    {
        QMessageBox::information(NULL, QString("数据库"), QString("数据库写入失败"));
    }
}

void Index::_Get_TheSecurity(int i_temp)
{
    if(m_nodeIsAutomic[i_temp]) return;

    m_calculateResult[i_temp] = 0;
    QMap<unsigned int, double> current_son=m_EvaluationValue[i_temp];
    for(auto i=current_son.begin();i!=current_son.end();i++) // 遍历其所有子节点
    {
        _Get_TheSecurity(i.key()); //计算该子节点的安全值
        m_calculateResult[i_temp]+=(m_calculateResult[i.key()]*i.value()); //累计求和计算父节点的安全值
    }
    return;

}
void Index::Get_TheSecurity()
{
    _Get_TheSecurity(1);
    update_access_result(m_calculateResult);  //需要这一句更新到数据库吗？？？？已经计算放到内存里了，并显示

}
//// 指标累积计算结果更新到数据库的函数，7.13测试通过
//void Index::update_access_result(QMap<unsigned int , double> atom_result)
//{
//    QString sqlquery;
//    bool bWrite = true;
//    QMap<unsigned , double>::const_iterator point;

//    QSqlQuery queryTmp(m_dbConnect);
//   // qDebug()<<"当前表名为："<<m_strTableName<<"atom_result大小为："<<atom_result.size();
//    for(point = atom_result.constBegin(); point != atom_result.constEnd(); ++point)
//        {
//            sqlquery = QString("UPDATE " + m_strTableName + " SET result =%1 WHERE nodeid = %2").arg(point.value()).arg(point.key());
//            bWrite = queryTmp.exec(sqlquery);
//            //if(bWrite == false)
//            //qDebug()<<"QMap类型写入出错"<<queryTmp.lastError();
//        }
//}



//void Index::Display_Index()
//{
//    //qDebug()<<"now enter display index";
//    QHash< pair<uint,uint>, pair<QString,QString> > channel;
//    QMap<unsigned int, unsigned int>::const_iterator i_temp;

//    for(i_temp = m_nodeParentId.constBegin(); i_temp != m_nodeParentId.constEnd(); ++i_temp)
//    {
//        uint child_ID=i_temp.key();
//        uint parent_ID=i_temp.value();
//        if(channel.find(pair<uint,uint>(child_ID,parent_ID))!=channel.end())
//        {
//            channel[pair<uint,uint>(child_ID,parent_ID)].first+=QString::number(m_EvaluationValue[parent_ID][child_ID]);
//        }
//        else
//        {
//            pair<QString,QString> curr_message;
//            curr_message.first+=QString::number(m_EvaluationValue[parent_ID][child_ID]);
//            channel.insert(pair<uint,uint>(child_ID,parent_ID),curr_message);
//        }
//    }

//    QList<link_info> fin_channel;
//    for(auto i=channel.begin();i!=channel.end();i++)
//    {
//       link_info temp_link;
//       temp_link.link=i.key();
//       temp_link.link_message=i.value();
//       temp_link.style=make_pair<QString,uint>("black",1);
//       fin_channel.push_back(temp_link);
//    }

//    CInfoInternetVizObj *a=new CInfoInternetVizObj();
//    a->IndexGraphvizWithProperty(fin_channel,this);
//    delete a;

//}


////************************2017.12.27新加代码*****保存xml***************************//

//void Index::AssessRecord(QMap<uint,double> ID_calculateResult)
//{
//    QFile file("D:/Desktop/Desktop/12/12.xml");//path为生成文件的路径
//    if(!file.open(QFile::WriteOnly|QFile::Truncate))//Truncate表示清空原来的内容
//    {
//            qDebug()<<"XML文件创建不成功！！！";
//    }
//    QDomDocument doc;
//    //写入xml头部
//    QDomProcessingInstruction instruction; //添加处理命令
//    instruction=doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"");
//    doc.appendChild(instruction);
//    //添加根节点
//    QDomElement root=doc.createElement("服务器设备");
//    doc.appendChild(root);
//    //添加第一个子节点及其子元素
//    QDomElement book=doc.createElement("设备名称");
//    QDomAttr name=doc.createAttribute("name");
//    name.setValue("服务器A");
//    book.setAttributeNode(name);
//    QDomElement title=doc.createElement("score"); //创建子元素
//    QDomText text; //设置括号标签中间的值
//    for(QMap<uint,double>::Iterator iter = ID_calculateResult.begin();iter!= ID_calculateResult.end(); iter ++)
//    {
//        int id = iter.key();
//        double score = iter.value();
//        QString info = "("+QString::number(id)+","+QString::number(score)+")";
//        title=doc.createElement("score");
//        text=doc.createTextNode(info);
//        book.appendChild(title);
//        title.appendChild(text);
//        root.appendChild(book);
//    }




//    //输出到文件
//    QTextStream out_stream(&file);
//    doc.save(out_stream,4); //缩进4格
//    file.close();
//}

// 指标累积计算结果更新到数据库的函数，7.13测试通过
void Index::update_access_result(QMap<unsigned int , double> atom_result)
{
    QString sqlquery;
    bool bWrite = true;
    QMap<unsigned , double>::const_iterator point;

    QSqlQuery queryTmp(m_dbConnect);
   // qDebug()<<"当前表名为："<<m_strTableName<<"atom_result大小为："<<atom_result.size();
    for(point = atom_result.constBegin(); point != atom_result.constEnd(); ++point)
        {
            sqlquery = QString("UPDATE " + m_strTableName + " SET result =%1 WHERE nodeid = %2").arg(point.value()).arg(point.key());
            bWrite = queryTmp.exec(sqlquery);
            //if(bWrite == false)
            //qDebug()<<"QMap类型写入出错"<<queryTmp.lastError();
        }
}



void Index::Display_Index()
{
    //qDebug()<<"now enter display index";
    QHash< pair<uint,uint>, pair<QString,QString> > channel;
    QMap<unsigned int, unsigned int>::const_iterator i_temp;

    for(i_temp = m_nodeParentId.constBegin(); i_temp != m_nodeParentId.constEnd(); ++i_temp)
    {
        uint child_ID=i_temp.key();
        uint parent_ID=i_temp.value();
        if(channel.find(pair<uint,uint>(child_ID,parent_ID))!=channel.end())
        {
            channel[pair<uint,uint>(child_ID,parent_ID)].first+=QString::number(m_EvaluationValue[parent_ID][child_ID]);
        }
        else
        {
            pair<QString,QString> curr_message;
            curr_message.first+=QString::number(m_EvaluationValue[parent_ID][child_ID]);
            channel.insert(pair<uint,uint>(child_ID,parent_ID),curr_message);
        }
    }

    QList<link_info> fin_channel;
    for(auto i=channel.begin();i!=channel.end();i++)
    {
       link_info temp_link;
       temp_link.link=i.key();
       temp_link.link_message=i.value();
       temp_link.style=make_pair<QString,uint>("black",1);
       fin_channel.push_back(temp_link);
    }

    CInfoInternetVizObj *a=new CInfoInternetVizObj();
    a->IndexGraphvizWithProperty(fin_channel,this);
    delete a;

}


//************************2017.12.27新加代码*****保存xml***************************//

void Index::AssessRecord(QMap<QString,QMap<uint,double>> ID_calculateResult,QString path)
{
    QFile file(path);//path为生成文件的路径
    if(!file.open(QFile::WriteOnly|QFile::Truncate))//Truncate表示清空原来的内容
    {
            qDebug()<<"XML文件创建不成功！！！";
    }
    QDomDocument doc;
    //写入xml头部
    QDomProcessingInstruction instruction; //添加处理命令
    instruction=doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"");
    doc.appendChild(instruction);
    //添加根节点
    QDomElement root=doc.createElement("服务器设备");
    doc.appendChild(root);
    //添加第一个子节点及其子元素
    QDomElement equip=doc.createElement("设备名称");
    QDomAttr name=doc.createAttribute("name");
    equip.setAttributeNode(name);
    QDomElement title=doc.createElement("score"); //创建子元素
    QDomText text; //设置括号标签中间的值

    for(QMap<QString,QMap<uint,double>>::Iterator iter = ID_calculateResult.begin();iter!= ID_calculateResult.end(); iter ++)
    {
        equip=doc.createElement("设备名称");
        QString equipment_name = iter.key();
        name=doc.createAttribute("name");
        name.setValue(equipment_name);
        equip.setAttributeNode(name);
        QMap<uint,double> info = iter.value();

        for(QMap<uint,double>::Iterator i = info.begin();i!=info.end();i++)
        {
            int id = i.key();
            double score = i.value();
            QString assessinfo = "("+QString::number(id)+","+QString::number(score)+")";
            title=doc.createElement("score");
            text=doc.createTextNode(assessinfo);
            equip.appendChild(title);
            title.appendChild(text);
        }
        root.appendChild(equip);
    }



    //输出到文件
    QTextStream out_stream(&file);
    doc.save(out_stream,4); //缩进4格
    file.close();
}


//************************2017.12.27新加代码********读取xml********************//
QMap<QString, QMap<uint, double> > Index::Read_AssessRecord(QString path)
{
    //打开或创建文件
    QMap<uint,double> calculate;
    QMap<QString,QMap<uint,double>> ID_calculate;
    QFile file(path);//path为生成文件的路径
    if(!file.open(QFile::ReadOnly))
    {
        qDebug()<<"XML文件读取不成功！！！";
        return ID_calculate;
    }
    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        file.close();
        qDebug()<<"XML文件格式有问题！！！";
        return ID_calculate;
    }
    file.close();

    QDomElement root=doc.documentElement(); //返回根节点
    qDebug()<<"根节点名"<<root.nodeName();
    QDomNode node=root.firstChild(); //获得第一个子节点
    while(!node.isNull())  //如果节点不空
    {
        qDebug()<<"节点非空";

        if(node.isElement()) //如果节点是元素
        {
            QDomElement e=node.toElement(); //转换为元素，注意元素和节点是两个数据结构，其实差不多
            qDebug()<<"xml测试2"<<e.tagName()<<" "<<e.attribute("name"); //输出设备名称
            QDomNodeList list=e.childNodes();
            for(int i=0;i<list.count();i++) //遍历子元素，count和size都可以用,可用于标签数计数
            {
                QString csv;
                QDomNode n=list.at(i);
                if(node.isElement())
                    //qDebug()<<"xml测试3"<<n.nodeName()<<":"<<n.toElement().text();
                {
                    csv = n.toElement().text();
                    csv.replace(QString("("), QString(""));
                    csv.replace(QString(")"), QString(""));
                    calculate.insert( csv.section(',', 0, 0).toInt() , csv.section(',', 1, 1).toDouble() );
                }
            }
            ID_calculate.insert(e.attribute("name"),calculate);
        }
        node=node.nextSibling(); //下一个兄弟节点,nextSiblingElement()是下一个兄弟元素，都差不多
    }
    //qDebug()<<"测试恢复输出"<<ID_calculate;
    return ID_calculate;

}

