#include "addgame.h"
#include "ui_addgame.h"
#include "mygp.h"
#include "qDebug"

extern QMap<QString,QString> m_mNameToPostion;
addgame::  addgame(CSqlite*m_sqll,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addgame)
{
    ui->setupUi(this);
    m_sql=m_sqll;
}

addgame::~addgame()
{
    delete ui;
}


//添加按钮
void addgame::on_pushButton_2_clicked()
{
    QString gname = ui->lineEdit->text();
    QString path = ui->lineEdit_2->text();
    //qDebug()<<gname<<path;
  //  m_mNameToPostion.insert(gname,path);
   // m_mNameToPostion.key(gname) ;
   // m_mNameToPostion.value(path);

    /*mygp::*///m_mNameToPostion[gname] = path;

    QString sqlStr =QString("insert into t_game (gameName,gamePath) values('%1','%2')").arg(gname,path) ;
                        m_sql->UpdateSql( sqlStr );

    auto ite = /*mygp::*/m_mNameToPostion.begin();
    while(ite  != /*mygp::*/m_mNameToPostion.end())
    {
        qDebug()<<ite.key()<<ite.value();
        ++ite;
    }

    emit SIG_updatalist( );


   // qDebug()<<gname<<m_mNameToPostion[gname];
    QMessageBox::about(this,"提示","添加成功");

}

void addgame::on_pushButton_3_clicked()
{
     QStringList path = QFileDialog::getOpenFileNames( this,"选择游戏","./");
}

void addgame::on_pushButton_clicked()
{
    this->hide();
}
