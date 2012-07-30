#ifndef DIALOGCOUNTDOWNOUTPUT_H
#define DIALOGCOUNTDOWNOUTPUT_H

#include <QDialog>
#include <QSharedPointer>
#include <QPushButton>
#include <QTimer>
#include "multimgr_device_dev.h"
#include "QRelayDeviceControl.h"

namespace Ui {
class DialogCountdownOutput;
}

class DialogCountdownOutput : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogCountdownOutput(RelayDeviceSharePonterType pdev,QWidget *parent = 0);
    ~DialogCountdownOutput();
    
private:
    Ui::DialogCountdownOutput *ui;

    RelayDeviceSharePonterType pdevice;
};

#endif // DIALOGCOUNTDOWNOUTPUT_H
