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
#include <QDateTime>

#include "waitfinisheddialog.h"


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

    connect(pdevice.data(),SIGNAL(DevcieWriteRtcFinihed()),this,SLOT(rtc_write_finished()));
    connect(pdevice.data(),SIGNAL(DeviceWriteTimingFinished()),this,SLOT(timing_write_finished()));
    connect(pdevice.data(),SIGNAL(DeviceWriteIoNameFinished(int,QString)),
            this,SLOT(ioname_write_finished(int,QString)));

    dialogtime = QTime::currentTime();

    //根据不同的路数，设定不同数量和模式的按钮
    int ioout_num = pdev->GetIoOutNum();



    this->setFixedHeight(40+(ioout_num * 34 + 22));  //定参数外框+IO数量高度
    this->ui->groupBox->setFixedHeight(15+(ioout_num * 34 + 22));
    this->setFixedWidth(730);
    this->ui->groupBox->setFixedWidth(715);


    //初始化定时器序列
    QVector<timing_node> & timlist = pdev->GetDeviceIoOutTimingList();


    for(int i=0;i<ioout_num;i++) {
        timing_node node = {{0,0},0,{0,0,0,0,0,0},{0,0,0,0,0,0},0,0,0};
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
    QLabel * labelname = new QLabel(tr("The Name"),parent);
    labelname->setFixedWidth(100);
    linelayout->addWidget(labelname);
    linelayout->addSpacing(6);
    QLabel * labelpragressbar = new QLabel(tr("Countdown time remaining progress bar"),parent);
    labelpragressbar->setFixedWidth(271);
    linelayout->addWidget(labelpragressbar);
    linelayout->addSpacing(35);
    QLabel * labelesace = new QLabel(tr("Time remaining"),parent);
    labelesace->setFixedWidth(84);
    linelayout->addWidget(labelesace);
    linelayout->addSpacing(6);
    QLabel * labelnewtime = new QLabel(tr("Total time"),parent);
    labelnewtime->setFixedWidth(84);
    linelayout->addWidget(labelnewtime);
    linelayout->addSpacing(7);
    QLabel * labelappaybutton = new QLabel(tr("Reset operation"),parent);
    labelappaybutton->setFixedWidth(73);
    linelayout->addWidget(labelappaybutton);
    linelayout->addStretch();

    ui->verticalLayout->addLayout(linelayout);

    if(ioout_num  >= 2 && ioout_num <= 32) {
        int i;
        for(i=0;i<ioout_num;i++) {
            QHBoxLayout * linelayout = new QHBoxLayout();
            QLineEdit * name = new QLineEdit(parent);
            namemap[i] = name;
            QProgressBar * progbar = new QProgressBar(parent);
            progbar->setFixedWidth(300);
            progressmap[i] = progbar;
            QTimeEdit * time = new QTimeEdit(parent);
            QTimeEdit * timenew = new QTimeEdit(parent);
            timemap[i] = time;
            timenewmap[i] = timenew;
            time->setReadOnly(true);

            {
                //设置进度条，和剩余时间，和总时间
                QDateTime starttime = pdevice->ConvertTimeNodeToQDT(count_down_timemap[i].start_time);
                QDateTime devicetime = pdevice->GetDeviceDateTime();
                QDateTime endtime   = pdevice->ConvertTimeNodeToQDT(count_down_timemap[i].end_time);

                SetRemainTimeAndProgressBar(i,starttime,devicetime,endtime,1);

            }



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

void DialogCountdownOutput::SetAndWriteIoOutOnceTiming(int index,QDateTime & start,QDateTime & end)
{
    timing_node node = {{0,0},0,{0,0,0,0,0,0},{0,0,0,0,0,0},0,0,0};
    node.addr[1] = index / 256;
    node.addr[0] = index % 256;
    node.start_time.year= start.date().year() - 1900;
    node.start_time.mon = start.date().month() - 1;
    node.start_time.day  = start.date().day();
    node.start_time.hour = start.time().hour();
    node.start_time.min = start.time().minute();
    node.start_time.sec = start.time().second();

    node.end_time.year= end.date().year() - 1900;
    node.end_time.mon = end.date().month() - 1;
    node.end_time.day  = end.date().day();
    node.end_time.hour = end.time().hour();
    node.end_time.min = end.time().minute();
    node.end_time.sec = end.time().second();




    SET_IO_TIME_DONE(&node,0);
    SET_IO_TIME_VALID(&node,1);
    SET_IO_TIME_SUBCYCLEON(&node,0);
    SET_IO_TIME_CYCLE_TYPE(&node,CYCLE_ONCE);
    count_down_timemap[index] = node;

    //生成定时器列表，输入到设备中
    QVector<timing_node> timinglist;
    for(int i=0;i<pdevice->GetIoOutNum();i++) {
        timing_node node = count_down_timemap[i];
        if(1) {  //GET_IO_TIME_VALID(&node)) {  //不管定时器是否有效，必须全部填加进去
            timinglist.push_back(node);
        }
    }
    pdevice->SetDeviceIoOutTimingList(timinglist);
    //调用写定时器
    pdevice->TcpStartWriteRtc();
    //pdevice->TcpStartWriteIoOutTiming();

    WaitFinishedDialog dlg;
    dlg.setprogresstitle(tr("Is writing a timer, please wait......"));
    connect(this,SIGNAL(write_some_data_finished()),&dlg,SLOT(progressfinished()));
    dlg.exec();

}

void DialogCountdownOutput::applayclick(bool)
{
    QApplyPushbutton * buttun = (QApplyPushbutton *)sender();
    int index = buttun->dev_index;
    //qDebug("sender button = %d",index);

    QDateTime datetimebase = QDateTime::currentDateTime();

    QTime tbase(0,0,0);
    int secs = tbase.secsTo(timenewmap[index]->time());  //求得定时秒数
    QDateTime enddt = datetimebase.addSecs(secs);

    index_name_change = index;
    index_name_string = namemap[index]->text();

    SetAndWriteIoOutOnceTiming(index,datetimebase,enddt);

}

void DialogCountdownOutput::rtc_write_finished(void)
{
    pdevice->TcpStartWriteIoOutTiming();
}

void DialogCountdownOutput::timing_write_finished(void)
{
    unsigned char addr[2];
    addr[0] = index_name_change & 0xFF;
    addr[1] = index_name_change >> 8;
    pdevice->TcpWriteIoName(addr,index_name_string);
}

void DialogCountdownOutput::ioname_write_finished(int index,QString name)
{
    QString devicename = pdevice->GetDeviceIoOutName(index);
    if(index < pdevice->GetIoOutNum()) {
        namemap[index]->setText(devicename);
    }
    emit write_some_data_finished();
}

void DialogCountdownOutput::SetRemainTimeAndProgressBar(int index,QDateTime start,QDateTime current,QDateTime end,bool isfirstone)
{
    //设置进度条，和剩余时间，和总时间
    QTime basetime(0,0,0);
    int all_secs = start.secsTo(end);
    if(isfirstone) {
        timenewmap[index]->setTime(basetime.addSecs(all_secs));
    }

    int   start2now_secs = start.secsTo(current);
    int   end2now_secs = end.secsTo(current);
    //qDebug("%d:start to end %d , start %d ,end %d",index,start.secsTo(end),start2now_secs,end2now_secs);
    progressmap[index]->setRange(0,100);
    if(start2now_secs > 0 && end2now_secs < 0 && pdevice->relay_bitmask[index]) {
        timemap[index]->setTime(basetime.addSecs(end2now_secs *(-1)));
        int persent = end2now_secs * -100 / all_secs;
        progressmap[index]->setValue(persent);
    } else {
        timemap[index]->setTime(basetime);
        progressmap[index]->setValue(0);
    }
}


void DialogCountdownOutput::timertick(void)
{

     for(int i=0;i<timemap.size();i++) {
         //设置进度条，和剩余时间，和总时间
         QDateTime starttime = pdevice->ConvertTimeNodeToQDT(count_down_timemap[i].start_time);
         QDateTime currenttime = QDateTime::currentDateTime(); //pdevice->GetDeviceDateTime();
         QDateTime endtime   = pdevice->ConvertTimeNodeToQDT(count_down_timemap[i].end_time);
         SetRemainTimeAndProgressBar(i,starttime,currenttime,endtime);
     }

}

void DialogCountdownOutput::timerclick(void)
{
    //qDebug("time click");
}


