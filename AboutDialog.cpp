#include "AboutDialog.h"
#include "ui_AboutDialog.h"


AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::paintEvent ( QPaintEvent * event )
{
    QPainter painter(this);
    QImage image;
    image.load(":/sys/sys_icon/sources/ABOUT.png");
    QRectF target(this->rect().left(),this->rect().top(),image.width(),image.height());
    painter.drawImage(target, image);
}
