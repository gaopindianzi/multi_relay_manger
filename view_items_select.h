#ifndef VIEW_ITEMS_SELECT_H
#define VIEW_ITEMS_SELECT_H

#include <QDialog>

namespace Ui {
class view_items_select;
}

class view_items_select : public QDialog
{
    Q_OBJECT
    
public:
    explicit view_items_select(QWidget *parent = 0);
    ~view_items_select();
    
private:
    Ui::view_items_select *ui;
};

#endif // VIEW_ITEMS_SELECT_H
