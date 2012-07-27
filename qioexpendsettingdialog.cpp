#include "qioexpendsettingdialog.h"
#include "ui_qioexpendsettingdialog.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>
#include <QRect>




QMyIoOutButton::QMyIoOutButton(RelayDeviceSharePonterType pdev, int index,QWidget * parent)
  : QPushButton(parent)
{
    pdevice = pdev;
    this->button_index = index;
    clock_image.load(":/sys/sys_icon/sources/alarm_clock.png");
    spaceing = 6;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(tcp_timer()));
}

void QMyIoOutButton::paintEvent ( QPaintEvent * event )
{
    QPainter painter(this);

    int num = pdevice->GetIoOutNum();
    if(button_index < num) {
        QString str;
        str.sprintf("%02d",button_index+1);
        str = ":/sys/sys_icon/sources/" + str + ".png";
        imagenum.load(str);
        if(pdevice->relay_bitmask[button_index]) {
            image.load(":/sys/sys_icon/sources/on_h32.png");
        } else {
            image.load(":/sys/sys_icon/sources/OFF_h32.png");
        }
        //绘图
        this->resize(imagenum.width()+image.width()+clock_image.width()+spaceing*2,image.height());
        QRectF target1(this->rect().left(),this->rect().top(),imagenum.width(),this->height());
        painter.drawImage(target1, imagenum);
        QRectF target2(this->rect().left()+imagenum.width()+spaceing,this->rect().top(),image.width(),this->height());
        painter.drawImage(target2, image);

        if(pdevice->GetTimingNodeNum(button_index) > 0) {
            QRectF target3(this->rect().left()+imagenum.width()+target2.width()+spaceing+3,this->rect().top(),clock_image.width(),clock_image.height());
            painter.drawImage(target3, clock_image);
        }
        //写名字
        target2.moveLeft(target2.left()+40);
        painter.drawText(target2,Qt::AlignVCenter|Qt::AlignLeft,pdevice->GetDeviceIoOutName(button_index));
    }
}

void QMyIoOutButton::mousePressEvent ( QMouseEvent * event )
{
    if(event->button()&Qt::LeftButton) {
        int x = event->x();
        if(x >= imagenum.width()+spaceing && x <= imagenum.width()+spaceing+image.width()) {
            pdevice->ConvertIoOutOneBitAndSendCmd(button_index);
            timer->start(100);
            timer_count = 0;
        }
    }
}

void QMyIoOutButton::tcp_timer()
{
    if(timer_count < 5) {
        timer_count++;
        this->repaint();
    } else {
        timer->setInterval(500);
        this->repaint();
    }
}


QIoExpendSettingDialog::QIoExpendSettingDialog(RelayDeviceSharePonterType pdev,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QIoExpendSettingDialog)
{
    ui->setupUi(this);    
    //根据布局绘制IO输出控件

    pdevice = pdev;

   QVBoxLayout * layout_left   = new QVBoxLayout();
   QVBoxLayout * layout_right = new QVBoxLayout();

   //根据不同的路数，设定不同数量和模式的按钮
   int ioout_num = pdev->GetIoOutNum();
   if(ioout_num  >= 2 && ioout_num <= 32) {
       int i;
       for(i=0;i<ioout_num/2;i++) {
           QMyIoOutButton * pbleft = new QMyIoOutButton(pdevice,i+0);
           QMyIoOutButton * pbright = new QMyIoOutButton(pdevice,i+ioout_num/2);
           layout_left->addWidget(pbleft,0);
           layout_right->addWidget(pbright,0);
           layout_left->addSpacing(10);
           layout_right->addSpacing(10);
       }
       layout_left->addStretch();
       layout_right->addStretch();
       //调整对话框大小
       this->setFixedSize(420,(ioout_num/2)*(32+6) + 50);
       ui->groupBoxIoButtons->setFixedSize(this->width()-20,this->height()-20);
   }

   QString str;
   str.sprintf("%d",ioout_num);

   str = tr("Relay Output : ") + str + tr(" channel");
   ui->groupBoxIoButtons->setTitle(str);
   ui->gridLayout->addLayout(layout_left,0,0);
   ui->gridLayout->addLayout(layout_right,0,1);
}

QIoExpendSettingDialog::~QIoExpendSettingDialog()
{
    delete ui;
}
