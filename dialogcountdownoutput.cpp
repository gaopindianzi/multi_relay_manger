#include "dialogcountdownoutput.h"
#include "ui_dialogcountdownoutput.h"


#include "appinfo.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>
#include <QRect>
#include <QLineEdit>
#include <QTextEdit>
#include <QProgressBar>
#include <QTimeEdit>
#include <QLabel>
#include <QDate>



QApplyPushbutton::QApplyPushbutton(RelayDeviceSharePonterType pdev,int index,QWidget *parent)
    : QPushButton(parent)
{
    dev_index = index;
}

QApplyPushbutton::~QApplyPushbutton()
{
}

DialogCountdownOutput::DialogCountdownOutput(RelayDeviceSharePonterType pdev,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCountdownOutput)
{
    ui->setupUi(this);
    pdevice = pdev;
    //构建

    dialogtime = QTime::currentTime();

    //根据不同的路数，设定不同数量和模式的按钮
    int ioout_num = pdev->GetIoOutNum();

    //初始化定时器序列
    QVector<timing_node> & timlist = pdev->GetDeviceIoOutTimingList();


    for(int i=0;i<ioout_num;i++) {
        timing_node node;
        memset(&node,0,sizeof(node));
        count_down_timemap[i] = node;
    }

    for(int i=0;i<timlist.size();i++) {
        timing_node node = timlist[i];
        int addr = node.addr[1] * 256 + node.addr[0];
        if(addr < ioout_num) {
            count_down_timemap[addr] = node;
        }
    }

    QHBoxLayout * linelayout = new QHBoxLayout();
    QLineEdit * labelname = new QLineEdit(tr("name"),parent);
    labelname->setFixedWidth(100);
    linelayout->addWidget(labelname);
    linelayout->addSpacing(6);
    QLineEdit * labelpragressbar = new QLineEdit(tr("The remainder of the progress bar"),parent);
    labelpragressbar->setFixedWidth(271);
    linelayout->addWidget(labelpragressbar);
    linelayout->addSpacing(35);
    QLineEdit * labelesace = new QLineEdit(tr("Remaining"),parent);
    labelesace->setFixedWidth(84);
    linelayout->addWidget(labelesace);
    linelayout->addSpacing(6);
    QLineEdit * labelnewtime = new QLineEdit(tr("New Time"),parent);
    labelnewtime->setFixedWidth(84);
    linelayout->addWidget(labelnewtime);
    linelayout->addSpacing(7);
    QLineEdit * labelappaybutton = new QLineEdit(tr("Operation"),parent);
    labelappaybutton->setFixedWidth(73);
    linelayout->addWidget(labelappaybutton);
    linelayout->addStretch();

    ui->verticalLayout->addLayout(linelayout);

    if(ioout_num  >= 2 && ioout_num <= 32) {
        int i;
        for(i=0;i<ioout_num;i++) {
            QHBoxLayout * linelayout = new QHBoxLayout();
            QLineEdit * name = new QLineEdit(parent);
            QProgressBar * progbar = new QProgressBar(parent);
            progbar->setFixedWidth(300);
            progressmap[i] = progbar;
            QTimeEdit * time = new QTimeEdit(parent);
            QTimeEdit * timenew = new QTimeEdit(parent);
            time->setReadOnly(true);

            {
                //设置进度条，和剩余时间，和总时间
                QTime base(0,0,0);
                int formstart = secs_escape_form_start(i);
                int formend = secs_escape_form_end(i);
                int startend = secs_start_to_end(i);

                qDebug("formstart=%d,formend=%d,startend=%d",formstart,formend,startend);

                timenew->setTime(base.addSecs(startend));

                progbar->setRange(0,100);
                if(formstart > 0 && formend < 0) {
                    time->setTime(base.addSecs(formend *(-1)));
                    int persent = formend * -100 / startend;
                    progbar->setValue(persent);
                } else {
                    //over time...
                    time->setTime(base);
                    progbar->setValue(0);
                }
            }


            timemap[i] = time;
            timenewmap[i] = timenew;

            QApplyPushbutton * okbutton = new QApplyPushbutton(pdevice,i,parent);
            connect(okbutton,SIGNAL(clicked(bool)),this,SLOT(applayclick(bool)));



            okbutton->setText(tr("Apply"));

            name->setFixedWidth(100);
            QPalette pa = name->palette();
            pa.setColor(QPalette::Base,QColor(135,206,235));
            name->setPalette(pa);
            name->setText(pdev->GetDeviceIoOutName(i));
            linelayout->addWidget(name);
            linelayout->addSpacing(6);
            linelayout->addWidget(progbar);
            linelayout->addSpacing(6);
            linelayout->addWidget(time);
            linelayout->addSpacing(6);
            linelayout->addWidget(timenew);
            linelayout->addSpacing(6);
            linelayout->addWidget(okbutton);
            linelayout->addSpacing(6);
            linelayout->addStretch();
            ui->verticalLayout->addLayout(linelayout);
            ui->verticalLayout->addSpacing(5);
        }
        ui->verticalLayout->addStretch();

        timer = new QTimer(parent);
        timer->start(1000);
        connect(timer,SIGNAL(timeout()),this,SLOT(timertick()));
    }
}

