#include <QtGui/QApplication>
#include <QMainWindow>
#include <QTextCodec>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QString>
#include <QWidget>
#include <QObject>
#include <QTranslator>
#include <QSplashScreen>
#include "mainwindow.h"


QTranslator appTranslator;
QTranslator qtTranslator;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    appTranslator.load(":/language/remotectlmultemgr_zh.qm");
    a.installTranslator(&appTranslator);



    QSystemSemaphore sema("JAMKey",1,QSystemSemaphore::Open);
    sema.acquire();   // 在临界区操作共享内存   SharedMemory
    QSharedMemory mem("SystemObject");// 全局对象名
    if (!mem.create(1))// 如果全局对象以存在则退出
    {
        QMessageBox box(QMessageBox::Warning,QObject::tr("Warning"),
                        QObject::tr("The program has already been running!"));
        box.exec();
        sema.release();// 如果是 Unix 系统，会自动释放。
        return 0;
    }

    sema.release();// 临界区


    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }





    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    MainWindow w;


    QSplashScreen * splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/sys/sys_icon/sources/start.png"));
    splash->show();

    w.sleep(3);
    w.show();
    splash->finish(&w);

    return a.exec();

    delete splash;
}
