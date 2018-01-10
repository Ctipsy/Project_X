#include "Licence.h"
//#include<QSettings>
#include<QDebug>
#include<QApplication>
#include<QMessageBox>
#include<windows.h>
//#include<stdio.h>
//#include<QUuid>
//#include<tchar.h>
#include<QFile>
#include<QNetworkInterface>
#include<QCryptographicHash>

#include<QClipboard>
#include<windows.h>
#include<QTime>
#include<QProcess>
#include<QString>


/*****************************************************************/
/*                    获取硬盘的C盘的逻辑序列号                       */
/*****************************************************************/
QString Licence::getHDID()
{

    DWORD VolumeSerialNumber;
    GetVolumeInformation(L"D:\\",NULL,0,&VolumeSerialNumber,NULL,NULL,NULL,0);
    return QString::number(VolumeSerialNumber,16).toUpper();
}

/*****************************************************************/
/*                         获取CPU序列号                           */
/*****************************************************************/
QString Licence::getCPUID()
{
        unsigned long s1,s2,s3,s4;
        char string[128];
        char szCpuId[1024];
        char p1[128], p2[128];
        unsigned int eax = 0;
        unsigned int ebx,ecx,edx;
        QString strCPUID;
        #if 1
        asm volatile
        (
            "cpuid"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "0"(0)
        );
        snprintf(szCpuId, 5, "%s", (char *)&ebx);
        snprintf(szCpuId+4, 5, "%s", (char *)&edx);
        snprintf(szCpuId+8, 5, "%s", (char *)&ecx);
        #endif
        asm volatile
        (
            "movl $0x01 , %%eax ; \n\t"
            "xorl %%edx , %%edx ;\n\t"
            "cpuid ;\n\t"
            "movl %%edx ,%0 ;\n\t"
            "movl %%eax ,%1 ; \n\t"
            :"=m"(s1),"=m"(s2)
        );
        char cpuID[20];
        memset(cpuID,0,20);
        sprintf((char *)p1, "%08X%08X", s1, s2);
        snprintf(szCpuId+12, 20, "%s", (char *)p1);
        memcpy(cpuID,p1,17);
        strCPUID = strCPUID.fromLatin1(cpuID);

        asm volatile
        (
            "movl $0x03,%%eax ;\n\t"
            "xorl %%ecx,%%ecx ;\n\t"
            "xorl %%edx,%%edx ;\n\t"
            "cpuid ;\n\t"
            "movl %%edx,%0 ;\n\t"
            "movl %%ecx,%1 ;\n\t"
            :"=m"(s3),"=m"(s4)
        );

        sprintf((char *)p2, "%08X-%08X\n", s3, s4);
        snprintf(szCpuId+31, 19, "%s", (char *)p2);
        return strCPUID;
}
/*****************************************************************/
/*     用于判断当前正在使用的网卡的MAC地址，如果没有联网，则输出为空        */
/*****************************************************************/

QString Licence::getMac()
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
    int nCnt = nets.count();
    QString strMacAddr = "";
    for(int i = 0; i < nCnt; i ++)
    {
        // 如果此网络接口被激活并且正在运行并且不是回环地址，则就是我们需要找的Mac地址
        if(!nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            strMacAddr = nets[i].hardwareAddress();
            break;
        }
    }
    return strMacAddr;
}

/*****************************************************************/
/*                        把各个参数连接起来                        */
/*****************************************************************/
QString Licence::getMachineCode()
{

    QString machineInfo="";
    machineInfo.append("@");
    machineInfo.append(this->getCPUID());
    machineInfo.append("@");
    machineInfo.append(this->getHDID());
    machineInfo.append("@");
    machineInfo.append(this->getMac());
    machineInfo.append("@");
    QCryptographicHash sha1(QCryptographicHash::Sha1);
    QByteArray datagram(machineInfo.toLatin1());
    const char* tempConstChar = datagram.data();
    sha1.addData(tempConstChar);
    QString  machineCode=sha1.result().toHex();
	return machineCode;

}

/*****************************************************************/
/*           使用SHA1算法，把机器码转换，加密，然后生成激活码             */
/*****************************************************************/
QString Licence::calActiveCode(QString machineCode)
{
    //机器码加上 3个混淆参数，再加上日期这个参数
    QString originalStr120="HUST"+machineCode+"Z111"+"C_tipsy";
    QCryptographicHash sha1(QCryptographicHash::Sha1);//生成一个SHA-1散列，比MD5强度高
    QByteArray datagram(originalStr120.toLatin1()); //获取最终的哈希值
    const char* tempConstChar = datagram.data();
    sha1.addData(tempConstChar);   //读取数值，计算出哈希值如果读取成功，返回true;
    QString  activeCode=sha1.result().toHex();//重置对象到十六进制
    return activeCode;
}

/*****************************************************************/
/*                                激活判断                         */
/*****************************************************************/
bool Licence::Judeg_Active()
{
    
    QString code = calActiveCode(getMachineCode());	
    QClipboard *board = QApplication::clipboard();
    board->setText(getMachineCode());
    if(code == ReadLicence())
    {
        //QMessageBox::warning(0,"软件授权","激活成功，剩余授权使用天数为：118天");
        return true;
    }

    else
    {
        QMessageBox::information (NULL,QObject::tr("软件尚未激活"),QObject::tr("软件ID已经复制到粘贴板上,\r\n\r\n请发给软件供应商以获取激活码"));
        return false;
    }

}


/*****************************************************************/
/*                       读取制定文件夹下的证书文件，并匹配             */
/*****************************************************************/
QString Licence::ReadLicence()
{
    const QString path = "D:\\Program Files (x86)\\X_Ship\\DB_51aspx\\licence\\";
    QString readInfo;
    QString file_name = path+"licence.txt";
    QFile file(file_name);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(0,"提示","证书文件丢失，或尚未激活！请联系软件提供商");
        return "";
    }
    QTextStream in(&file);
    readInfo=in.readAll();
    file.close();
    readInfo=readInfo.replace(QRegExp("\\-"), ""); //移除-字符
    readInfo=readInfo.left(readInfo.length() - 8); //删除后8位随机附加码
    readInfo=readInfo.toLower();                   // 转换成小写字母
    return readInfo;
}
int Licence::Read_Date_Module(const QString str)
{
    const QString path = "D:\\Program Files (x86)\\X_Ship\\DB_51aspx\\licence\\";
    int module,days;
    QString readInfo;
    QString file_name = path+"licence.txt";
    QFile file(file_name);
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&file);
    readInfo=in.readAll();
    //file.close();
    readInfo=readInfo.replace(QRegExp("\\-"), ""); //移除-字符
    readInfo = readInfo.right(readInfo.length() -40);
    QByteArray ba = readInfo.toLatin1(); //已经转成ASCII字符了
    module = (int)(QChar(ba.at(0))).toLatin1()-65;
    days = ((int)(QChar(ba.at(1))).toLatin1()-65)*100+((int)(QChar(ba.at(2))).toLatin1()-65)*10+((int)(QChar(ba.at(3))).toLatin1()-65);
    if(str=="Module")
        return module;
    if(str=="Days")
        return days;

}

