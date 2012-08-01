#include "tryversiondialog.h"
#include "ui_tryversiondialog.h"

TryVersionDialog::TryVersionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TryVersionDialog)
{
    ui->setupUi(this);
    timeout_tick_count = 8;
    connect(&timer,SIGNAL(timeout()),this,SLOT(timeout_tick()));
    timer.start(1000);
}

TryVersionDialog::~TryVersionDialog()
{
    delete ui;
}
void TryVersionDialog::setTryVersionString(QString title1,QString title2)
{
    ui->label->setText(title1);
    ui->label_2->setText(title2);
}

void TryVersionDialog::timeout_tick(void)
{
    if(--timeout_tick_count == 0) {
        this->close();
    }
}
