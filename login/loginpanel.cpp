#include "login/loginpanel.h"
#include "main/mainwindow.h"
extern bool g_session;//账户类型区分标识

extern QSqlDatabase m_dbConnect;
extern int active_days;
extern QString  creatUser;     //全局变量 创建项目人


//登录面板构造函数
LoginPanel::LoginPanel()
{
    //判断数据库文件是否存在
    /*oDatabase = QSqlDatabase::addDatabase("QODBC");

    bool create = QFile::exists("C:/719/2017_06_10_Total/rc/DB.mdb");
    if (!create)
    {
        QMessageBox::warning(0, tr("提示"), tr("帐号记录信息文件丢失!请联系管理员"));
    }
    if (!createConnection())
    {
        QMessageBox::warning(0, tr("提示"), tr("连接数据库错误!"));
        return;
    }*/
    //设置登录面板基本属性
    /*if (create&&createConnection())//数据库文件存在，并且成功连接上
    {*/
        this->resize(815, 540);
        this ->setWindowFlags(Qt::FramelessWindowHint);//设置无边框
        this ->setAttribute(Qt::WA_TranslucentBackground);//透明背景
        //关闭
        closeLogButton = new QPushButton(this);
        closeLogButton ->setObjectName(tr("closeLogButton"));
        //最小化
        minLogButton = new QPushButton(this);
        minLogButton ->setObjectName(tr("minLogButton"));
        //登录
        loginButton = new QPushButton(this);
        loginButton ->setObjectName(tr("loginButton"));
        //注册
        registerButton = new QPushButton(this);
        registerButton ->setObjectName(tr("registerButton"));
        //找回密码
        findPasswordButton = new QPushButton(this);
        findPasswordButton ->setObjectName(tr("findPasswordButton"));

        //账号
        userID = new QLineEdit(this);
        userID ->setObjectName(tr("userID"));
        userID->setStyleSheet("QLineEdit{border-width:0;border-style:outset}");

        //密码
        password = new QLineEdit(this);
        password ->setEchoMode(QLineEdit::Password);
        password ->setObjectName(tr("password"));
        password->setStyleSheet("QLineEdit{border-width:0;border-style:outset}");

        //错误提示
        errorLgLabel = new QLabel(this);
        errorLgLabel ->setObjectName(tr("errorLgLabel"));
        errorLgLabel ->setVisible(false);

        errorLgLabel2 = new QLabel(this);
        errorLgLabel2 ->setObjectName(tr("errorLgLabel2"));
        errorLgLabel2 ->setVisible(false);

        //注册面板
        regPanel = new RegPanel(this);

        //找回密码面板
        findPassword = new FindPassword(this);

        //登录成功信息显示面板
        loginInfoLabel = new LoginInfoLabel;

        //注册事件过滤器
        password ->installEventFilter(this);

        //设置组件位置和大小
        closeLogButton ->setGeometry(775, 0, 40, 31);//关闭按钮位置
        minLogButton ->setGeometry(735, 0, 40, 31);//最小按钮位置

        loginButton ->setGeometry(354, 322, 64, 23);
        registerButton ->setGeometry(504, 322, 64, 23);
        findPasswordButton ->setGeometry(248, 318, 78, 28);


        userID ->setGeometry(354, 222, 212, 36);
        password ->setGeometry(354, 275, 212, 36);
        errorLgLabel ->setGeometry(526,228,20,21);
        errorLgLabel2 ->setGeometry(526,281,20,21);

        //信号与槽连接
        connect(registerButton, SIGNAL(clicked()), this, SLOT(registerPanel()));//注册
        connect(findPasswordButton, SIGNAL(clicked()), this, SLOT(findPasswordPanel()));//找回密码
        connect(loginButton, SIGNAL(clicked()), this, SLOT(startLogin()));//开始登录
        connect(closeLogButton, SIGNAL(clicked()), this, SLOT(closeAll()));//关闭
        connect(minLogButton, SIGNAL(clicked()), this, SLOT(showMinimized()));//最小化
        //////////////////////////////////////////////////////////////////////
//        userID ->setText("1");
//        password ->setText("1");
        //////////////////////////////////////////////////////////////////////
    //}

        //  判断软件激活情况，如果没有激活，则对话框提示，并退出程序
        lic = new Licence;
        if(!lic->Judeg_Active())
        {
            QWidget::close();
            QApplication::exit();
        }
        else
        {
            //active_module = lic->Read_Date_Module("Module");
            active_days = lic->Read_Date_Module("Days");
        }
         //  判断软件激活情况，如果没有激活，则对话框提示，并退出程序


}

