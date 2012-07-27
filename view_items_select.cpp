#include "view_items_select.h"
#include "ui_view_items_select.h"

view_items_select::view_items_select(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::view_items_select)
{
    ui->setupUi(this);
}

view_items_select::~view_items_select()
{
    delete ui;
}
