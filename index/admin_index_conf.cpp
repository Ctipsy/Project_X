#include "admin_index_conf.h"
#include "ui_admin_index_conf.h"
#include "index/index.h"
#include <QMessageBox>
#include <QDebug>
#include <QTimer>


admin_index_conf::admin_index_conf(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::admin_index_conf)
{
    ui->setupUi(this);
    ui->inputIndex->setPlaceholderText("请输入指标名称");
    //Index::DatabaseConnect();//一定要加啊  7.13更新去除
    setWindowTitle("指标体系编辑");
    ui->success_tip->setHidden(true);
    index_select=index_select_return();
    index_conf=index_select[0];
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setHeaderLabels(QStringList()<<"权值"<<"指标项");
    ui->treeWidget->setColumnWidth(0,180);
    index_conf->RebuildEvaluationTree(ui->treeWidget);
    ui->treeWidget->expandAll(); //结点全部展开
    ui->treeWidget->setHeaderLabel(tr("指标体系")); //设置头的标题

    ui->treeWidget_modify->setColumnCount(2);
    ui->treeWidget_modify->setHeaderLabels(QStringList()<<"权值"<<"指标项");
    ui->treeWidget_modify->setColumnWidth(0,100);
    //ui->treeWidget_modify->setA->setFlags(item->flags()|Qt::ItemIsEditable);
    svgWindow = new SvgWindow; //8.22添加

}

QList<Index*> index_select_return()
{
    QList<Index*> select;
    Index * a1=new Index("服务器设备指标树");
    select.push_back(a1);
    Index * a2=new Index("控制设备指标树");
    select.push_back(a2);
    Index * a3=new Index("显控设备指标树");
    select.push_back(a3);
    Index * a4=new Index("网络架构指标树");
    select.push_back(a4);
    Index * a5=new Index("信息流指标树");
    select.push_back(a5);
    return select;
}
admin_index_conf::~admin_index_conf()
{
    delete ui;
}

QTreeWidgetItem * admin_index_conf::AddTreeRoot(QString name, QString desc)
{
    QTreeWidgetItem * item = new QTreeWidgetItem(QStringList() << name << desc);
    ui->treeWidget->addTopLevelItem(item);
    return item;
}

QTreeWidgetItem * admin_index_conf::AddTreeNode(QTreeWidgetItem *parent, QString name, QString desc)
{
    QTreeWidgetItem * item = new QTreeWidgetItem(QStringList() << name << desc);
    parent->addChild(item);
    return item;
}
//添加根节点
void admin_index_conf::on_add_root_index_clicked()
{

    QMap<unsigned int, double> tmpValue;
    AddTreeRoot(ui->index_select->currentText(), "");
    index_conf->m_nodeParentId.insert(1, 0);            //填充父节点，节点id为1，父节点id为0
    index_conf->m_nodeMark.insert(1, ui->index_select->currentText());   //填充节点名
    tmpValue.insert(1, 0.0);  //节点id及节点值
    index_conf->m_EvaluationValue.insert(0, tmpValue);
    index_conf->m_nodeIsAutomic.insert(1, false);
    index_conf->m_calculateResult.insert(1, 0.0);
}

//添加叶子节点
void admin_index_conf::on_add_child_index_clicked()
{
    QTreeWidgetItem * item= ui->treeWidget->currentItem();
    //QMap<unsigned int, QString>::iterator my_map = m_nodeMark.end();
    unsigned int i;
    QList<unsigned int> my_find,find_SonNode;
    QMap<unsigned int, double> tmpValue;


    if(m_strEditAfter == "")
    {
        QMessageBox::information(NULL, QString("指标编辑"), QString("指标内容为空！"));
    }
    if(item != Q_NULLPTR)
    {
        AddTreeNode(item, m_strEditAfter, "");
        //插入节点编号和节点描述；
        index_conf->m_nodeMark.insert(index_conf->m_unSnNum, m_strEditAfter); //子节点序号从2开始，1被主根节点占用了
        //获取父节点的中文描述
        m_strFather = QString(item->text(0));
        my_find = index_conf->m_nodeMark.keys(m_strFather);//根据节点名称，找节点id
        index_conf->m_nodeParentId.insert(index_conf->m_unSnNum, my_find[0]);//因为节点名称是独一无二的，所以返回的节点id只有一个，故为find[0]

        //插入节点的权重值
        tmpValue.insert(index_conf->m_unSnNum, 0.0);//初始权值为0.0

        //先判断一下，当前节点是否出现过
        if(index_conf->m_EvaluationValue.find(my_find[0])==index_conf->m_EvaluationValue.end())
        {
            index_conf->m_EvaluationValue.insert(my_find[0],tmpValue);
            //index_conf.m_EvaluationValue[my_find[0]] = tmpValue;
        }
        index_conf->m_EvaluationValue[my_find[0]].insert(index_conf->m_unSnNum, 0.0);

        //修改父节点是否为原子指标
        if(index_conf->m_nodeIsAutomic[my_find[0]] == true)
        {
            index_conf->m_nodeIsAutomic[my_find[0]] = false;
        }
        index_conf->m_nodeIsAutomic.insert(index_conf->m_unSnNum,true);
        index_conf->m_calculateResult.insert(index_conf->m_unSnNum, -1);
        //index_conf->m_questionId.insert(index_conf->m_unSnNum, 1);
        //index_conf->m_questionDes.insert(index_conf->m_unSnNum, "index_number_judge()");
    }
    else
    {
         QMessageBox::information(NULL, QString("添加节点"), QString("当前未选择父节点！"));
    }
    (index_conf->m_unSnNum)++;
}



