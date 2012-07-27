#ifndef APPINFO_H
#define APPINFO_H

#include <QObject>
#include <QDataStream>
#include <QString>
#include <QMap>



//extern QDataStream& operator>>(QDataStream&, AppInfo&);
//extern QDataStream& operator<<(QDataStream&, AppInfo&);


class AppInfo : public QObject
{
public:
    AppInfo();
public:
    friend QDataStream& operator>>(QDataStream&, AppInfo&);
    friend QDataStream& operator<<(QDataStream&, AppInfo&);
public:
    QString  filename;
    QString  username;
    QString  password;
    QMap<int,int>   colomn_withs;
    int row_height;
    int app_with;
    int app_height;
};

#endif // APPINFO_H
