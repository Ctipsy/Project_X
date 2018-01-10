#include "login/regpanel.h"
#include <QMessageBox>
//注册界面构造方法
RegPanel::RegPanel(QDialog *dialog)
{
    //设置面板基本属性
    this ->resize(700, 525);//设置大小
    //去掉窗体边框和总是显示在屏幕最前面,但要和showNormal一起用
  //  this ->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this ->setAttribute(Qt::WA_TranslucentBackground);//设置背景透明

    //关闭
    closeRegButton = new QPushButton(this);
    closeRegButton ->setObjectName(tr("closeRegButton"));

    //最小化
    minRegButton = new QPushButton(this);
    minRegButton ->setObjectName(tr("minRegButton"));

    //注册标签
    regLabel = new QLabel(this);
    regLabel ->setObjectName(tr("regLabel"));

    //注册成功标签
    regSuccessLabel = new QLabel(this);
    regSuccessLabel ->setObjectName(tr("regSuccessLabel"));

    //信息标签
    infoLabel = new QLabel(this);
    infoLabel ->setObjectName(tr("infoLabel"));
    infoLabel3 = new QLabel(this);
    infoLabel3 ->setObjectName(tr("infoLabel3"));

    //成功信息标签
    infoLabel2 = new QLabel(this);
    infoLabel2 ->setObjectName(tr("infoLabel2"));    

    //用户ID
    userNUM = new QLineEdit(this);
    userNUM ->setObjectName(tr("userNUM"));

    //姓名
    userRegName = new QLineEdit(this);
    userRegName ->setObjectName(tr("userRegName"));

    //密码
    passwordReg = new QLineEdit(this);
    passwordReg ->setObjectName(tr("passwordReg"));
    passwordReg ->setEchoMode(QLineEdit::Password);

    //确认密码
    passwordRegOk = new QLineEdit(this);
    passwordRegOk ->setObjectName(tr("passwordRegOk"));
    passwordRegOk ->setEchoMode(QLineEdit::Password);

    //所属部门
    departmentList << tr("核心设备部")<< tr("雷达") << tr("安检")<< tr("总体")<< tr("电力")<< tr("通讯")<< tr("动力")<< tr("控制室")<< tr("维修");
    department = new QComboBox(this);
    department ->setObjectName(tr("department"));
    department ->addItems(departmentList);





    //注册日期
    regday = new QDateTimeEdit(this);
    QDateTime currentdate = QDateTime::currentDateTime();
    QString current_date = currentdate.toString("yyyy-M-dd");
    regday->text() = current_date;
    regday ->setObjectName(tr("regday"));
    regday ->setDisplayFormat("yyyy-M-dd");
    regday ->setCalendarPopup(true);



    //单选按钮
    boyButton = new QRadioButton(tr("男"), this);
    boyButton ->setChecked(true);
    girlButton = new QRadioButton(tr("女"), this);
    buttonGroup = new QButtonGroup(this);
    buttonGroup ->addButton(boyButton);
    buttonGroup ->addButton(girlButton);

    //注册
    startRegButton = new QPushButton(this);
    startRegButton ->setObjectName(tr("startRegButton")); 
   

    //密保问题
    safeRegQuestionList << tr("你的入职时间？") << tr("你最爱的水果？")<< tr("你最爱的城市？") << tr("你最喜欢的数字？");
    safeRegQuestion = new QComboBox(this);
    safeRegQuestion ->setObjectName(tr("safeRegQuestion"));
    safeRegQuestion ->addItems(safeRegQuestionList);

    //密保答案
    safeRegAnswer = new QLineEdit(this);
    safeRegAnswer ->setObjectName(tr("safeRegAnswer"));

    //确定按钮
    okRegButton = new QPushButton(this);
    okRegButton ->setObjectName(tr("okRegButton"));

    //错误提示
	errorLabel0 = new QLabel(this);
    errorLabel0 ->setObjectName(tr("errorLabel0"));
    errorLabel0 ->setVisible(false);
    errorLabel = new QLabel(this);
    errorLabel ->setObjectName(tr("errorLabel"));
    errorLabel ->setVisible(false);
    errorLabel2 = new QLabel(this);
    errorLabel2 ->setObjectName(tr("errorLabel2"));
    errorLabel2 ->setVisible(false);
    errorLabel3 = new QLabel(this);
    errorLabel3 ->setObjectName(tr("errorLabel3"));
    errorLabel3 ->setVisible(false);
    errorLabel4 = new QLabel(this);
    errorLabel4 ->setObjectName(tr("errorLabel4"));
    errorLabel4 ->setVisible(false);

    //把注册成功界面的组件隐藏  
    infoLabel2 ->setVisible(false);
    regSuccessLabel ->setVisible(false);
    okRegButton ->setVisible(false);
    

    //各个组件注册事件过滤器
	userNUM ->installEventFilter(this);
    userRegName ->installEventFilter(this);
    passwordReg ->installEventFilter(this);
    passwordRegOk ->installEventFilter(this);
    safeRegQuestion ->installEventFilter(this);
	safeRegAnswer ->installEventFilter(this);

	

    //设置注册面板各个组件的位置和大小
    closeRegButton ->setGeometry(656, 0, 40, 31);
    minRegButton ->setGeometry(611, 0, 40, 31);

    regLabel ->setGeometry(316, 61, 78, 42);
    infoLabel ->setGeometry(32, 129, 87, 251);
    infoLabel3 ->setGeometry(388, 129, 87, 251);

    regSuccessLabel ->setGeometry(273, 170, 155, 47);
    //userID ->setGeometry(260, 138, 500, 39);

    infoLabel2 ->setGeometry(146, 251, 95, 74);
	
    userNUM->setGeometry(120, 122, 170, 37);
    userRegName ->setGeometry(120, 168, 170, 37);
    passwordReg ->setGeometry(120, 215, 170, 37);
    passwordRegOk ->setGeometry(120, 260, 170, 37);

    department ->setGeometry(476, 122, 170, 37);
    regday ->setGeometry(476, 168, 170, 37);
    safeRegQuestion ->setGeometry(475, 215, 172, 37);
    safeRegAnswer ->setGeometry(476, 260, 170, 37);

    boyButton ->setGeometry(138, 312, 40, 25);
    girlButton ->setGeometry(250, 312, 40, 25);

    startRegButton ->setGeometry(266, 430, 169, 48);
    okRegButton ->setGeometry(266, 430, 166, 44);


	
    errorLabel0 ->setGeometry(250, 268, 20, 21);
    errorLabel ->setGeometry(250, 129, 20, 21);
    errorLabel2 ->setGeometry(250, 175, 20, 21);
    errorLabel3 ->setGeometry(250, 222, 20, 21);
    errorLabel4 ->setGeometry(250, 268, 20, 21);






    //信号与槽连接
    connect(closeRegButton, SIGNAL(clicked()), this, SLOT(closeReg()));//关闭
    connect(minRegButton, SIGNAL(clicked()), this, SLOT(showMinimized()));//最小化
    connect(startRegButton, SIGNAL(clicked()), this, SLOT(startReg()));//开始注册
    connect(okRegButton, SIGNAL(clicked()), this, SLOT(regInfoToDb()));//把信息录入数据库
}

