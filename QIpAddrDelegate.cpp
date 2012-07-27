#include "QIpAddrDelegate.h"
#include <QLineEdit>
#include <QLabel>


#include "debug.h"

#define THISINFO               0
#define THISERROR            1
#define THISASSERT          1


QIpAddressDelegate::QIpAddressDelegate(QObject *parent)
    : CDeviceDelegate(parent)
{
}

QWidget *QIpAddressDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem & option,
                                     const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QLabel * edit = new QLabel(parent);
    edit->setText(pdev->GetDeviceAddress());
    return edit;
}

void QIpAddressDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    QLabel *edit = qobject_cast<QLabel *>(editor);
    edit->setText(pdev->GetDeviceAddress());
}

void QIpAddressDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(index.data());
    //model->setData(index, qVariantFromValue(pdev));
}

void QIpAddressDelegate::buttonClicked(bool click)
{
}
