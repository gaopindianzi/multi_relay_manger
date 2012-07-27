#include "appinfo.h"


AppInfo::AppInfo()
{
}

QDataStream& operator>>(QDataStream& in, AppInfo& data)
{
     in >> data.filename >> data.username >> data.password >> data.app_with >> data.app_height;
     in >> data.colomn_withs;
     in >> data.row_height;
     if(data.app_with == 0) {
         data.app_with = 770;
     }
     if(data.app_height == 0) {
         data.app_height = 336;
     }
     if(data.colomn_withs[1]  == 0) {
         data.colomn_withs[1] = 100;
     }
     if(data.colomn_withs[5]  == 0) {
         data.colomn_withs[5] = 514;
     }
     if(data.row_height == 0) {
         data.row_height = 32;
     }
    return in;
}

QDataStream& operator<<(QDataStream& out, AppInfo& data)
{
    out << data.filename << data.username << data.password << data.app_with << data.app_height;
    out << data.colomn_withs;
    out << data.row_height;
    return out;
}
