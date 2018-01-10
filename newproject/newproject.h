//newproject.h  
#ifndef NEWPROJECT_H  
#define NEWPROJECT_H  
  
#include <QDialog>  
#include <QString>

namespace Ui {  
class NewProject;
}  
  
class NewProject : public QDialog  
{  
    Q_OBJECT  
      
public:  
    explicit NewProject(QWidget *parent = 0);  
    ~NewProject();   
    //获得新建项目名  
    QString get_new_project_name(){return new_project_name_;}  
    //获得新建项目的目录  
    QString get_new_project_dir(){return new_project_dir_;}
    bool select_item[5];  //新建工程时，选择的评估组件 状态标记
    bool NameIsLegal(QString text);
    void JiHuo();

protected:
   void mouseMoveEvent(QMouseEvent *event);
   bool select_judge();
   QString create_proDir();



      
private:  
    Ui::NewProject *ui;   
    QString new_project_name_;//新建项目名  
    QString new_project_dir_;//新建项目文件夹路径

private slots:  
    void choose_project_dir();//选择项目目录按钮
    void on_cancelNewProject_clicked();
    void on_main_next_clicked();
    void on_okNewProject_clicked();

};
  
#endif // NEWPROJECT_H  	
	
	
	
