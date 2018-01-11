#include "admin_config.h"
#include "ui_admin_config.h"
#include <QMessageBox>
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QSqlDatabase>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QSqlError>
#include <QTableWidget>
#include <QListWidgetItem>
#include <QButtonGroup>
#include <QDebug>
#include <map>
#include <QImage>
#include <QDesktopServices>
#include <QMenu>



extern QSqlDatabase m_dbConnect;//全局变量打开数据库文件
extern QMap<QString,int> indexNameNotFound;

admin_config::admin_config(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::admin_config)
{
    ui->setupUi(this);
    index_conf=new admin_index_conf(this);
    this->setWindowTitle("后台管理系统");
    account = new QTableWidget(this);
    account->setObjectName("account");
    account->setVisible(0);
    //设置分数较低的设备名
    account->setColumnCount(8);
    account->verticalHeader()->setVisible(false); //隐藏行序号
    //account->horizontalHeader()->setVisible(false); //隐藏列序号
    account->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    account->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空
    account->setSelectionBehavior(QAbstractItemView::SelectRows);
    QStringList header;
    header<<"用户ID"<<"用户名"<<"密码"<<"性别"<<"创建时间"<<"所属部门"<<"密保问题"<<"密保问题答案";
    account->setHorizontalHeaderLabels(header);
    account->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    account->setContextMenuPolicy(Qt::CustomContextMenu);
    connect (account, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customMenuRequested(QPoint)));   // right click connection
}




admin_config::~admin_config()
{
    delete ui;
}

/************************************************************************************************************/
//                              获取光标位置，显示右键菜单弹窗
/************************************************************************************************************/
void admin_config::customMenuRequested(QPoint pos)
{
    QTableWidgetItem* selecteditem = account->itemAt(pos) ; //get right click pos item
    QMenu* menu = new QMenu(this);
    menu->setStyleSheet("QMenu{background: white; color: black}");
    QAction* action = new QAction("删除账户",this);
    menu->addAction(action);
    menu->popup(account->viewport()->mapToGlobal(pos)) ;
    connect (action, SIGNAL(triggered()), this, SLOT(rightMenu()));
}



/************************************************************************************************************/
//                              右键菜单功能，删除账户
/************************************************************************************************************/
void admin_config::rightMenu()
{
    bool bWrite;
    bool focus = account->isItemSelected(account->currentItem()); // 判断是否选中一行
    int row = account->currentItem()->row(); // 当前选中行
    QString a = account->item(row,0)->text();
    QString sqlStr ="delete * from regInfo where userID='" + a + "'";
    //qDebug()<<account->item(row,0)->text();// 当前选中行的第一列的内容，即userID
    QSqlQuery queryTmp(m_dbConnect);
    QMessageBox megBox;
    megBox.addButton("是",QMessageBox::AcceptRole);
    megBox.addButton("否",QMessageBox::RejectRole);
    megBox.setText("确认删除该账户？");
    int ret= megBox.exec();
    if(ret == QMessageBox::AcceptRole)
    {
       bWrite = queryTmp.exec(sqlStr);
       if(bWrite == false)
       {
           QMessageBox::information(NULL, QString("数据库"), QString("账户删除出错！"));
           //qDebug()<<queryTmp.lastError();
       }
       else
           on_account_manage_clicked();
    }
}

/************************************************************************************************************/
//                              点击问卷编辑按钮
/************************************************************************************************************/
void admin_config::on_questionnaire_config_clicked()
{

    QString strWorkingDir="D:\\Program Files (x86)\\X_Ship";
    QDir::setCurrent(strWorkingDir);
    QString strQuestionnaireProgramPath=".\\QuestionnaireSystemAdmin.exe";
    QStringList strQuestionnaireCallArguments;
    strQuestionnaireCallArguments<<".\\WeifenLuo.WinFormsUI.Docking.dll";
    QProcess *myProcess=new QProcess(this);
    myProcess->start(strQuestionnaireProgramPath, strQuestionnaireCallArguments);
    myProcess->waitForFinished(-1);
    QString show="";
    //把没有对应上原子指标的指标树名称和原子指标名称显示在界面上
    for(QMap<QString,int>::iterator iter=indexNameNotFound.begin();iter!=indexNameNotFound.end();iter++)
    {
        show += iter.key()+"\n";
    }
    if(""==show)//所有原子指标都有题目
    {

    }
    else
    {
        QString s1="以下原子指标没有对应上题目:";
        s1=s1+"\n";
        QString s2="请以管理员身份登陆，针对未对应原子指标的问卷增加问卷题目！";
        QMessageBox::information(NULL, QString("错误"), s1+show+s2);
    }
}



