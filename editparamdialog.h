#ifndef EDITPARAMDIALOG_H
#define EDITPARAMDIALOG_H

#include <QDialog>
#include <QAbstractItemModel>
#include <QListWidget>
#include "QRelayDeviceControl.h"

namespace Ui {
class EditParamDialog;
}

class EditParamDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit EditParamDialog(QWidget *parent = 0);
    ~EditParamDialog();
public:
    void CreateDeviceList(void);
    void InsertDevice(RelayDeviceSharePonterType & pdev);
protected slots:
    void	itemClicked ( QListWidgetItem * item );
    void ListCurrentItemChanged ( QListWidgetItem * current, QListWidgetItem * previous );
    void DeviceAckStatus(QString ackstr);
protected slots:
    void RemoveClicked(bool);
    void ReadClicked(bool);
    void WriteClicked(bool);
    void ResetClicked(bool);
private:
    Ui::EditParamDialog *ui;
    QListWidget * list_devices;
    device_info_st    edit_info;
};

#endif // EDITPARAMDIALOG_H
