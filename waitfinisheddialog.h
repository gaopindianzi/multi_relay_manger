#ifndef WAITFINISHEDDIALOG_H
#define WAITFINISHEDDIALOG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class WaitFinishedDialog;
}

class WaitFinishedDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit WaitFinishedDialog(QWidget *parent = 0);
    ~WaitFinishedDialog();
    
private:
    Ui::WaitFinishedDialog *ui;
public:
    void setprogressticks(int all);
    void setprogresstitle(QString title);
private slots:
    void timetick(void);
public slots:
    void progresstick(void);
    void progressfinished(void);
private:
    QTimer  timer;
    int         ticks;
    int         ticks_all;
};

#endif // WAITFINISHEDDIALOG_H