//设置一个定时器
void RegPanel::timeOut()
{
    errorLabel ->setVisible(false);
    errorLabel2 ->setVisible(false);
	errorLabel0 ->setVisible(false);
    errorLabel3 ->setVisible(false);
    errorLabel4 ->setVisible(false);
}

//判断是否注册成功
bool RegPanel::judgeReg()
{
    //设置一个定时器
    timer = new QTimer(this);
    timer ->start(3000);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeOut()));

	if (userNUM ->text().isEmpty())//昵称为空
    {
        errorLabel0 ->setVisible(true);
    }
    if(userNUM ->text() == "admin")
    {
        QMessageBox::information(0, tr("提示"), tr("该用户名被占用!!!"));

    }
    if (userRegName ->text().isEmpty())//昵称为空
    {
        errorLabel ->setVisible(true);
    }
    if (passwordReg ->text().isEmpty())//密码为空
    {
        errorLabel2 ->setVisible(true);
    }
    if (passwordRegOk ->text().isEmpty())//确认密码为空
    {
        errorLabel3 ->setVisible(true);
    }
    if (passwordRegOk ->text() != passwordReg ->text())//确认密码不等于密码
    {
        errorLabel3 ->setVisible(true);
    }
    if (!userRegName ->text().isEmpty()     //注册成功
        && !passwordReg ->text().isEmpty()
        && !passwordRegOk ->text().isEmpty()
		&& !userNUM ->text().isEmpty()
        && passwordReg ->text() == passwordRegOk ->text()
        && userNUM ->text() != "admin")
    {
        return true;
    }

    return false;
}

//开始注册
void RegPanel::startReg()
{
    if (judgeReg())//注册成功
    {
        //注册界面隐藏

        regLabel ->setVisible(false);
        infoLabel ->setVisible(false);
        infoLabel3 ->setVisible(false);
        userRegName ->setVisible(false);
		userNUM -> setVisible(false);
        passwordReg ->setVisible(false);
        passwordRegOk ->setVisible(false);
        department ->setVisible(false);

        regday ->setVisible(false);
        boyButton ->setVisible(false);
        girlButton ->setVisible(false);
        startRegButton ->setVisible(false);

        //注册成功界面显示

        infoLabel2 ->setVisible(true);
        regSuccessLabel ->setVisible(true);

        safeRegQuestion ->setGeometry(308, 250, 172, 37);
        safeRegAnswer ->setGeometry(308, 295, 170, 37);

        safeRegAnswer ->setVisible(true);
        safeRegQuestion ->setVisible(true);
        okRegButton ->setVisible(true);
        //userID ->setVisible(true);
    }
}