void admin_index_conf::on_modifyIndex_clicked()
{
//更改指标名称和权重
    QTreeWidgetItem * item= ui->treeWidget->currentItem();
    unsigned int i;
    QList<unsigned int> find_id;
    if(m_strEditAfter == ""||item == Q_NULLPTR)
    {
        QMessageBox::information(NULL, QString("名称更改"), QString("未选择指标或指标名称为空！"));
    }
    if(item != Q_NULLPTR&&m_strEditAfter!="")
    {
        find_id = index_conf->m_nodeMark.keys(QString(item->text(0)));//根据节点名称，找节点id
        if(!find_id.isEmpty())
        {
            //qDebug()<<find_id[0];
            index_conf->m_nodeMark[find_id[0]]=m_strEditAfter;
        }
    }
    //修改完毕后，立即刷新重新显示
    index_conf->RebuildEvaluationTree(ui->treeWidget);
    ui->treeWidget->expandAll(); //结点全部展开
}


void admin_index_conf::on_deletindex_clicked()
{
    QList<unsigned int> find_id,my_find;
    uint find_parentId;
    uint i,j;
    QTreeWidgetItem * currentItem = ui->treeWidget->currentItem();
    if(currentItem == Q_NULLPTR)
    {
        return;
    }
    else
    {
        find_id = index_conf->m_nodeMark.keys(QString(currentItem->text(0)));//根据节点名称，找节点id
        find_parentId = index_conf->m_nodeParentId[find_id[0]];//根据节点id找到父节点id
        //如果当前选中的节点没有子节点（即为原子指标）,就直接删除
        if(currentItem->child(0) == Q_NULLPTR && (!find_id.isEmpty()))
        {
            index_conf->m_nodeIsAutomic.remove(find_id[0]);
            index_conf->m_nodeMark.remove(find_id[0]);
            index_conf->m_nodeParentId.remove(find_id[0]);
            index_conf->m_calculateResult.remove(find_id[0]);
            index_conf->m_EvaluationValue[find_parentId].remove(find_id[0]);
        }
        //如果当前选中项不是主根节点，且它有子节点，那么需要遍历清空该节点与其子节点
        if(currentItem->child(0) != Q_NULLPTR && (!find_id.isEmpty())&&(find_parentId!=0) )
        {
            index_conf->m_nodeIsAutomic.remove(find_id[0]);
            index_conf->m_nodeMark.remove(find_id[0]);
            index_conf->m_nodeParentId.remove(find_id[0]);
            index_conf->m_calculateResult.remove(find_id[0]);
            index_conf->m_EvaluationValue[find_parentId].remove(find_id[0]);
            my_find = index_conf->m_nodeParentId.keys(find_id[0]);//根据节点id，找到其下属的所有子节点id
            for(i=0;i<my_find.size();i++)
            {
                j=my_find[i];
                index_conf->m_nodeIsAutomic.remove(j);
                index_conf->m_nodeMark.remove(j);
                index_conf->m_nodeParentId.remove(j);
                index_conf->m_calculateResult.remove(j);
            }
            index_conf->m_EvaluationValue.remove(find_id[0]);
            //如果有父节点就要用父节点的takeChild删除节点
            delete currentItem->parent()->takeChild(ui->treeWidget->currentIndex().row());
        }
        index_conf->RebuildEvaluationTree(ui->treeWidget);
        ui->treeWidget->expandAll(); //结点全部展开
    }
}


void admin_index_conf::on_inputIndex_textEdited(const QString &arg1)
{
    m_strEditAfter = arg1;
    //QMessageBox::information(NULL, QString("title"), arg1);
}



void admin_index_conf::on_index_select_currentIndexChanged(int index)
{
    index_conf=index_select[index];//下拉菜单变动时，直接传递当前的选项索引值index
    index_conf->RebuildEvaluationTree(ui->treeWidget);
    ui->treeWidget->expandAll(); //结点全部展开
}

