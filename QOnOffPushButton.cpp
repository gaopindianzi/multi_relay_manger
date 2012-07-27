#include "QOnOffPushButton.h"

#include <QPainter>
#include <QBrush>
#include <QMouseEvent>
#include <QColor>
#include <QRect>
#include <QSharedPointer>
#include <QImage>
#include "QRelayDeviceControl.h"
#include "mainwindow.h"
#include "debug.h"
#define THISINFO               0
#define THISERROR            1
#define THISASSERT          1

QSetOnPushButton::QSetOnPushButton(RelayDeviceSharePonterType pdev,QWidget * parent )
    : QPushButton(parent)
{
    pdevice = pdev;
}

void QSetOnPushButton::mousePressEvent ( QMouseEvent * event )
{
    if(event->button()&Qt::LeftButton) {
        QBitArray bitmsk;
        bitmsk.resize(pdevice->GetIoOutNum());
        bitmsk.fill(true,pdevice->GetIoOutNum());
        pdevice->MultiIoOutSet(0,bitmsk);
        this->update();
    }
}


QSetOFFPushButton::QSetOFFPushButton(RelayDeviceSharePonterType pdev,QWidget * parent )
    : QPushButton(parent)
{
    pdevice = pdev;
}

void QSetOFFPushButton::mousePressEvent ( QMouseEvent * event )
{
    if(event->button()&Qt::LeftButton) {
        debuginfo(("mouse precess event."));
        QBitArray bitmsk;
        bitmsk.resize(pdevice->GetIoOutNum());
        bitmsk.fill(false,pdevice->GetIoOutNum());
        pdevice->MultiIoOutSet(0,bitmsk);
        this->update();
    }
}







QSetOnPushDelegate::QSetOnPushDelegate(QObject *parent)
    : CDeviceDelegate(parent)
{
}

QWidget *QSetOnPushDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem & /* option */,
                                     const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QSetOnPushButton *pushbutton = new QSetOnPushButton(pdev,parent);
    connect(pushbutton, SIGNAL(clicked ( bool )), this, SLOT(buttonClicked(bool)));
    return pushbutton;
}

void QSetOnPushDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QSetOnPushButton *pushbutton = qobject_cast<QSetOnPushButton *>(editor);
    pushbutton->setText(tr("SetON"));
}

void QSetOnPushDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    QSetOnPushButton *pushbutton = qobject_cast<QSetOnPushButton *>(editor);
    model->setData(index, pushbutton->text());
}

void QSetOnPushDelegate::buttonClicked(bool click)
{
   // debuginfo(("set ALL ON."));
}



QSetOffPushDelegate::QSetOffPushDelegate(QObject *parent)
    : CDeviceDelegate(parent)
{
}

QWidget *QSetOffPushDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem & /* option */,
                                     const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QSetOFFPushButton *pushbutton = new QSetOFFPushButton(pdev,parent);
    connect(pushbutton, SIGNAL(clicked ( bool )), this, SLOT(buttonClicked(bool)));
    return pushbutton;
}

void QSetOffPushDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QSetOFFPushButton *pushbutton = qobject_cast<QSetOFFPushButton *>(editor);
    pushbutton->setText(tr("SetOFF"));
}

void QSetOffPushDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    QSetOFFPushButton *pushbutton = qobject_cast<QSetOFFPushButton *>(editor);
    model->setData(index, pushbutton->text());
}

void QSetOffPushDelegate::buttonClicked(bool click)
{
   // debuginfo(("set ALL OFF."));
}












QRelayValueSingalChannalButton::QRelayValueSingalChannalButton(RelayDeviceSharePonterType pdev,QWidget * parent)
    : QPushButton(parent)
{
    relay_bitmap = 0;
    pdevice = pdev;
    this->setText("");
}

void QRelayValueSingalChannalButton::paintEvent ( QPaintEvent * event )
{
    QPainter painter(this);
    QImage image;
    int num = pdevice->GetIoOutNum();
    for(int i=0;i<num;i++) {
        if(pdevice->relay_bitmask[i]) {
            image.load(":/sys/sys_icon/sources/ON.png");
        } else {
            image.load(":/sys/sys_icon/sources/OFF.png");
        }
        QRectF target(this->rect().left()+this->width()*i/num,this->rect().top(),image.width(),this->height());
        painter.drawImage(target, image);
        //写路数
        QString numstr;
        //numstr.sprintf("  %d",i+1);
       // painter.drawText(target,Qt::AlignTop|Qt::AlignLeft,numstr);
        //写名字
        //painter.drawText(target,Qt::AlignBottom|Qt::AlignLeft,"  " + pdevice->GetDeviceIoOutName(i));
        target.moveLeft(target.left()+40);
        painter.drawText(target,Qt::AlignVCenter|Qt::AlignLeft,pdevice->GetDeviceIoOutName(i));
    }
}

int QRelayValueSingalChannalButton::mouseAtButtonPosition(int x)
{
    int star = (x / (this->width() / pdevice->GetIoOutNum()));
    if (star < 0 || star > pdevice->GetIoOutNum())
        return -1;

    return star;
}

void QRelayValueSingalChannalButton::mousePressEvent ( QMouseEvent * event )
{
    if(event->button()&Qt::LeftButton) {
        int pos = event->x();
        int x = mouseAtButtonPosition(pos);
        if(x >= 0 && x < 32) relay_bitmap ^= (1<<x);
        //发送指令，翻转这个位
        pdevice->ConvertIoOutOneBitAndSendCmd(x);
        this->update();
    }
}


QRelayValueSingalChannalButtonDelegate::QRelayValueSingalChannalButtonDelegate(QObject *parent)
    : CDeviceDelegate(parent)
{
}

QWidget *QRelayValueSingalChannalButtonDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem & /* option */,
                                     const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QRelayValueSingalChannalButton *pushbutton = new QRelayValueSingalChannalButton(pdev,parent);
    connect(pushbutton, SIGNAL(clicked ( bool )), this, SLOT(buttonClicked(bool)));
    return pushbutton;
}

void QRelayValueSingalChannalButtonDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    QRelayValueSingalChannalButton *pushbutton = qobject_cast<QRelayValueSingalChannalButton *>(editor);
    pushbutton->update();
}

void QRelayValueSingalChannalButtonDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    QRelayValueSingalChannalButton *pushbutton = qobject_cast<QRelayValueSingalChannalButton *>(editor);
     index.model()->data(index) = pushbutton->relay_bitmap;
}

void QRelayValueSingalChannalButtonDelegate::buttonClicked(bool click)
{
    //QPushButton *pushbutton = qobject_cast<QPushButton *>(sender());
    //debuginfo(("on off button clicked"));
}

