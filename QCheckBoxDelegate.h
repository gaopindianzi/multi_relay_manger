#ifndef QCHECKBOXDELEGATE_H
#define QCHECKBOXDELEGATE_H

#include <QItemDelegate>
#include <QWidget>
#include <QModelIndex>
#include <QCheckBox>
#include <QTableWidgetItem>
#include <QMouseEvent>
#include "CDeviceDelegate.h"
#include "QRelayDeviceControl.h"


class QDeviceCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    QDeviceCheckBox (RelayDeviceSharePonterType & pdev, QWidget * parent = 0 );
protected:
    void	mousePressEvent ( QMouseEvent * event );
    void paintEvent ( QPaintEvent * event );
private:
    RelayDeviceSharePonterType pdevice;
    //void paintEvent ( QPaintEvent * event );
};

class QCheckBoxDelegate : public CDeviceDelegate
{
    Q_OBJECT

public:
    QCheckBoxDelegate(QObject *parent = 0);
private:
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




#endif // QCHECKBOXDELEGATE_H
