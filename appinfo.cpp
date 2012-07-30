#include "appinfo.h"
#include <QFile>



AppInfo    app_info;



AppInfo::AppInfo()
{
    LoadInfo();
}

AppInfo::~AppInfo()
{
    SaveInfo();
}
void   AppInfo::SaveInfo(void)
{
    app_info.filename = "multideviceappinfo.dat";
    QFile file(app_info.filename);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    out << this->initialized;
    out << this->filename << this->username << this->password << this->app_with << this->app_height;
    out << this->colomn_withs;
    out << this->row_height;
    out << this->countdowndefault;

     file.close();
}
void   AppInfo::LoadInfo(void)
{
    QFile file("multideviceappinfo.dat");
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    in >> this->initialized;
    in >> this->filename >> this->username >> this->password >> this->app_with >> this->app_height;
    in >> this->colomn_withs;
    in >> this->row_height;
    in >> this->countdowndefault;

    if(app_with < 10) {
        app_with = 10;
    }
    if(app_height < 10) {
        app_height = 10;
    }
    if(row_height < 10) {
        row_height = 10;
    }
    if(initialized == "initialized") {
        return ;
    }


    this->app_with = 770;


    this->app_height = 336;

        this->colomn_withs[1] = 100;


        this->colomn_withs[5] = 514;


        this->row_height = 32;

    for(int i=0;i<32;i++) {
        if(countdowndefault[i].msecsTo(QTime(0,0,0)) == 0) {
            countdowndefault[i] = QTime::fromString("10.00", "m.s");
        }
    }
    initialized = "initialized";
}

void   AppInfo::setCountDownDefaultTime(int index,QTime & time)
{
    countdowndefault[index] = time;
}

QTime & AppInfo::getCountDownDefaultTime(int index)
{
    return this->countdowndefault[index];
}
