#include "login/findpassword.h"
//找回密码构造方法
FindPassword::FindPassword(QDialog *dialog)
{
    //设置面板基本属性
    this ->resize(700, 525);//设置大小
    //去掉窗体边框和总是显示在屏幕最前面,但要和showNormal一起用
   // this ->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this ->setAttribute(Qt::WA_TranslucentBackground);//设置背景透明

    //关闭
    closeFdButton = new QPushButton(this);
    closeFdButton ->setObjectName("closeFdButton");

    //最小化
    minFdButton = new QPushButton(this);
    minFdButton ->setObjectName("minFdButton");




    //显示信息
    infoFdLabel = new QLabel(this);
    infoFdLabel ->setObjectName("infoFdLabel");

    //账号
    userFdID = new QLineEdit(this);
    userFdID ->setObjectName("userFdID");

    //密保问题
    safeFdQuestionList << tr("你的入职时间？") << tr("你最爱的水果？")<< tr("你最爱的城市？") << tr("你最喜欢的数字？");
    safeFdQuestion = new QComboBox(this);
    safeFdQuestion ->setObjectName("safeFdQuestion");
    safeFdQuestion ->addItems(safeFdQuestionList);

    //密保答案
    safeFdAnswer = new QLineEdit(this);
    safeFdAnswer ->setObjectName("safeFdAnswer");

    //确定
    okFdButton = new QPushButton(this);
    okFdButton ->setObjectName("okFdButton");

    //密码标签
    passwordLabel = new QLabel(this);    
    passwordLabel->setStyleSheet("color:green;");
    passwordLabel ->setObjectName("passwordLabel");
    QFont font(" ",28,4);
    passwordLabel->setFont(font);


    //设置组件事件过滤器
    userFdID ->installEventFilter(this);
    safeFdAnswer ->installEventFilter(this);

    //设置注册面板各个组件的位置和大小
    closeFdButton ->setGeometry(656, 0, 40, 31);  //(656, 0, 44, 60);
    minFdButton ->setGeometry(611, 0, 40, 31);

    infoFdLabel ->setGeometry(130, 199, 117, 127);
    userFdID ->setGeometry(261, 195, 222, 37);
    safeFdQuestion ->setGeometry(261, 245, 222, 37);
    safeFdAnswer ->setGeometry(261, 295, 222, 37);
    passwordLabel ->setGeometry(250, 380, 500, 39);

    okFdButton ->setGeometry(266, 458, 166, 44);

    //信号与槽连接
    connect(closeFdButton, SIGNAL(clicked()), this, SLOT(closeFd()));//关闭
    connect(minFdButton, SIGNAL(clicked()), this, SLOT(showMinimized()));//最小化
    connect(okFdButton, SIGNAL(clicked()), this, SLOT(findPwd()));//找回密码
}

//找回密码
void FindPassword::findPwd()
{
    passwordLabel->setVisible(true);
    //从数据库中找出相应账号对应的密码
    QSqlQuery query;
    query.prepare("select password, safeQuestion, safeAnswer from regInfo where userID = ?");
    query.addBindValue(userFdID ->text());
    query.exec();

    QString password;
    QString safeQuestion;
    QString safeAnswer;

    while (query.next())
    {
        password = query.value(0).toString();
        safeQuestion = query.value(1).toString();
        safeAnswer = query.value(2).toString();
    }

    if (userFdID ->text()=="admin"||userFdID ->text().isEmpty()||safeFdAnswer ->text().isEmpty()||safeFdQuestion ->currentText() != safeQuestion||safeFdAnswer ->text() != safeAnswer)//账号为空
    {
       QMessageBox::warning(0, "提示", "帐号不存在或信息不匹配！");
    }

    if (!userFdID ->text().isEmpty()        //输入成功
        && !safeFdAnswer ->text().isEmpty()
        && safeQuestion == safeFdQuestion ->currentText()
        && safeAnswer == safeFdAnswer ->text()
        && userFdID ->text()!="admin")
    {
        passwordLabel ->setText("密码为：" + password);
    }
}

//关闭找回密码界面
void FindPassword::closeFd()
{
    userFdID ->setText("");
    safeFdAnswer ->setText("");
    passwordLabel ->setText("");
    passwordLabel ->setVisible(false);
    passwordLabel->close();
    this ->close();
}

//事件过滤器
bool FindPassword::eventFilter(QObject *target, QEvent *event)
{
    if (target == userFdID || target == safeFdAnswer)
    {
        if (event ->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent ->key() == Qt::Key_Return)
            {
                okFdButton ->setCheckable(true);//选中确定按钮
                this ->close();
                return true;
            }
        }
    }

    return QDialog::eventFilter(target, event);
}

//重画面板
void FindPassword::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap backgroundImage;
    backgroundImage.load(":/image/findpassword/fregBg.png");

    this ->setMask(backgroundImage.mask());
    painter.drawPixmap(0, 0, 700, 525, backgroundImage);
}

//处理鼠标点击事件
void FindPassword::mousePressEvent(QMouseEvent *event)
{
    if (event ->button() == Qt::LeftButton)
    {
        dragPosition = event ->globalPos() - frameGeometry().topLeft();
        event ->accept();
    }
}

//处理鼠标拖拽事件
void FindPassword::mouseMoveEvent(QMouseEvent *event)
{
    if (event ->buttons() == Qt::LeftButton)
    {
        move(event ->globalPos() - dragPosition);//移动窗体
        event ->accept();
    }
}



