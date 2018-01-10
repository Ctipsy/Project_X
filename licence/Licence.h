#ifndef LICENCE_H
#define LICENCE_H

using namespace std;
#include<QString>
class Licence
{

private:
    QString getCPUID();
    QString getHDID();
    QString getMachineCode();
    QString calActiveCode(QString machineCode);
    QString getMac();
    QString ReadLicence( );
public:
    bool Judeg_Active();
    int Read_Date_Module(const QString str);
};

#endif // LICENCE_H