/************************************************************************************************************/
//                             账户管理功能，读取access并显示到表格里
/************************************************************************************************************/
void admin_config::on_account_manage_clicked()
{
    ui->textBrowser->close();
    QStringList header;
    header<<"用户ID"<<"用户名"<<"密码"<<"性别"<<"创建时间"<<"所属部门"<<"密保问题"<<"密保问题答案";
    //ui->horizontalLayout_2->removeItem(ui->textBrowser);
    account->setVisible(1);
    ui->horizontalLayout_2->addWidget(account);
    account->setRowCount(0);   //相当于重设表格，   用clear清空后，表格还在，只是内容没有
    account->setColumnCount(8);
    account->setHorizontalHeaderLabels(header);
    QSqlQuery *search = new QSqlQuery;
    int currentRow=account->rowCount();
    QString sqlStr = "select userID, userName, passWord, sex, creatTime, department, safeQuestion, safeAnswer from regInfo";
    search->prepare( sqlStr );
    if( search->exec() )
    {
       while( search->next() )
       {
           account->insertRow(currentRow);
           account->setItem( currentRow, 0, new QTableWidgetItem( search->value(0).toString()));
           account->setItem( currentRow, 1, new QTableWidgetItem( search->value(1).toString()));
           account->setItem( currentRow, 2, new QTableWidgetItem( search->value(2).toString()));
           account->setItem( currentRow, 3, new QTableWidgetItem( search->value(3).toString()));
           account->setItem( currentRow, 4, new QTableWidgetItem( search->value(4).toString()));
           account->setItem( currentRow, 5, new QTableWidgetItem( search->value(5).toString()));
           account->setItem( currentRow, 6, new QTableWidgetItem( search->value(6).toString()));
           account->setItem( currentRow, 7, new QTableWidgetItem( search->value(7).toString()));
           for(auto i=0;i<8;i++)
                account->item(currentRow,i)->setTextAlignment(Qt::AlignCenter); //设置文本内容居中
           currentRow++;
       }
    }
}


/************************************************************************************************************/
//                              指标编辑按钮
/************************************************************************************************************/
void admin_config::on_index_config_clicked()
{
    index_conf->show();
    creat_index_txt();
}



/************************************************************************************************************/
//                              遍历数据库，查询所有原子指标，并对应建立txt，如果已经有，则跳过
/************************************************************************************************************/
void admin_config::creat_index_txt()
{


    QSqlQuery *search = new QSqlQuery;
    QList<QString> index_name[5];
    QString sqlStr[5];
    QString path[3];
    path[0]="D:\\Program Files (x86)\\X_Ship\\DB_51aspx\\wuli_assess_KnowledgeBase\\";
    path[1]="D:\\Program Files (x86)\\X_Ship\\DB_51aspx\\message_assess_KnowledgeBase\\";
    path[2]="D:\\Program Files (x86)\\X_Ship\\DB_51aspx\\jiagou_assess_KnowledgeBase\\";

     sqlStr[0] = "select description, atomic from 服务器设备指标树";
     sqlStr[1] = "select description, atomic from 控制设备指标树";
     sqlStr[2] = "select description, atomic from 显控设备指标树";
     sqlStr[3] = "select description, atomic from 信息流指标树";
     sqlStr[4] = "select description, atomic from 网络架构指标树";

    for(auto i=0;i<5;i++)
    {
        search->prepare(QString(sqlStr[i]));
        if( search->exec() )
        {

            while( search->next() )
           {
               if(search->value(1).toString()=="1")
                    index_name[i].append(search->value(0).toString());

           }
        }


        for(QList<QString>::iterator iter=index_name[i].begin();iter!=index_name[i].end();iter++)
        {
            if(i<3)
            {
                QFile file(QString(path[0]+*iter+".txt"));
                if ( file.exists())   continue;
                file.open( QIODevice::ReadWrite | QIODevice::Text );
                file.close();
            }
            else
            {
                QFile file(QString(path[i-2]+*iter+".txt"));
                if ( file.exists())   continue;
                file.open( QIODevice::ReadWrite | QIODevice::Text );
                file.close();
            }

        }

    }

}




