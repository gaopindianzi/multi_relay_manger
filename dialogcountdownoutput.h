#ifndef DIALOGCOUNTDOWNOUTPUT_H
#define DIALOGCOUNTDOWNOUTPUT_H

#include <QDialog>
#include <QSharedPointer>
#include <QPushButton>
#include <QTimer>
#include "multimgr_device_dev.h"
#include "QRelayDeviceControl.h"
#include <QTimer>
#include <QVector>
#include <QMap>
#include <QTimeEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QTime>
#include "command_datatype.h"

namespace Ui {
class DialogCountdownOutput;
}

class QApplyPushbutton : public QPushButton
{
    Q_OBJECT

public:
    QApplyPushbutton(RelayDeviceSharePonterType pdev,int index,QWidget *parent = 0);
    virtual ~QApplyPushbutton();
public:
    int dev_index;
};

class DialogCountdownOutput : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogCountdownOutput(RelayDeviceSharePonterType pdev,QWidget *parent = 0);
    ~DialogCountdownOutput();
public:
    void SetAndWriteIoOutOnceTiming(int index,QDateTime & start,QDateTime & end);
signals:
    void write_some_data_finished(void);
private:
    void SetRemainTimeAndProgressBar(int index,QDateTime start,QDateTime current,QDateTime end,bool isfirstone = 0);
private:
    Ui::DialogCountdownOutput *ui;

    RelayDeviceSharePonterType pdevice;

    QTimer * timer;
    QMap<int,QLineEdit *>       namemap;
    QMap<int,QTimeEdit *>      timemap;
    QMap<int,QTimeEdit *>      timenewmap;
    QMap<int,QProgressBar *> progressmap;
    //倒计时定时器
    QMap<int,timing_node>      count_down_timemap;
    //
    QTime                                dialogtime;  //打开对话框的时候的时间，用于倒计时显示
    unsigned int                       index_name_change;
    QString                              index_name_string;
private slots:
    void timertick(void);
    void timerclick(void);
    void applayclick(bool);
    void rtc_write_finished(void);
    void timing_write_finished(void);
    void ioname_write_finished(int index,QString name);
};

#endif // DIALOGCOUNTDOWNOUTPUT_H
