#include "history.h"
#include "ui_history.h"
#include "QFile"
#include <QDebug>
#include <QTextCodec>
#include <QProcess>
#include <QMessageBox>
#include <QDir>
extern QString  creatName;     //全局变量 创建项目名称
extern QString  creatTime;     //全局变量 创建项目时间
extern int  active_module;
extern QString  creatUser;     //全局变量 创建项目名称


History::History(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::History)
{
    ui->setupUi(this);
    initial();
}

History::~History()
{
    delete ui;
}
void History::initial()
{
    this->setWindowTitle("历史项目管理");
    Project_Info = new QTableWidget(this);
    ui->verticalLayout->addWidget(Project_Info);
    Project_Info->setVisible(1);
    //设置分数较低的设备名
    Project_Info->setColumnCount(6);
    Project_Info->verticalHeader()->setVisible(false); //隐藏行序号
    Project_Info->setColumnHidden(5,true);  //最后一列的 仿真模块个数 隐藏，因为前面已经有了模块名称
    //Project_Info->horizontalHeader()->setVisible(false); //隐藏列序号
    Project_Info->horizontalHeader()->setStretchLastSection(true);   //设置tableWidget末尾不留空
    Project_Info->setSelectionBehavior(QAbstractItemView::SelectRows);
    Project_Info->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //x先自适应宽度
    //Project_Info->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列
    QStringList header;
    header<<"项目名称"<<"创建人"<<"创建时间"<<"项目路径"<<"仿真构件";
    Project_Info->setHorizontalHeaderLabels(header);
    read_project_dat();
}

void History::read_project_dat()
{
    int currentRow=0;
    QStringList list;

    QTextCodec *codec =  QTextCodec::codecForName("GBK");  //解决编码问题
    QFile file("D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles/projects.dat");
    if (file.open(QIODevice::ReadOnly))
    {
        while (!file.atEnd())
        {
            QByteArray line = file.readLine();   //逐行读取，原理应该是判断编码中的换行符号 /r或者/n
             list<<codec->toUnicode(line);
        }
        file.close();
    }

    for(int i=0;i<list.size();i++)
    {
        QStringList sections = list.at(i).split(QRegExp("[,]")); //把每一个块装进一个QStringList中   参考文档：http://blog.csdn.net/xuleisdjn/article/details/51438162
        Project_Info->insertRow(currentRow);
        for(int i=0; i<sections.size();i++)  //获取每一行字符串中 的 每一块 内容，最后一项 激活模块个数这一列被隐藏了
        {
           // qDebug()<<"测试项目"<<sections.at(i).trimmed(); //去掉空格
            Project_Info->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列

            Project_Info->setItem( currentRow, i, new QTableWidgetItem(  sections.at(i).trimmed()  ) );//每格显示的内容
        }
        currentRow++;
    }
    connect(Project_Info, SIGNAL(cellClicked(int,int)),this,SLOT(editTxt(int,int))); //单击，吧

}

/************************************************************************************************************/
//                                      双击项目路径后，打开对应路径
/************************************************************************************************************/
void History::editTxt(int x,int y)
{

    QTableWidgetItem *item = new QTableWidgetItem;
    item = Project_Info->item(x,5);
    active_module = item->text().toInt();
    qDebug()<<"最初读的激活数"<<active_module;
    item = Project_Info->item(x,3);
    xml_path =item->text();
    item = Project_Info->item(x,0);
    creatName = item->text();
    item = Project_Info->item(x,2);
    creatTime = item->text();
    item = Project_Info->item(x,1);
    creatUser = item->text();
    //qDebug()<<"测试历史"<<xml_path<<modules;

}


 /*exec（）是一个循环时间函数，哪它什么时候才能返回了？
 * 当调用 accept（）（返回QDialog::Accepted），
 * reject（）（返回QDialog::Rejected），
 * done（int r）（返回r），
 * close（）（返回QDialog::Rejected），
 * hide（）（返回 QDialog::Rejected），
 * destory（）（返回QDialog::Rejected）。
 * 还有就是delete 自己的时候也会返回 QDialog::Rejected（destory（）就会delete自己）。
 * */
void History::on_open_clicked()
{
    accept();
}

void History::on_deleter_clicked() //删除行槽函数
{
    int rowIndex = Project_Info->currentRow();
    //qDebug()<<"当前所指行数"<<rowIndex;
    QMessageBox msgBox(QMessageBox::Warning, tr("警告"),"确定要删除该项目？", 0, this);
    msgBox.addButton(tr("确定"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("取消"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::AcceptRole)
    {
        if (rowIndex != -1)
            Project_Info->removeRow(rowIndex);
        ui->deleter->setFocus();
        DelDir("D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles/"+creatName);
        read_table();

    }
}

bool History::DelDir(const QString &path)
{
    if (path.isEmpty()){
        return false;
    }
    QDir dir(path);
    if(!dir.exists()){
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach (QFileInfo file, fileList){ //遍历文件信息
        if (file.isFile()){ // 是文件，删除
            file.dir().remove(file.fileName());
        }else{ // 递归删除
            DelDir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath()); // 删除文件夹
}


/************************************************************************************************************/
//                                 读取项目历史表格，并遍历保存所有内容
/************************************************************************************************************/
void History::read_table()
{
    QStringList list;
    int row = Project_Info->rowCount();

    int col = Project_Info->columnCount();
    qDebug()<<"行数和列数"<<row<<" "<<col;
    for(int i=0;i<row;i++)
        for(int j=0;j<col;j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem;
            item = Project_Info->item(i,j);
            list.append(item->text());
        }

    //将表格的数据清空重写到projects.dat里

    QFile pro_file2("D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles/projects.dat");
    pro_file2.open(QIODevice::WriteOnly|QIODevice::Truncate);
    QTextStream pro_stream2(&pro_file2);
    for(int num=1;num<list.size()+1;num++)
    {
        if(num%6!=0)
            pro_stream2<<list.at(num-1)<<",";
        else
        {
            pro_stream2<<list.at(num-1)<<"\n";  //最后一行不需要加逗号，直接换行即可
        }
    }


}



