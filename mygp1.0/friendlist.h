#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#include <QWidget>

namespace Ui {
class friendList;
}

class friendList : public QWidget
{
    Q_OBJECT

public:
    explicit friendList(QWidget *parent = nullptr);
    ~friendList();

private:
    Ui::friendList *ui;
};

#endif // FRIENDLIST_H