//清空Access里面存储的指标体系，为新建指标挪出空间
void admin_index_conf::on_delete_access_clicked()
{
    QMessageBox msgBox(QMessageBox::Warning, tr("警告"),"该操作将清空当前选择的指标体系", 0, this);
    msgBox.addButton(tr("确定"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("取消"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::AcceptRole)
    {
        //index_conf->DatabaseDelet();
        //ui->treeWidget->clear();
        index_conf->m_EvaluationValue.clear();
        index_conf->m_nodeIsAutomic.clear();
        index_conf->m_nodeMark.clear();
        index_conf->m_nodeParentId.clear();
        index_conf->m_calculateResult.clear();
        on_add_root_index_clicked();
        index_conf->m_unSnNum=2;
        index_conf->RebuildEvaluationTree(ui->treeWidget);
    }
    else
    {

    }
}

//存储数据到Access表中
void admin_index_conf::on_index_write_access_clicked()
{
    index_conf->DatabaseDelet();
    for(auto i=index_conf->m_nodeIsAutomic.begin();i!=index_conf->m_nodeIsAutomic.end();i++)
    {
        index_conf->DatabaseWrite(i.key());
    }
}

//单击指标树中的某一项，在右边会显示出来，分原子指标和非原子指标两种进行处理
void admin_index_conf::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    QString name;
    QList<unsigned int> find_id;
    ui->node_show->setText(item->text(0));//显示当前点击的指标内容于右上方的方框里
    ui->treeWidget_modify->clear();//选择其他的指标选项后，先清空treeWidget

    find_id = index_conf->m_nodeMark.keys(QString(item->text(0)));//根据节点名称，找节点id
    for(auto i_map=index_conf->m_EvaluationValue[find_id[0]].begin();i_map!=index_conf->m_EvaluationValue[find_id[0]].end();i_map++)
    {
        QTreeWidgetItem * newitem = new QTreeWidgetItem(QStringList()<<QString::number(i_map.value())<<index_conf->m_nodeMark[i_map.key()] );
        ui->treeWidget_modify->addTopLevelItem(newitem);
    }
}

//节点权值改变时，及时更新权值数值，并重新显示左边的指标体系树
void admin_index_conf::on_treeWidget_modify_itemChanged(QTreeWidgetItem *item, int column)
{
    QList<uint> find_id;
    uint parent_id;
    double i=0,value_sum = 0;
    find_id = index_conf->m_nodeMark.keys(QString(item->text(1)));//根据名称找自己的节点
    parent_id = index_conf->m_nodeParentId[find_id[0]];
    //qDebug()<<"值改变，更新！"<<parent_id;
    //index_conf->m_EvaluationValue[index_conf->m_nodeParentId[find_id[0]]][find_id[0]]=(item->text(0)).toFloat();//把修改的权值信息更新赋给类的成员

}

/************************************************************************************************************/
//                                //设置为可编辑
/************************************************************************************************************/
void admin_index_conf::on_treeWidget_modify_itemClicked(QTreeWidgetItem *item, int column)
{
    if(column == 0)
    {
        item->setFlags(item->flags()|Qt::ItemIsEditable);//设置为可编辑
    }
}


/************************************************************************************************************/
//                                //  “应用”  按钮
/************************************************************************************************************/
void admin_index_conf::on_pushButton_clicked()
{
    QList<uint> find_id;
    uint parent_id;
    double i=0,sum_tmp=0;

    QTreeWidgetItemIterator it(ui->treeWidget_modify);   //定义指针，遍历QtreeWidget
    QTreeWidgetItemIterator it2(ui->treeWidget_modify);  //定义指针，遍历QtreeWidget

    while(*it)
    {
        find_id = index_conf->m_nodeMark.keys(QString((*it)->text(1)));
        parent_id = index_conf->m_nodeParentId[find_id[0]];
        sum_tmp += ((*it)->text(0)).toFloat();//把修改的权值信息更新赋给类的成员
        ++it;
    }
    if((sum_tmp-1)>0.000001||(1-sum_tmp)>0.000001)
    {
        QMessageBox msgBox(QMessageBox::Warning, tr("警告"),"当前指标体系权值和不为1 ！", 0, this);
        msgBox.addButton(tr("修改"), QMessageBox::AcceptRole);
        //msgBox.addButton(tr("放弃"), QMessageBox::RejectRole);
        if (msgBox.exec() == QMessageBox::AcceptRole)
        {
         return;
        }
    }
    else
    {
        ui->success_tip->setHidden(false);
        QTimer::singleShot(700,this, SLOT(show_modify_success()));  //显示 “修改成功”的标签 ，0.7秒后自动隐藏

        while(*it2)
        {
            find_id = index_conf->m_nodeMark.keys(QString((*it2)->text(1)));
            parent_id = index_conf->m_nodeParentId[find_id[0]];
            index_conf->m_EvaluationValue[index_conf->m_nodeParentId[find_id[0]]][find_id[0]]=((*it2)->text(0)).toFloat();//把修改的权值信息更新赋给类的成员
            ++it2;
        }
        index_conf->RebuildEvaluationTree(ui->treeWidget);
        ui->treeWidget->expandAll();
    }
}
void admin_index_conf::show_modify_success()
{
    ui->success_tip->setHidden(true);

}



void admin_index_conf::on_svg_view_clicked(bool checked)
{
    index_conf->Display_Index();

    svgWindow->setFile ("D:/Program Files (x86)/X_Ship/image/index.svg");    //8.22添加
    if(checked)
        svgWindow->show();
    else
        svgWindow->close();
}
