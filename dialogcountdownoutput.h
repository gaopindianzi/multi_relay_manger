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
    int   secs_escape_form_start(int index);
    int   secs_escape_form_end(int index);
    int   secs_start_to_end(int index);
    void SetAndWriteIoOutOnceTiming(int index,QTime & start,QTime & end);
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
};

#endif // DIALOGCOUNTDOWNOUTPUT_H
