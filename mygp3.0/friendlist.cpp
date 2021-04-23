#include "friendlist.h"
#include "ui_friendlist.h"

friendList::friendList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::friendList)
{
    ui->setupUi(this);
}

friendList::~friendList()
{
    delete ui;
}
