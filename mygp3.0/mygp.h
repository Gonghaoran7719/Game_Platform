#ifndef MYGP_H
#define MYGP_H

#include <QWidget>
#include"Packdef.h"
#include"logindialog.h"
#include"qmytcpclient.h"
#include <QList>
#include <QMap>
#include <QMessageBox>
#include <QFileDialog>
#include<QProcess>
#include "addgame.h"
#include"managerdlg.h"
#include"csqlite.h"
#include"gamesup.h"
#include <QInputDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class mygp; }
QT_END_NAMESPACE

class mygp : public QWidget
{
    Q_OBJECT

public:
    mygp(QWidget *parent = nullptr);
    ~mygp();




public slots:
    void slot_loginCommit(QString name, QString password);
    void slot_registerCommit(QString name, QString password);
    void slot_clientDealData(char *buf, int len);
    void slot_updatalist();

    void  slot_killProcess();

public:
    void dealLoginRs(char *buf, int len);
    void dealRegisterRs(char *buf, int len);
    void dealGameup(char *buf, int len);
    void dealgameupup(char *buf, int len);
    void  delgamerq(char *buf, int len);
    void downloadinfoRs(char *buf, int len);
    void downloadblock(char *buf, int len);



private slots:
    void on_toolButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_toolButton_3_clicked();

    void on_pushButton_5_clicked();

 void on_gamelist_doubleClicked(const QModelIndex &index);



 void on_pushButton_6_clicked();



 void on_gamelist_clicked(const QModelIndex &index);

 void on_pushButton_3_clicked();

 void on_pushButton_14_clicked();
 void slot_openAudio(QByteArray ba) ;

 void on_pb_down_clicked();

 void on_pushButton_15_clicked();

 QString slot_getChatLine(QString str);

signals:

    void SIG_updatalist();
    void SIG_RoomNum(int );

public:
    Ui::mygp *ui;
    QMap<QString , FileInfo*> m_mapfileTOFileInfo;
    LoginDialog * m_login;
    QMyTcpClient * m_client;
    addgame *m_addgame;
    managerDlg*m_anager;
    int m_userID;
   // QList<QString> listgname;

    char m_userName[MAX_SIZE];
    int m_gameCount;
   // QProcess *Process;
    QProcess *myProcess;

    QProcess *Process1;
    void loadSqlAndSetMusicList(); // 加载sql 并设置歌单 音量
    CSqlite *m_sql;
     bool res ;
      QString sqlStr ;

      QStringList resList;
      QStringList resList2;
       int indexs;

       gamesup *m_gamesup;
    int m_RoomID;
//    static QMap<QString,QString> m_mNameToPostion;
//    friend class addgame;
       //friend class gamesup;
    void CreateRoomRS(char *buf, int len);
    void JionRoomRS(char *buf, int len);
    void getMemeber(char *buf, int len);

    QMap<int , Audio_Write*> m_mapIDToAudioWrite;
    void dealChatRq(char *buf, int len);
    void dealChatRs(char *buf, int len);
};

#endif // MYGP_H
