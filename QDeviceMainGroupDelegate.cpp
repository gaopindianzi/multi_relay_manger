#include "QDeviceMainGroupDelegate.h"
#include <QLineEdit>
#include <QPainter>
#include <QPoint>
#include <QRect>

#include "debug.h"

#define THISINFO                0
#define THISERROR            1
#define THISASSERT          1



QDeviceNameDelegate::QDeviceNameDelegate(QObject *parent)
    : CDeviceDelegate(parent)
{
}

QWidget *QDeviceNameDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem & option,
                                     const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QLineEdit * edit = new QLineEdit(parent);
    edit->setText(pdev->GetDeviceName());
    return edit;
}

void QDeviceNameDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QRectF rect = option.rect;
    painter->drawText(rect,Qt::AlignVCenter|Qt::AlignLeft,pdev->GetDeviceName());
}


void QDeviceNameDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    edit->setText(pdev->GetDeviceName());
}

void QDeviceNameDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    //debuginfo(("set device name:%s",edit->text().toAscii().data()));
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    pdev->SetDeviceName(edit->text());
   // model->setData(index, qVariantFromValue(pdev));
}

void QDeviceNameDelegate::buttonClicked(bool click)
{
}










QDeviceMainGroupDelegate::QDeviceMainGroupDelegate(QObject *parent)
    : CDeviceDelegate(parent)
{
}

QWidget *QDeviceMainGroupDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem & option,
                                     const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QLineEdit * edit = new QLineEdit(parent);
    edit->setText(pdev->GetGroup1Name());
    return edit;
}

void QDeviceMainGroupDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    edit->setText(pdev->GetGroup1Name());
}

void QDeviceMainGroupDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    pdev->SetGroup1Name(edit->text());
    //model->setData(index, qVariantFromValue(pdev));
}
void QDeviceMainGroupDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QRectF rect = option.rect;
    painter->drawText(rect,Qt::AlignVCenter|Qt::AlignLeft,pdev->GetGroup1Name());
}
void QDeviceMainGroupDelegate::buttonClicked(bool click)
{
}














QDeviceSlaveGroupDelegate::QDeviceSlaveGroupDelegate(QObject *parent)
    : CDeviceDelegate(parent)
{
}

QWidget *QDeviceSlaveGroupDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem & option,
                                     const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QLineEdit * edit = new QLineEdit(parent);
    edit->setText(pdev->GetGroup2Name());
    return edit;
}

void QDeviceSlaveGroupDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    edit->setText(pdev->GetGroup2Name());
}

void QDeviceSlaveGroupDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    pdev->SetGroup2Name(edit->text());
    model->setData(index, qVariantFromValue(pdev));
}
void QDeviceSlaveGroupDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QRectF rect = option.rect;
    painter->drawText(rect,Qt::AlignVCenter|Qt::AlignLeft,pdev->GetGroup2Name());
}
void QDeviceSlaveGroupDelegate::buttonClicked(bool click)
{
}
