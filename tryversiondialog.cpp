#include "tryversiondialog.h"
#include "ui_tryversiondialog.h"

TryVersionDialog::TryVersionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TryVersionDialog)
{
    ui->setupUi(this);
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
