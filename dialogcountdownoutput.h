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
private:
    void SetRemainTimeAndProgressBar(int index,QDateTime start,QDateTime current,QDateTime end,bool isfirstone = 0);
private:
    Ui::DialogCountdownOutput *ui;

    RelayDeviceSharePonterType pdevice;

    QTimer * timer;
    QMap<int,QTimeEdit *>      timemap;
    QMap<int,QTimeEdit *>      timenewmap;
    QMap<int,QProgressBar *> progressmap;
    //����ʱ��ʱ��
    QMap<int,timing_node>      count_down_timemap;
    //
    QTime                                dialogtime;  //�򿪶Ի����ʱ���ʱ�䣬���ڵ���ʱ��ʾ
private slots:
    void timertick(void);
    void timerclick(void);
    void applayclick(bool);
    void rtc_write_finished(void);
    void timing_write_finished(void);
};

#endif // DIALOGCOUNTDOWNOUTPUT_H
