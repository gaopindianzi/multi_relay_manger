#include "qpasswordmangerdialog.h"
#include "ui_qpasswordmangerdialog.h"
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QMessageBox>


#include "debug.h"
#define THISINFO                0
#define THISERROR            1
#define THISASSERT          1



QPasswordItem::QPasswordItem (password_item & pwd, QListWidget * parent, int type)
    : QListWidgetItem(parent,type)
{
    ppwd = pwd;
}





QPasswordMangerDialog::QPasswordMangerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QPasswordMangerDialog)
{
    ui->setupUi(this);
    CreatePasswordList();
    is_ok = false;
}

QPasswordMangerDialog::~QPasswordMangerDialog()
{
    delete ui;
}


void QPasswordMangerDialog::CreatePasswordList(void)
{
    password_listwidget = new QListWidget(this);
    password_listwidget->setSelectionMode(QAbstractItemView::ContiguousSelection);
    connect(password_listwidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(itemClicked(QListWidgetItem*)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(password_listwidget);
    ui->groupBoxPasswordList->setLayout(layout);

    ui->lineEditPwdOne->setEchoMode(QLineEdit::Password);
    ui->lineEditPwdTwo->setEchoMode(QLineEdit::Password);
    connect(ui->pushButtonAddOnePwdItem,SIGNAL(clicked()),this,SLOT(InsertNewPassword()));
    connect(ui->pushButtonRemovePwdItem,SIGNAL(clicked()),this,SLOT(RemovePassword()));
    connect(ui->pushButtonLoadPasswordList,SIGNAL(clicked()),this,SLOT(LoadPasswordList()));
    connect(ui->pushButtonSavePwdList,SIGNAL(clicked()),this,SLOT(SavePasswordList()));
    connect(ui->pushButtonCancel,SIGNAL(clicked()),this,SLOT(Cancel()));
    connect(ui->pushButtonOK,SIGNAL(clicked()),this,SLOT(Ok()));
}

void QPasswordMangerDialog::InitPasswordList(QList<password_item> pwd_list)
{
    int count = pwd_list.count();
    for(int i=0;i<count;i++) {
        password_item  pwd = pwd_list.at(i);
        QPasswordItem * item = new QPasswordItem(pwd);
        item->setText(pwd.alias);
        this->password_listwidget->insertItem(i,item);
        this->password_listwidget->setCurrentItem(item);
         itemClicked(item);
    }
}

void	QPasswordMangerDialog::itemClicked ( QListWidgetItem * item )
{
    QPasswordItem * it = (QPasswordItem *)item;
    this->ui->lineEditPwdAlias->setText(it->ppwd.alias);
    ui->lineEditPwdOne->setText("");
    ui->lineEditPwdTwo->setText("");
}



void QPasswordMangerDialog::LoadPasswordList(void)
{
    debuginfo(("LoadPasswordList clicked."));
}



void QPasswordMangerDialog::SavePasswordList(void)
{
    debuginfo(("SavePasswordList clicked."));
}




void QPasswordMangerDialog::InsertNewPassword(void)
{
    password_item pwd;
    pwd.alias = this->ui->lineEditPwdAlias->text();
    pwd.pwd = this->ui->lineEditPwdOne->text();

    if(pwd.alias.isEmpty()) {
        return ;
    }

    if(pwd.pwd != this->ui->lineEditPwdTwo->text()) {
        QMessageBox box(QMessageBox::Warning,tr("Warning"),tr("The input password is not the equal!"));
        box.exec();
        return ;
    }
    if(pwd.pwd.isEmpty()) {
        return ;
    }
    QPasswordItem * item = new QPasswordItem(pwd);
    item->setText(pwd.alias);

    int row = password_listwidget->count();
    //查找是否有相同的
    int i;
    for(i=0;i<row;i++) {
        QPasswordItem *it = (QPasswordItem *)password_listwidget->item(i);
        if(it->ppwd.alias == pwd.alias) {
            QString msg;
            msg = tr("The password alias has exist name:") + pwd.alias + tr(",\ndo you replace the exist password item?");
            QMessageBox box(QMessageBox::Question,tr("Question"),msg);
            box.setStandardButtons( QMessageBox::Yes | QMessageBox::Cancel);
            int ret = box.exec();
            if(ret == QMessageBox::Yes) {
                it->ppwd.pwd = pwd.pwd;
                return ;
            } else if(ret == QMessageBox::Cancel){
                return  ;  //放弃
            }
        }
    }
    if(i == row) {
        this->password_listwidget->insertItem(row,item);
        this->password_listwidget->setCurrentItem(item);
    }
}

void QPasswordMangerDialog::RemovePassword(void)
{
    debuginfo(("RemovePassword clicked."));
    if(password_listwidget->count() == 0) {
        return ;
    }
    int row = password_listwidget->currentRow();
    password_listwidget->takeItem(row);
}
void QPasswordMangerDialog::Ok(void)
{
    int row = password_listwidget->count();
    for(int i=0;i<row;i++) {
        QPasswordItem *it = (QPasswordItem *)password_listwidget->item(i);
        current_list.push_back(it->ppwd);
    }
    is_ok = true;
    this->close();
}

void QPasswordMangerDialog::Cancel(void)
{
    is_ok = false;
    this->close();
}
