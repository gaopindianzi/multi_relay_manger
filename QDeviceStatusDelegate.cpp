#include "QDeviceStatusDelegate.h"
#include <QLabel>
#include <QPainter>

#include "debug.h"

#define THISINFO                0
#define THISERROR            1
#define THISASSERT          1

QDeviceStatusDelegate::QDeviceStatusDelegate(QObject *parent)
    : CDeviceDelegate(parent)
{
}

QWidget *QDeviceStatusDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem & option,
                                     const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QLabel * edit = new QLabel(parent);
    edit->setText(pdev->GetStatus());
    return edit;
}

void QDeviceStatusDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QLabel *edit = qobject_cast<QLabel *>(editor);
    edit->setText(pdev->GetStatus());
}
void QDeviceStatusDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QRectF rect = option.rect;
    QColor color;
    if(pdev->is_online) {
        color.setRgb(0,100,0);
    } else {
        color.setRgb(255,0,0);
    }
    painter->save();
    painter->eraseRect(rect);
    painter->setPen(color);
    painter->drawText(rect,Qt::AlignVCenter|Qt::AlignLeft,pdev->GetStatus());
    painter->restore();
}
void QDeviceStatusDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    model->setData(index, qVariantFromValue(pdev));
}

void QDeviceStatusDelegate::buttonClicked(bool click)
{
}










QDeviceTimeDelegate::QDeviceTimeDelegate(QObject *parent)
    : CDeviceDelegate(parent)
{
}

QWidget *QDeviceTimeDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem & option,
                                     const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QLabel * edit = new QLabel(parent);
    edit->setText(pdev->GetDeviceTime());
    return edit;
}
void QDeviceTimeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QRectF rect = option.rect;
    painter->drawText(rect,Qt::AlignVCenter|Qt::AlignLeft,pdev->GetDeviceTime());
}

void QDeviceTimeDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QLabel *edit = qobject_cast<QLabel *>(editor);
    edit->setText(pdev->GetDeviceTime());
}

void QDeviceTimeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    model->setData(index, qVariantFromValue(pdev));
}

void QDeviceTimeDelegate::buttonClicked(bool click)
{
}