//关闭
void RegPanel::closeReg()
{
    //清除输入框内容
    userRegName ->setText("");
    userNUM ->setText("");
    passwordReg ->setText("");
    passwordRegOk ->setText("");
    safeRegAnswer ->setText("");

    //错误提示隐藏
	errorLabel0 ->setVisible(false);
    errorLabel ->setVisible(false);
    errorLabel2 ->setVisible(false);
    errorLabel3 ->setVisible(false);
    errorLabel4 ->setVisible(false);

    //注册成功界面隐藏
    infoLabel2 ->setVisible(false);
    //userID ->setVisible(false);
    userNUM ->setVisible(false);	
    regSuccessLabel ->setVisible(false);
    safeRegAnswer ->setVisible(false);
    safeRegQuestion ->setVisible(false);
    okRegButton ->setVisible(false);

    //注册界面显示
    regLabel ->setVisible(true);
    infoLabel ->setVisible(true);
    infoLabel3 ->setVisible(true);
    userRegName ->setVisible(true);
    passwordReg ->setVisible(true);
    passwordRegOk ->setVisible(true);
    department ->setVisible(true);
	userNUM ->setVisible(true);
    regday ->setVisible(true);
    boyButton ->setVisible(true);
    girlButton ->setVisible(true);
    startRegButton ->setVisible(true);
	safeRegAnswer ->setVisible(true);
    safeRegQuestion ->setVisible(true);

    this ->close();
}

//把注册信息录入数据库
void RegPanel::regInfoToDb()
{
    //恢复窗口位置
    safeRegQuestion ->setGeometry(475, 215, 172, 37);
    safeRegAnswer ->setGeometry(476, 260, 170, 37);
    //设置一个定时器
    timer2 = new QTimer(this);
    timer2 ->start(3000);
    connect(timer2, SIGNAL(timeout()), this, SLOT(timeOut()));

    if (safeRegAnswer ->text().isEmpty())
    {
        errorLabel4 ->setVisible(true);
        return;
    }

    QString usernum = userNUM ->text();
    QString userName = userRegName ->text();
    QString password = passwordRegOk ->text();
    QString regdayTime = regday ->text();
    QString departmentLocale = department ->currentText();
    QString safeQuestion = safeRegQuestion ->currentText();
    QString safeAnswer = safeRegAnswer ->text();
    QString localeStr =departmentLocale;
    QString sex;

    if (boyButton ->isChecked())
    {
        sex = "男";
    }
    else if (girlButton ->isCheckable())
    {
        sex = "女";
    }

    //将注册信息录入数据库
    QSqlQuery query;
    query.prepare("insert into regInfo values(?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(usernum);
    query.addBindValue(userName);
    query.addBindValue(password);
    query.addBindValue(sex);
    query.addBindValue(regdayTime);
    query.addBindValue(localeStr);
    query.addBindValue(safeQuestion);
    query.addBindValue(safeAnswer);
    query.exec();

    this ->closeReg();
}

//事件过滤器
bool RegPanel::eventFilter(QObject *target, QEvent *event)
{
    if (target == userRegName || target == passwordReg || target == passwordRegOk)
    {
        if (event ->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent ->key() == Qt::Key_Return)
            {
                startRegButton ->setCheckable(true);//选中注册按钮
                return true;
            }
        }
    }

    if (target == safeRegAnswer)
    {
        if (event ->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent ->key() == Qt::Key_Return)
            {
                okRegButton ->setCheckable(true);//选中确定按钮
                return true;
            }
        }
    }

    return QDialog::eventFilter(target, event);
}


//重画面板
void RegPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap backgroundImage;
    backgroundImage.load(":/image/reg/rregBg.png");

    this ->setMask(backgroundImage.mask());
    painter.drawPixmap(0, 0, 700, 525, backgroundImage);
}

//处理鼠标点击事件
void RegPanel::mousePressEvent(QMouseEvent *event)
{
    if (event ->button() == Qt::LeftButton)
    {
        dragPosition = event ->globalPos() - frameGeometry().topLeft();
        event ->accept();
    }
}

//处理鼠标拖拽事件
void RegPanel::mouseMoveEvent(QMouseEvent *event)
{
    if (event ->buttons() == Qt::LeftButton)
    {
        move(event ->globalPos() - dragPosition);//移动窗体
        event ->accept();
    }
}