//连接数据库
bool LoginPanel::createConnection()
{
    /*oDatabase.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};\
    FIL={MS Access};DBQ=C:/719/2017_06_10_Total/rc/DB.mdb");
    QString strUserName="123";
    QString strUserPassword="123";
    oDatabase.setUserName(strUserName);//设置登陆数据库的用户名
    oDatabase.setPassword(strUserPassword);//设置密码
    bool bOpen=oDatabase.open(strUserName,strUserPassword);
    if(bOpen)
    {
//        QMessageBox messageBox;
//        messageBox.setText("Open!");
//        messageBox.exec();
        return true;
     }
     else
     {
         QMessageBox messageBox;
         messageBox.setText("Connect Database Error!");
         messageBox.exec();
         oDatabase.close();
         return false;
     }*/
    return 1;
}

//如果没有创建表，则创建
void LoginPanel::createTable()
{
    QSqlQuery query(m_dbConnect);
       query.exec("drop table regInfo");
       query.exec("create table regInfo (userID varchar(20) primary key not null,"
                  "userName varchar(20), password varchar(30), sex varchar(10),"
                  "creatTime varchar(20), department varchar(30), safeQuestion varchar(30), safeAnswer varchar(20))");

       query.exec("insert into regInfo values('001', '橘右京', 'admin', '男', '2017-05-20', '核心设备部', '你的入职时间？', '20160901')");
}

//设置一个定时器
void LoginPanel::timeOut()
{
    errorLgLabel ->setVisible(false);
    errorLgLabel2 ->setVisible(false);
}

//判断是否登录成功
bool LoginPanel::judgeLogin()
{
    //设置一个定时器
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::red);

    timer = new QTimer(this);
    timer ->start(3000);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeOut()));

    //从数据库中找出相应账号对应的密码
    QSqlQuery query(m_dbConnect);
    query.prepare("select password from regInfo where userID = ?");
    query.addBindValue(userID ->text());
    query.exec();

    QString passwd;

    while (query.next())
    {
        passwd = query.value(0).toString();
    }

    if (userID ->text().isEmpty())//账号为空
    {
        errorLgLabel ->setVisible(true);
        errorLgLabel->setPalette(pe);
    }
    if (password ->text().isEmpty())//密码为空
    {
        errorLgLabel2 ->setVisible(true);
        errorLgLabel2->setPalette(pe);
    }
    if (password ->text() != passwd)//输入密码和数据库不一致
    {
        errorLgLabel2 ->setVisible(true);
        errorLgLabel2->setPalette(pe);
    }
    if (!userID ->text().isEmpty()//登录成功
       && !password ->text().isEmpty()
       && password ->text() == passwd)
    {
        creatUser = userID->text();  //获取项目创建员工ID号，即登录所用ID号码

        if(userID ->text() == "admin")
            {
                QMessageBox::information(0, tr("提示"), tr("管理员身份登录! 请谨慎操作数据!"));
                g_session = true;
            }
        return true;
    }

    return false;
}

//开始登录
bool LoginPanel::startLogin()
{
    if (judgeLogin())
    {
        //loginInfoLabel ->show();
        //qDebug() << "登录成功";
        accept();
        return true;
    }
    else
    {
       // qDebug() << "登录失败";
        return false;
    }
}


//显示注册面板
void LoginPanel::registerPanel()
{
    if (!regPanel ->isVisible())
    {
        regPanel ->showNormal();
    }
}

//显示找回密码面板
void LoginPanel::findPasswordPanel()
{
    if (!findPassword ->isVisible())
    {
        findPassword ->showNormal();
    }
}

//关闭所有程序
void LoginPanel::closeAll()
{
//    //设置界面渐变显示
//    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
//    animation ->setDuration(1000);
//    animation ->setStartValue(1);
//    animation ->setEndValue(0);
//    animation ->start();
    regPanel ->close();
    findPassword ->close();
    loginInfoLabel->close();
    //oDatabase.close();
    this ->close();
}

//事件过滤器
bool LoginPanel::eventFilter(QObject *target, QEvent *event)
{
    if (target == password)
    {
        if (event ->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent ->key() == Qt::Key_Return)
            {
                qDebug() << "Key_Return";
                return true;
            }
        }
    }

    return QDialog::eventFilter(target, event);
}

//重画面板
void LoginPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap backgroundImage;
    backgroundImage.load(":/image/login/background.png");

    this ->setMask(backgroundImage.mask());
    painter.drawPixmap(0, 0, 815, 540, backgroundImage);
}

//处理鼠标点击事件
void LoginPanel::mousePressEvent(QMouseEvent *event)
{
    if (event ->button() == Qt::LeftButton)
    {
        dragPosition = event ->globalPos() - frameGeometry().topLeft();
        event ->accept();
    }
}

//处理鼠标移动事件
void LoginPanel::mouseMoveEvent(QMouseEvent *event)
{
    if (event ->buttons() == Qt::LeftButton)
    {
        move(event ->globalPos() - dragPosition);
        event ->accept();
    }
}

