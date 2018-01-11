#include "newproject.h"  
#include "ui_newproject.h"
#include <QFileDialog>  
#include <QMessageBox>  
#include "newproject/project_guide.h"
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QString>
#include <QMouseEvent>
#include <QDebug>
#include "sys_discript/system_informa.h"
#include "QDateTime"
//构造函数  
extern System_Informa* sys_inf;
extern QString  creatUser;     //全局变量 创建项目人
extern QString  creatTime;     //全局变量 创建项目时间
extern QString  creatPath;     //全局变量 创建项目路径
extern QString  creatName;     //全局变量 创建项目名称
extern int jihuo;
NewProject::NewProject(QWidget *parent) :  
    QDialog(parent),  
    ui(new Ui::NewProject)  
{

    ui->setupUi(this);
    setWindowTitle("新建工程向导");	
    ui->main_next->hide();
    ui->gongzuo_select->setEnabled(false);
    this->setMouseTracking(true);//设置光标追踪事件
    ui->groupBox->setMouseTracking(true);
    ui->groupBox ->installEventFilter(this);
    ui->wuli_select ->installEventFilter(this);
    ui->jiagou_select ->installEventFilter(this);
    ui->xinxi_select ->installEventFilter(this);
    ui->gongzuo_select ->installEventFilter(this);
    ui->taishi_select ->installEventFilter(this);
    ui->info->setToolTip("必须勾选前三项，才可进行工作流仿真！");
    JiHuo();
// // 使用内置图标
//   QStyle*style = QApplication::style();
//   QIcon icon = style->standardIcon(QStyle::SP_FileIcon);
//   ui->okNewProject->setIcon(icon);
    //选择新建项目目录按钮信号槽



    for(auto i=0;i<5;i++)
        select_item[i]=false;
    connect(ui->chooseProjectDir,SIGNAL(clicked()),this,SLOT(choose_project_dir()));

}
//析构函数  
NewProject::~NewProject()  
{  
    delete ui;  
}  



/************************************************************/  
  /*功能：选择新建项目目录  
    设计：弹出选择文件夹的对话框，选择文件夹*/  
/************************************************************/  
void NewProject::choose_project_dir()  
{  
    QString path=QFileDialog::getOpenFileName(this,tr("打开评估信息模板文件"),tr("D:/Program Files (x86)/X_Ship/DB_51aspx/XML_templet/"),tr("Project Files(*.xml)")); //2018.1.1日添加，打开默认路径下的文件夹
    QFileInfo fi;
    fi = QFileInfo(path);
    if(!path.isEmpty())
    {
        QFile cXmlFile(path);
        if(!cXmlFile.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            QMessageBox::warning(this,tr("加载模板文件"),tr("文件加载错误 ！").arg(path));
            return;
        }
        new_project_dir_ = path;
        ui->projectDir->setText(fi.fileName()); //获取文件名

       // qDebug()<<"测试xml模板路径"<<fi.fileName();
     }


}  

//确定新建项目
void NewProject::on_okNewProject_clicked()
{
    ui->main_next->show();
   // QMessageBox::information(this,"提示","已经入next",QMessageBox::Yes);

    if(ui->wuli_select->isChecked()==true&&ui->jiagou_select->isChecked()==true&&ui->xinxi_select->isChecked()==true)
    {
        ui->gongzuo_select->setEnabled(true);
    }
    if(new_project_dir_=="") //如果没有勾选评估xml模板，那么就用默认的xml
    {
        new_project_dir_ = "D:/Program Files (x86)/X_Ship/DB_51aspx/XML_templet/default.xml";//要复制的潜艇型号和系统描述xml文件的绝对路径
        ui->projectDir->setText("default.xml"); //获取文件名
    }
}

void NewProject::on_cancelNewProject_clicked()
{
    QMessageBox::information(this,"提示","已取消新建项目",QMessageBox::Yes);
    accept();
}

/************************************************************/
           /*功能：点击下一步后，创建文件夹，复制xml文件 */