DialogCountdownOutput::~DialogCountdownOutput()
{
    delete ui;
}

int   get_seconds_form_timnode(time_type & node)
{
    int  secs = 0;
    secs = node.hour * 3600;
    secs += node.min * 60;
    secs += node.sec;
    return secs;
}

void DialogCountdownOutput::SetAndWriteIoOutOnceTiming(int index,QTime & start,QTime & end)
{
    QDate date = QDate::currentDate();
    timing_node node;
    node.addr[1] = index / 256;
    node.addr[0] = index % 256;
    node.start_time.year= date.year() - 1900;
    node.start_time.mon = date.month() - 1;
    node.start_time.day  = date.day();
    node.start_time.hour = start.hour();
    node.start_time.min = start.minute();
    node.start_time.sec = start.second();

    node.end_time.year= date.year() - 1900;
    node.end_time.mon = date.month() - 1;
    node.end_time.day  = date.day();
    node.end_time.hour = end.hour();
    node.end_time.min = end.minute();
    node.end_time.sec = end.second();


    SET_IO_TIME_DONE(&node,0);
    SET_IO_TIME_VALID(&node,1);
    SET_IO_TIME_SUBCYCLEON(&node,0);
    SET_IO_TIME_CYCLE_TYPE(&node,CYCLE_ONCE);
    count_down_timemap[index] = node;

    //生成定时器列表，输入到设备中
    //调用写定时器
}

void DialogCountdownOutput::applayclick(bool)
{
    QApplyPushbutton * buttun = (QApplyPushbutton *)sender();
    int index = buttun->dev_index;
    qDebug("sender button = %d",index);

    QTime time = timenewmap[index]->time();

    //app_info.setCountDownDefaultTime(index,time);

    QTime base(0,0,0);
    int secs = time.secsTo(base);

    QTime curr = QTime::currentTime();

    QTime start = curr;
    QTime end = curr.addSecs(secs);

    //构建设备结构
    SetAndWriteIoOutOnceTiming(index,start,end);

}


int   DialogCountdownOutput::secs_escape_form_start(int index)
{
    int curr_secs = pdevice->GetDeviceTimeSecs();
    timing_node node = count_down_timemap[index];
    time_type    start = node.start_time;
    return (curr_secs - get_seconds_form_timnode(start));
}
int   DialogCountdownOutput::secs_escape_form_end(int index)
{
    int curr_secs = pdevice->GetDeviceTimeSecs();
    timing_node node = count_down_timemap[index];
    time_type    end   = node.end_time;
    return (curr_secs - get_seconds_form_timnode(end));
}
int   DialogCountdownOutput::secs_start_to_end(int index)
{
    timing_node node = count_down_timemap[index];
    time_type    start = node.start_time;
    time_type    end   = node.end_time;
    return (get_seconds_form_timnode(end) - get_seconds_form_timnode(start));
}

void DialogCountdownOutput::timertick(void)
{
//     qDebug("time tick;");
     for(int i=0;i<timemap.size();i++) {
        // QTimeEdit * time = timemap[i];
        //// if(time->time() !=QTime(0,0,0)) {
        //     QTime t = time->time().addSecs(-1);
        //     time->setTime(t);
         //}
     }
}

void DialogCountdownOutput::timerclick(void)
{
    //qDebug("time click");
}


