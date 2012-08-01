#ifndef TRYVERSIONDIALOG_H
#define TRYVERSIONDIALOG_H

#include <QDialog>

namespace Ui {
class TryVersionDialog;
}

class TryVersionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TryVersionDialog(QWidget *parent = 0);
    ~TryVersionDialog();
public:
    void setTryVersionString(QString title1,QString title2);
private:
    Ui::TryVersionDialog *ui;
};

#endif // TRYVERSIONDIALOG_H
