#ifndef QDEVICEMAINGROUPDELEGATE_H
#define QDEVICEMAINGROUPDELEGATE_H
#include <QItemDelegate>
#include <QWidget>
#include <QSharedPointer>
#include "multimgr_device_dev.h"
#include "QRelayDeviceControl.h"
#include "CDeviceDelegate.h"



class QDeviceNameDelegate : public CDeviceDelegate
{
    Q_OBJECT

public:
    QDeviceNameDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
                             const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

//! [1] //! [2]
private slots:
    void buttonClicked ( bool );
};

class QDeviceMainGroupDelegate : public CDeviceDelegate
{
    Q_OBJECT

public:
    QDeviceMainGroupDelegate(QObject *parent = 0);
//! [0]

//! [1]
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
                             const QModelIndex &index) const;
//! [1] //! [2]
private slots:
    void buttonClicked ( bool );
};


class QDeviceSlaveGroupDelegate : public CDeviceDelegate
{
    Q_OBJECT

public:
    QDeviceSlaveGroupDelegate(QObject *parent = 0);
//! [0]

//! [1]
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
                             const QModelIndex &index) const;
//! [1] //! [2]
private slots:
    void buttonClicked ( bool );
};


#endif // QDEVICEMAINGROUPDELEGATE_H
