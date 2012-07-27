#ifndef QPASSWORDMANGERDIALOG_H
#define QPASSWORDMANGERDIALOG_H

#include <QDialog>
#include <QList>
#include <QListWidgetItem>
#include <QListWidget>
#include "PasswordItemDef.h"

namespace Ui {
class QPasswordMangerDialog;
}

class QPasswordItem: public QListWidgetItem
{
public:
    QPasswordItem (password_item & pwd, QListWidget * parent = 0, int type = Type );
public:
    password_item  ppwd;
};



class QPasswordMangerDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QPasswordMangerDialog(QWidget *parent = 0);
    ~QPasswordMangerDialog();
public:
    void CreatePasswordList(void);
    void InitPasswordList(QList<password_item> pwd_list);
    QList<password_item> & GetPasswordList(void) { return current_list; }
    bool  is_ok;
protected slots:
    void	itemClicked ( QListWidgetItem * item );
protected slots:
    void LoadPasswordList(void);
    void SavePasswordList(void);
    void InsertNewPassword(void);
    void RemovePassword(void);
    void Ok(void);
    void Cancel(void);
private:
    Ui::QPasswordMangerDialog *ui;
    QListWidget * password_listwidget;
    QList<password_item>  current_list;
};

#endif // QPASSWORDMANGERDIALOG_H
