#ifndef APPINFO_H
#define APPINFO_H

#include <QObject>
#include <QDataStream>
#include <QString>
#include <QMap>
#include <QTime>



//extern QDataStream& operator>>(QDataStream&, AppInfo&);
//extern QDataStream& operator<<(QDataStream&, AppInfo&);

#include <QTime>
#include <QMap>

class AppInfo : public QObject
{
public:
    AppInfo();
    ~AppInfo();
    void   setCountDownDefaultTime(int index,QTime & time);
    QTime & getCountDownDefaultTime(int index);
    void   LoadInfo(void);
    void   SaveInfo(void);
public:
    QString  initialized;
    QString  filename;
    QString  username;
    QString  password;
    QMap<int,int>   colomn_withs;
    int row_height;
    int app_with;
    int app_height;
    //
private:
    QMap<int,QTime> countdowndefault;
};

extern AppInfo    app_info;

#endif // APPINFO_H
