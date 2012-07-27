#ifndef DEBUG_H
#define DEBUG_H
#include <QDebug>



#define debuginfo(arg)   if(THISINFO)do{ qDebug arg ; }while(0)
#define debugerror(arg)   if(THISERROR)do{ qDebug arg ; }while(0)

#endif // DEBUG_H
