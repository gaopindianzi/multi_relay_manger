#include "QCheckBoxDelegate.h"
#include <QCheckBox>
#include <QPainter>
#include "debug.h"

#define THISINFO              0
#define THISERROR            1
#define THISASSERT          1


QDeviceCheckBox::QDeviceCheckBox (RelayDeviceSharePonterType & pdev,QWidget * parent)
    : QCheckBox(parent)
{
    pdevice = pdev;
}
void	QDeviceCheckBox::mousePressEvent ( QMouseEvent * event )
{
    //QCheckBox::mousePressEvent(event);
    if(event->buttons() & Qt::LeftButton) {
        debuginfo(("left button."));
        bool check = this->isChecked();
        check = !check;
        this->setCheckState(check?Qt::Checked:Qt::Unchecked);
        pdevice->is_checked = check;
    }
    //debuginfo(("mouse precess...%s",this->isChecked()?"true":"fasle"));
}

void QDeviceCheckBox::paintEvent ( QPaintEvent * event )
{
    this->setChecked(pdevice->is_checked);
    QCheckBox::paintEvent(event);
}

QCheckBoxDelegate::QCheckBoxDelegate(QObject *parent)
    : CDeviceDelegate(parent)
{
}

void QCheckBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    //Ïû³ýÏÔÊ¾
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QRectF rect = option.rect;
    QColor color;
    QString indexstr;
    indexstr.sprintf("    %d",pdev->index);
    painter->save();
    painter->eraseRect(rect);
    painter->setPen(color);
    painter->drawText(rect,Qt::AlignVCenter|Qt::AlignLeft,indexstr);
    painter->restore();
}

QWidget *QCheckBoxDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &  option ,
                                     const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QDeviceCheckBox * edit = new QDeviceCheckBox(pdev,parent);
    //connect(edit,SIGNAL(clicked(bool)),this,SLOT(buttonClicked(bool)));
    edit->setCheckState(pdev->is_checked?Qt::Checked:Qt::Unchecked);
    //edit->move(5,0);
    edit->setText("");
    return edit;
}



void QCheckBoxDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QDeviceCheckBox *edit = qobject_cast<QDeviceCheckBox *>(editor);
   // edit->move(5,0);
    edit->setText("");
    edit->setCheckState(pdev->is_checked?Qt::Checked:Qt::Unchecked);
}

void QCheckBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    //QDeviceCheckBox *edit = qobject_cast<QDeviceCheckBox *>(editor);
    debuginfo(("check box set mode data."));
    //pdev->is_checked = edit->isChecked();
    model->setData(index, qVariantFromValue(pdev));
}

void QCheckBoxDelegate::buttonClicked(bool click)
{
}



