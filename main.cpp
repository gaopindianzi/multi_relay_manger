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
    sema.acquire();   // ���ٽ������������ڴ�   SharedMemory
    QSharedMemory mem("SystemObject");// ȫ�ֶ�����
    if (!mem.create(1))// ���ȫ�ֶ����Դ������˳�
    {
        QMessageBox box(QMessageBox::Warning,QObject::tr("Warning"),
                        QObject::tr("The program has already been running!"));
        box.exec();
        sema.release();// ����� Unix ϵͳ�����Զ��ͷš�
        return 0;
    }

    sema.release();// �ٽ���


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

    w.sleep(0);
    w.show();
    //w.showMaximized();
    splash->finish(&w);

    return a.exec();

    delete splash;

    return 0;
}
