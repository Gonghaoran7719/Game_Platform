#ifndef GAMESUP_H
#define GAMESUP_H

#include <QWidget>

namespace Ui {
class gamesup;
}

class gamesup : public QWidget
{
    Q_OBJECT

public:
    explicit gamesup(QWidget *parent = nullptr);
    ~gamesup();
signals:
    void SIG_killProcess();

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::gamesup *ui;
};

#endif // GAMESUP_H
