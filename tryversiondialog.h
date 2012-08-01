#ifndef TRYVERSIONDIALOG_H
#define TRYVERSIONDIALOG_H

#include <QDialog>
#include <QTimer>

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
    QTimer timer;
    unsigned int  timeout_tick_count;
private slots:
    void timeout_tick(void);
};

#endif // TRYVERSIONDIALOG_H
