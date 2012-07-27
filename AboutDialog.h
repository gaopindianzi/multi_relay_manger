#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QImage>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();
protected:
    void paintEvent ( QPaintEvent * event );
private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