/************************************************************************************************************/
//                              编辑物理评估知识库，打开对应的知识库文件夹路径
/************************************************************************************************************/
void admin_config::on_knowledgebase_clicked()
{
    //直接打开目录，手动删改
   // QDesktopServices::openUrl(QUrl("file:///C:/719/2017_06_10_Total/DB_51aspx/wuli_assess_KnowledgeBase", QUrl::TolerantMode));
    QStringList header;
    QString path;
    header<<"文件名"<<"文件大小"<<"双击单元格打开对应文件";
    account->setHorizontalHeaderLabels(header);    int currentRow=0;
    ui->textBrowser->close();
    account->setVisible(1);
    ui->horizontalLayout_2->addWidget(account);
    account->setRowCount(0);   //相当于重设表格，   用clear清空后，表格还在，只是内容没有
    account->setColumnCount(3);
    switch(Selcet_Knowledeg_Base())
    {
        case 1: path = "D:/Program Files (x86)/X_Ship/DB_51aspx/wuli_assess_KnowledgeBase";  break;
        case 2: path = "D:/Program Files (x86)/X_Ship/DB_51aspx/jiagou_assess_KnowledgeBase";  break;
        case 3: path = "D:/Program Files (x86)/X_Ship/DB_51aspx/message_assess_KnowledgeBase";  break;
        default: break;
    }
    QDir *dir = new QDir(path);
    QStringList filter;
    filter<<"*.txt";
    dir->setNameFilters(filter);
    QList<QFileInfo> *fileInfo = new QList<QFileInfo>(dir->entryInfoList(filter));
    int num = fileInfo->count();//文件数目
    for(auto i=0;i<num;i++)
    {
        QPushButton *pBtn = new QPushButton();
        pBtn->setText("编辑");
        account->insertRow(currentRow);
        account->setItem( currentRow, 0, new QTableWidgetItem(fileInfo->at(i).fileName())  );//文件名
        account->setItem( currentRow, 2, new QTableWidgetItem(fileInfo->at(i).filePath())  );//文件路径
        if(fileInfo->at(i).size()==0)
            account->setItem( currentRow, 1, new QTableWidgetItem("知识库内容为空！请编辑添加"));//文件路径
        else
            account->setItem( currentRow, 1, new QTableWidgetItem(QString::number(fileInfo->at(i).size())+"  Byte"    )  );//文件路径
        for(auto i=0;i<3;i++)
            account->item(currentRow,i)->setTextAlignment(Qt::AlignCenter); //设置文本内容居中
        currentRow++;
    }
    connect(account, SIGNAL(cellDoubleClicked(int,int)),this,SLOT(editTxt(int,int)));

}


/************************************************************************************************************/
//                             账户管理功能，读取access并显示到表格里
/************************************************************************************************************/
int admin_config::Selcet_Knowledeg_Base()
{
    int flag = 1;
    QMessageBox msgBox(QMessageBox::Information, tr("知识库"),"请选择要管理的知识库", 0, this);
    msgBox.resize(200, 50);
    msgBox.addButton(tr("物理设备"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("架构安全"), QMessageBox::RejectRole);
    msgBox.addButton(tr("信息流"),QMessageBox::ActionRole);
    auto m = msgBox.exec();
    if (m == QMessageBox::AcceptRole)
        flag = 1;
    else if(m ==QMessageBox::RejectRole)
       flag = 2;
    else
        flag = 3;
    return flag;
}

/************************************************************************************************************/
//                                      双击知识库路径后，打开对应的txt文件
/************************************************************************************************************/
void admin_config::editTxt(int x,int y)
{
    //直接打开文件夹
    //QProcess::execute("explorer D:\\719\\2017_06_10_Total\\DB_51aspx\\wuli_assess_KnowledgeBase\\平均修复时间.txt");
    QTableWidgetItem * item = new QTableWidgetItem;
    item = account->item(x,y);
    QString path1 = item->text();
    QString path2 = path1.replace(QRegExp("\\/"), "\\");
    QString path3 = "explorer "+path2;
    if(path1.contains("D:\\",Qt::CaseSensitive))//排除第一列的影响，字符串包含D:\\证明是选择的第二列
        QProcess::execute(path3);
}
