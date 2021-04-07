#include "gamesup.h"
#include "ui_gamesup.h"
#include "mygp.h"
#include "qdebug.h"
gamesup::gamesup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::gamesup)
{
    ui->setupUi(this);
}

gamesup::~gamesup()
{
    delete ui;
}

void gamesup::on_pushButton_3_clicked()
{
    qDebug()<<"发送 SIG_killProcess 信号";
    emit SIG_killProcess();
}

void gamesup::on_pushButton_5_clicked()
{

    QString str = ui->lineEdit->text();
    ui->textBrowser->append("我说："+str);
    ui->lineEdit->clear();
}

void gamesup::on_pushButton_clicked()
{
    ui->pushButton->setText("关闭语音");
}

void gamesup::on_pushButton_2_clicked()
{
     ui->pushButton_2->setText("关闭视屏");
}
