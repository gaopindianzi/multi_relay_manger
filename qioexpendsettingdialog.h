#ifndef QIOEXPENDSETTINGDIALOG_H
#define QIOEXPENDSETTINGDIALOG_H

#include <QDialog>
#include <QSharedPointer>
#include <QPushButton>
#include <QTimer>
#include "multimgr_device_dev.h"
#include "QRelayDeviceControl.h"







class QMyIoOutButton : public QPushButton
{
    Q_OBJECT
public:
    QMyIoOutButton(RelayDeviceSharePonterType pdev,int index,QWidget * parent = 0 );
private:
    void paintEvent ( QPaintEvent * event );
protected:
    void mousePressEvent ( QMouseEvent * event );
private slots:
    void tcp_timer();
private:
    RelayDeviceSharePonterType pdevice;
    int  button_index;
    QImage imagenum;
    QImage image;
    QImage clock_image;
    int spaceing;
    QTimer * timer;
    int           timer_count;
};







namespace Ui {
class QIoExpendSettingDialog;
}

class QIoExpendSettingDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QIoExpendSettingDialog(RelayDeviceSharePonterType pdev,QWidget *parent = 0);
    ~QIoExpendSettingDialog();
    
private:
    Ui::QIoExpendSettingDialog *ui;
private:
    RelayDeviceSharePonterType pdevice;
};

#endif // QIOEXPENDSETTINGDIALOG_H
