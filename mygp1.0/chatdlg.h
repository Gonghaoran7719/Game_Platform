#ifndef CHATDLG_H
#define CHATDLG_H

#include <QWidget>

namespace Ui {
class chatDlg;
}

class chatDlg : public QWidget
{
    Q_OBJECT

public:
    explicit chatDlg(QWidget *parent = nullptr);
    ~chatDlg();

private:
    Ui::chatDlg *ui;
};

#endif // CHATDLG_H
