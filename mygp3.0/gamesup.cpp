#include "gamesup.h"
#include "ui_gamesup.h"
#include "mygp.h"
#include "qdebug.h"
gamesup::gamesup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::gamesup)
{
    ui->setupUi(this);
   m_pAudioRead = new Audio_Read;


    ui->label_3->setText("未加入房间..");

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

//点击放松按键
void gamesup::on_pushButton_5_clicked()
{

    QString str = ui->ChatLine->text();
    ui->ChatLine->clear();
    emit SIG_SendMsg(str);

}

//打开音频
void gamesup::on_pushButton_clicked()
{

    //
    QString str = ui->pushButton->text();
    std::string strTmp = str.toStdString();
    char* buf = (char*) strTmp.c_str();
    qDebug()<<buf;
    if(strcmp (buf,"打开音频") == 0)
    {
        qDebug()<<buf;
        //创建音频采集
        if(m_pAudioRead)
        {
            m_pAudioRead->ResumeAudio();
        }
        ui->pushButton->setText("关闭音频");
    }
    else if(strcmp (buf,"关闭音频") == 0)
    {
        qDebug()<<buf;
        if(m_pAudioRead)
        {
            m_pAudioRead->PauseAudio();
        }
        ui->pushButton->setText("打开音频");
    }

}



void gamesup::on_pushButton_2_clicked()
{
     ui->pushButton_2->setText("关闭视屏");

}

void gamesup::dealAudioFrame(char *buf, int len)
{
    ///音频数据帧
    /// 成员描述
    /// int type;
    /// int userId;
    /// int roomId;
    /// QByteArray audioFrame;

    char* tmp = buf;
    tmp += sizeof(int);

    int userId =  *(int*) tmp; //按照四个字节取数据
    tmp += sizeof(int);

    int roomId = *(int*) tmp;
    tmp += sizeof(int);

    if( m_mapIDToAudioWrite.find( userId) != m_mapIDToAudioWrite.end() )
    {
        QByteArray bt(tmp , len -12);
        Audio_Write * pAudioWrite = m_mapIDToAudioWrite[userId];
        pAudioWrite->slot_net_rx( bt );
    }
}