/************************************************************/
QString NewProject::create_proDir()
{
    //新建项目文件夹
    QDir dir("D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles");//项目文件夹所在固定目录  //2017.12.30PM修改
    dir.mkdir(new_project_name_);//创建以项目名为名字的目录
    QString _project_dir_=dir.absoluteFilePath(new_project_name_);//获取项目文件夹的绝对路径


    QDir tempDir;
    QString filePath = "D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles/"+new_project_name_+"/DB_51aspx"          ;
    QString currentDir = tempDir.currentPath();
    //如果filePath路径不存在，创建它
    if(!tempDir.exists(filePath))
    {
        tempDir.mkpath(filePath);
    }


    QDir project_dir(_project_dir_);
    //以下方法可以将文件复制到所选则的文件根目录下
    QFile file(new_project_dir_);
    QFile file2("D:/Program Files (x86)/X_Ship/DB_51aspx/Lj_QuestionnaireSys.mdb");//要复制的数据库文件路径
    QFile file3("D:/Program Files (x86)/X_Ship/QuestionnaireSystemUser.exe");
    QFile file4("D:/Program Files (x86)/X_Ship/WeifenLuo.WinFormsUI.Docking.dll");

    QString xml_data = new_project_name_+".xml"; //根据新建项目名，重新拷贝命名一份xml文件
    QString mdb_data = "Lj_QuestionnaireSys.mdb"; //
    QString exe_name = "QuestionnaireSystemUser.exe";
    QString dll_name = "WeifenLuo.WinFormsUI.Docking.dll";



    QFileInfo info(xml_data);//要生成的文件名
    QFileInfo info2(mdb_data);//
    QFileInfo info3(exe_name);
    QFileInfo info4(dll_name);


    QString dstPath = _project_dir_+QString("/");//  加/表示子目录
    QString dstPath2 = filePath+"/";//
    QString dstPath3 = "D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles/"+new_project_name_+"/" ;
    QString dstPath4 = "D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles/"+new_project_name_+"/" ;


    dstPath += info.fileName();//不加这句，没法生成xml文件
    dstPath2 += info2.fileName();//不加这句，没法生成mdb文件
    dstPath3 += info3.fileName();//不加这句，没法生成mdb文件
    dstPath4 += info4.fileName();//不加这句，没法生成mdb文件



    file.copy(dstPath);
    file2.copy(dstPath2);
    file3.copy(dstPath3);
    file4.copy(dstPath4);


    file.close();
    file2.close();
    file3.close();
    file4.close();


    //新建项目文件pro
    QFile pro_file( project_dir.filePath(new_project_name_+".project") );
    pro_file.open(QFile::WriteOnly|QIODevice::Text);
    //新建历史评估结果xml
    QFile result_file1( project_dir.filePath(new_project_name_+"_wuli"+".xml") );
    result_file1.open(QFile::WriteOnly|QIODevice::Text);
    //新建历史评估结果xml
    QFile result_file2( project_dir.filePath(new_project_name_+"_jiagou"+".xml") );
    result_file2.open(QFile::WriteOnly|QIODevice::Text);
    //新建历史评估结果xml
    QFile result_file3( project_dir.filePath(new_project_name_+"_xinxi"+".xml") );
    result_file3.open(QFile::WriteOnly|QIODevice::Text);

    //pro_path_.append(pro_file.fileName());//添加pro文件
    QTextStream pro_stream(&pro_file);
    pro_stream<<"Project was created successfully!\n";
    pro_stream<<"PROJECT NAME = "<<new_project_name_<<"\n";//添加项目名
    pro_stream<<"PROJECT DIR = "<<_project_dir_<<"\n";//添加项目路径
    pro_stream<<"PROJECT FILE NAME = "<<pro_file.fileName()<<"\n\n\n\n";//添加pro文件名的路径
//    pro_stream<<"________________This is a test demo on 12.25.2017@Ctipsy___________________";


    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm");
    creatTime = current_date;
    QFile pro_file2("D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles/projects.dat");
    pro_file2.open(QIODevice::WriteOnly|QIODevice::Append);
    QTextStream pro_stream2(&pro_file2);
    pro_stream2<<creatName<<",";
    pro_stream2<<creatUser<<",";
    pro_stream2<<creatTime<<",";
    pro_stream2<<creatPath<<",";

    return dstPath; //返回项目绝对路径，给on_main_next_clicked()函数调用
}

// 判断文件名命名是否合法
bool NewProject::NameIsLegal(QString text)
{
    bool is_legal = false;
    QString pattern("[\\\\/:*?\"<>|]");
    QRegExp rx(pattern);
    int match = text.indexOf(rx);
    if(match >= 0)
    {
        is_legal = true;
    }
    return is_legal;
}


//填写项目名称和位置 那个界面的下一步
void NewProject::on_main_next_clicked()
{
    //获取项目名
     new_project_name_=ui->projectName->text().trimmed();
    //判断名字是否为空
    if( new_project_name_.isEmpty() || new_project_dir_.isEmpty()||select_judge() ||  NameIsLegal(new_project_name_))
    {
        QMessageBox::information(this,"提示","创建失败：表格未填完整或命名不规范！",QMessageBox::Yes);
    }
    else
    {       
        creatName = ui->projectName->text();
        creatPath = "D:/Program Files (x86)/X_Ship/DB_51aspx/ProjectFiles/"+creatName+"/";
        QString dstPath=create_proDir();
        if(!dstPath.isEmpty())
        {
            QFile cXmlFile(dstPath);
            if(!cXmlFile.open(QIODevice::ReadOnly|QIODevice::Text))
            {
                QMessageBox::warning(this,tr("Read File"),tr("cannot open file:\n%1").arg(dstPath));
                return;
            }
            QTextStream floStream(&cXmlFile);
            QTextCodec *codec=QTextCodec::codecForName("UTF-8");
            floStream.setCodec(codec);
            QString xmlDataStr = floStream.readAll();
            QDomDocument doc;
            doc.setContent(xmlDataStr);
            cXmlFile.close();
            delete sys_inf;
            sys_inf=new System_Informa(doc);
            //项目配置向导
            project_guide *projectguide= new project_guide(select_item,5,0);
            done(projectguide->exec());
        }
    }
}



