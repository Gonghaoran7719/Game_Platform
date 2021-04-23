#include "chatdlg.h"
#include "ui_chatdlg.h"

chatDlg::chatDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chatDlg)
{
    ui->setupUi(this);
}

chatDlg::~chatDlg()
{
    delete ui;
}
