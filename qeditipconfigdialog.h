#ifndef QEDITIPCONFIGDIALOG_H
#define QEDITIPCONFIGDIALOG_H

#include <QDialog>
#include <QAbstractItemModel>
#include <QListWidget>
#include "QRelayDeviceControl.h"

namespace Ui {
class QEditIpConfigDialog;
}

class QEditIpConfigDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QEditIpConfigDialog(QWidget *parent = 0);
    ~QEditIpConfigDialog();
public:
    void CreateDeviceList(void);
    void InsertDevice(RelayDeviceSharePonterType & pdev);
protected slots:
    void	itemClicked ( QListWidgetItem * item );
protected slots:
    void ReadClicked(bool);
    void WriteClicked(bool);
    void ResetClicked(bool);

private:
    Ui::QEditIpConfigDialog *ui;
    QListWidget * list_devices;
};

#endif // QEDITIPCONFIGDIALOG_H