//处理鼠标点击事件,用来实时刷新判断复选框的选择状态
void NewProject::mouseMoveEvent(QMouseEvent *event)
{
    int x=event->x();
    int y=event->y();
    if(x!=y)
    {
        if(ui->wuli_select->isChecked()==true&&ui->jiagou_select->isChecked()==true&&ui->xinxi_select->isChecked()==true&&jihuo>4)
        {
            ui->gongzuo_select->setEnabled(true);
            if(ui->gongzuo_select->isChecked()==true&&jihuo>5)
                ui->taishi_select->setEnabled(true);
            else
            {
                ui->taishi_select->setChecked(false);
                ui->taishi_select->setEnabled(false);
            }
        }
        if(ui->wuli_select->isChecked()==false||ui->jiagou_select->isChecked()==false||ui->xinxi_select->isChecked()==false)
        {
            ui->gongzuo_select->setChecked(false);
            ui->gongzuo_select->setEnabled(false);
            ui->taishi_select->setChecked(false);
            ui->taishi_select->setEnabled(false);
        }
    }
    ui->wuli_select->setChecked(true);  //2018.1.2必须至少选择一项，不然无法评估
    if(ui->wuli_select->isChecked()==true)   select_item[0] = true; else  select_item[0] = false;
    if(ui->jiagou_select->isChecked()==true) select_item[1] = true; else  select_item[1] = false;
    if(ui->xinxi_select->isChecked()==true)  select_item[2] = true; else  select_item[2] = false;
    if(ui->gongzuo_select->isChecked()==true)select_item[3] = true; else  select_item[3] = false;
    if(ui->taishi_select->isChecked()==true) select_item[4] = true; else  select_item[4] = false;

}
//处理鼠标点击事件,用来实时刷新判断复选框的选择状态
bool NewProject::select_judge()
{
    if(ui->wuli_select->isChecked()==true||ui->jiagou_select->isChecked()==true||ui->xinxi_select->isChecked()==true||ui->gongzuo_select->isChecked()==true||ui->taishi_select->isChecked()==true)
        return false;
    else
        return true;
}

void NewProject::JiHuo()
{
    //ui->projectName->setFocusPolicy(Qt::NoFocus);
    qDebug()<<"激活测试"<<jihuo;
    switch (jihuo)
    {
        case 1:
        {
            ui->jiagou_select->setEnabled(false);ui->jiagou_select->setFocusPolicy(Qt::NoFocus);
            ui->xinxi_select->setEnabled(false);ui->jiagou_select->setFocusPolicy(Qt::NoFocus);
            ui->gongzuo_select->setEnabled(false);ui->jiagou_select->setFocusPolicy(Qt::NoFocus);
            ui->taishi_select->setEnabled(false);ui->jiagou_select->setFocusPolicy(Qt::NoFocus);
        }
        break;
        case 2:
        {
            ui->xinxi_select->setEnabled(false);ui->jiagou_select->setFocusPolicy(Qt::NoFocus);
            ui->gongzuo_select->setEnabled(false);ui->jiagou_select->setFocusPolicy(Qt::NoFocus);
            ui->taishi_select->setEnabled(false);ui->jiagou_select->setFocusPolicy(Qt::NoFocus);
        }
        break;
        case 3:
        {
            ui->jiagou_select->setEnabled(false);ui->jiagou_select->setFocusPolicy(Qt::NoFocus);
            ui->gongzuo_select->setEnabled(false);ui->jiagou_select->setFocusPolicy(Qt::NoFocus);
            ui->taishi_select->setEnabled(false);ui->jiagou_select->setFocusPolicy(Qt::NoFocus);
        }
        break;
        case 4:
        {
            ui->gongzuo_select->setEnabled(false);ui->jiagou_select->setFocusPolicy(Qt::NoFocus);
            ui->taishi_select->setEnabled(false);ui->jiagou_select->setFocusPolicy(Qt::NoFocus);
        }
        break;
        case 5:
        {
            ui->taishi_select->setEnabled(false);ui->jiagou_select->setFocusPolicy(Qt::NoFocus);
        }
        break;
        case 6:
        {
        }
        break;

         default:
        break;
    }
}

