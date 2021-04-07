#ifndef ADDGAME_H
#define ADDGAME_H

#include <QWidget>
#include"csqlite.h"

namespace Ui {
class addgame;
}

class addgame : public QWidget
{
    Q_OBJECT

public:
    explicit addgame(CSqlite*m_sqll,QWidget *parent = nullptr);
    ~addgame();

    signals:
     void SIG_updatalist( );

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

private:
    Ui::addgame *ui;
    CSqlite*m_sql;
};

#endif // ADDGAME_H
