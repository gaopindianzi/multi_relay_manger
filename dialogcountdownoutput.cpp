#include "dialogcountdownoutput.h"
#include "ui_dialogcountdownoutput.h"



#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>
#include <QRect>
#include <QLineEdit>
#include <QTextEdit>
#include <QProgressBar>
#include <QTimeEdit>



DialogCountdownOutput::DialogCountdownOutput(RelayDeviceSharePonterType pdev,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCountdownOutput)
{
    ui->setupUi(this);
    pdevice = pdev;
    //构建
    //根据不同的路数，设定不同数量和模式的按钮
    int ioout_num = pdev->GetIoOutNum();
    if(ioout_num  >= 2 && ioout_num <= 32) {
        int i;
        for(i=0;i<ioout_num;i++) {
            QHBoxLayout * linelayout = new QHBoxLayout();
            QLineEdit * name = new QLineEdit(parent);
            QProgressBar * progbar = new QProgressBar(parent);
            QTimeEdit * time = new QTimeEdit(parent);
            QPushButton * okbutton = new QPushButton(parent);

            progbar->setFixedWidth(300);
            okbutton->setText(tr("Apply"));
            name->setFixedWidth(100);
            //name->setFixedHeight(32);
            QPalette pa = name->palette();
            pa.setColor(QPalette::Base,QColor(135,206,235));
            name->setPalette(pa);
            name->setText(pdev->GetDeviceIoOutName(i));

            progbar->setRange(0,100);
            progbar->setValue(100*i/ioout_num);

            linelayout->addWidget(name);
            linelayout->addSpacing(6);
            linelayout->addWidget(progbar);
            linelayout->addSpacing(6);
            linelayout->addWidget(time);
            linelayout->addSpacing(6);
            linelayout->addWidget(okbutton);
            linelayout->addSpacing(6);
            linelayout->addStretch();
            ui->verticalLayout->addLayout(linelayout);
            ui->verticalLayout->addSpacing(5);
        }
        ui->verticalLayout->addStretch();
    }
}
DialogCountdownOutput::~DialogCountdownOutput()
{
    delete ui;
}
