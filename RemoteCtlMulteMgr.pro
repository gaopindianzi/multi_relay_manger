#-------------------------------------------------
#
# Project created by QtCreator 2012-05-09T12:10:05
#
#-------------------------------------------------

QT       += core gui network

TARGET = RemoteCtlMulteMgr
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    QOnOffPushButton.cpp \
    QRelayDeviceControl.cpp \
    QCheckBoxDelegate.cpp \
    CDeviceDelegate.cpp \
    CDeviceTableWidgetItem.cpp \
    QDeviceMainGroupDelegate.cpp \
    QIpAddrDelegate.cpp \
    QDeviceStatusDelegate.cpp \
    editparamdialog.cpp \
    QDeviceList.cpp \
    qeditipconfigdialog.cpp \
    qpasswordmangerdialog.cpp \
    rc4.cpp \
    qioexpendsettingdialog.cpp \
    AboutDialog.cpp

HEADERS  += mainwindow.h \
    debug.h \
    QOnOffPushButton.h \
    QRelayDeviceControl.h \
    multimgr_device_dev.h \
    QCheckBoxDelegate.h \
    CDeviceDelegate.h \
    CDeviceTableWidgetItem.h \
    QDeviceMainGroupDelegate.h \
    QIpAddrDelegate.h \
    QDeviceStatusDelegate.h \
    modbus_interface.h \
    editparamdialog.h \
    QDeviceList.h \
    qeditipconfigdialog.h \
    qpasswordmangerdialog.h \
    rc4.h \
    PasswordItemDef.h \
    qioexpendsettingdialog.h \
    AboutDialog.h \
    command_datatype.h

FORMS    += mainwindow.ui \
    editparamdialog.ui \
    qeditipconfigdialog.ui \
    qpasswordmangerdialog.ui \
    qioexpendsettingdialog.ui \
    AboutDialog.ui

RC_FILE = myapp.rc

OTHER_FILES += \
    myapp.rc \
    remotectlmultemgr_zh.qm \
    sources/ON.png \
    sources/OFF.png

RESOURCES += \
    resource_files.qrc
