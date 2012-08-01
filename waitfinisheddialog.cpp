#include "waitfinisheddialog.h"
#include "ui_waitfinisheddialog.h"

WaitFinishedDialog::WaitFinishedDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WaitFinishedDialog)
{
    ui->setupUi(this);

    connect(&timer,SIGNAL(timeout()),this,SLOT(timetick()));
    setprogressticks(60);

    timer.start(50);

    this->setWindowTitle(tr("wait finished dialog"));
}

WaitFinishedDialog::~WaitFinishedDialog()
{
    delete ui;
}


void WaitFinishedDialog::setprogressticks(int all)
{
    ticks_all = ticks = all;
    ui->progressBar->setRange(0,ticks_all);
    ui->progressBar->setValue(0);
}

void WaitFinishedDialog::timetick(void)
{
    if(ticks > 0) {
        if(--ticks == 0) {
            //Ê§°Ü
            ui->label->setText(tr("Write device may fail, try again!!!"));
        }
    }
    ui->progressBar->setValue(ticks_all - ticks);
}

void WaitFinishedDialog::progresstick(void)
{
    timetick();
}

void WaitFinishedDialog::progressfinished(void)
{
    if(ticks > 0) {
        close();
    }
}

void WaitFinishedDialog::setprogresstitle(QString title)
{
    ui->label->setText(title);
}
