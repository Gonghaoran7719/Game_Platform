#include "mygp.h"
#include "ui_mygp.h"
#include<QMessageBox>
#include <QCryptographicHash>
#include"ui_gamesup.h"

#define MD5_KEY  (1234)
static QByteArray GetMD5(QString str)
{
    QString val = QString("%1_%2").arg(str).arg(MD5_KEY);
    QByteArray buf = val.toLocal8Bit(); //  toLatin1() ASCII 编码  toLocal8Bit Unicode编码
    QByteArray res =  QCryptographicHash::hash(buf ,  QCryptographicHash::Md5 );
    return res.toHex();  // ABF132...  32个长
}

QMap<QString,QString> m_mNameToPostion;

mygp::mygp(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mygp)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(1);
    m_login = new LoginDialog();
//    m_login = new LoginDialog();

   // m_anager=new managerDlg();
   connect( m_login ,SIGNAL(SIG_loginCommit(QString,QString)) ,
            this , SLOT(slot_loginCommit(QString,QString)) );
   connect( m_login ,SIGNAL(SIG_registerCommit(QString,QString)) ,
            this , SLOT(slot_registerCommit(QString,QString)) );
    Process1 = NULL;
    myProcess = NULL;
   m_login->show();
   //数据库
   m_sql=new CSqlite;

   //加载数据库
       loadSqlAndSetMusicList();
        m_addgame = new addgame(m_sql,nullptr);

        slot_updatalist();

        m_client = new QMyTcpClient;
     m_client ->InitNetWork( "192.168.11.143" , _DEF_PORT);

   m_anager=new managerDlg(m_client,nullptr);

   m_gamesup = new gamesup;
  connect(m_gamesup,SIGNAL(SIG_killProcess()),
          this,SLOT( slot_killProcess() ) );

   connect( m_client , SIGNAL(SIG_ReadyData(char*,int)) ,
            this , SLOT(slot_clientDealData(char*,int))  );
   connect( this , SIGNAL(SIG_ReadyData(char*,int)) ,
            this , SLOT(slot_clientDealData(char*,int))  );
   connect( this , SIGNAL(SIG_ReadyData(char*,int)) ,
             m_anager, SLOT(slot_clientDealData(char*,int))  );


   connect(  m_addgame,SIGNAL(SIG_updatalist()) ,
             this , SLOT( slot_updatalist() ) ) ,


  connect( m_gamesup , SIGNAL(on_pushButton_clicked()),
           this ,  SLOT(slot_openAudio() ) );
//   connect(m_pAudioRead, SIGNAL(sig_net_tx_frame(QByteArray)) ,
//           this , SLOT(slot_sendAudioFrame(QByteArray)) );
    m_gameCount = 1;
    m_mNameToPostion["炸弹人"] = "D:/IT/GroupProject/1128/mygp/炸弹人.exe";
    m_mNameToPostion["泡泡堂"] = "D:/IT/GroupProject/1128/mygp/BnB.exe";
   // m_mNameToPostion["贪吃蛇"] = "D:/IT/GroupProject/1128/mygp/炸弹人.exe";
    connect(m_gamesup->m_pAudioRead, SIGNAL(sig_net_tx_frame(QByteArray)) ,
            this , SLOT(slot_openAudio(QByteArray)) );

 connect(m_gamesup,SIGNAL(SIG_SendMsg(QString)),this,SLOT(slot_getChatLine(QString)));
}

mygp::~mygp()
{
    delete ui;
//    if(myProcess)
//        delete myProcess;
//    if(Process)
//        delete Process;

}
//发送音频帧
void mygp::slot_openAudio(QByteArray ba)
{
    ///音频数据帧
    /// 成员描述
    /// int type;
    /// int userId;
    /// int roomId;
    /// QByteArray audioFrame;
    int nlen = ba.size() + 12 ;
    char *buf = new char[nlen];
    char * tmp = buf;

    *(int*)tmp = DEF_PACK_AUDIO_FRAME;
    tmp += sizeof(int);

    *(int*)tmp = m_userID;
    tmp += sizeof(int);

    *(int*)tmp =  m_RoomID;
    tmp += sizeof(int);

    memcpy( tmp , ba.data() , ba.size());
    qDebug()<<"audio frame"<<buf;
    m_client->SendData( buf , nlen);
    delete[] buf;
}

void  mygp:: slot_updatalist()
{
    ui->gamelist->clear();
    sqlStr =QString("select * from t_game")  ;

    resList2.clear();
    resList.clear();


       res = m_sql->SelectSql( sqlStr ,2 , resList);

              //QLlist<QString> ::iterator ite=resList.begin();
       while(!resList.empty())
       {
           ui->gamelist->addItem(resList.front());
           resList.pop_front() ;
           resList2.push_back(resList.front());
           resList.pop_front() ;

       }

}

void mygp::on_pushButton_clicked()
{
    QStringList resList;
    QString gname = ui->lineEdit->text();
    if(gname == NULL)
    {
        QMessageBox::about(this,"提示","请输入游戏名");
        return;
    }
    sqlStr =QString("select gamePath from t_game where gameName='%1'").arg(gname)  ;


       res = m_sql->SelectSql( sqlStr ,1 , resList);



   if( resList.size())
   {
      myProcess = new QProcess();
       myProcess->start(resList[0]);

       m_gamesup->move(1450,400);
       m_gamesup->show();

//       if (!myProcess)
////         //this->hide();
////       else
//           QMessageBox::about(this,"提示","游戏启动失败/请查看路径是否正确");

   }
   else
       QMessageBox::about(this,"提示","没找到");

}
void mygp::loadSqlAndSetMusicList() // 加载sql 并设置歌单 音量
{
    //首先要获取路径  设置sql
    QString DBDir = QDir::currentPath() + "/sql/";
    QString FileName = "music.db";
    QDir tempDir;
    tempDir.setPath( DBDir );
    //看路径是否存在 //路径是否存在 没有就创建
    if( !tempDir.exists( DBDir  ))
    {
        qDebug()<< "不存在该路径";
        tempDir.mkdir(DBDir );
    }
//    // 需要 images  music  lrc
//    if( !tempDir.exists( QDir::currentPath() + "/music/"  ))
//    {
//        qDebug()<< "不存在该路径";
//        tempDir.mkdir( QDir::currentPath() + "/music/"  );
//    }
//    if( !tempDir.exists( QDir::currentPath() + "/images/"  ))
//    {
//        qDebug()<< "不存在该路径";
//        tempDir.mkdir( QDir::currentPath() + "/images/" );
//    }
//    if( !tempDir.exists( QDir::currentPath() + "/lrc/"  ))
//    {
//        qDebug()<< "不存在该路径";
//        tempDir.mkdir( QDir::currentPath() + "/lrc/"  );
//    }

    //看有没有数据库
    //其中 t_musicList 包含字段为musicName varchar(260) , musicPath varchar(260)
    //sql语句:
    //create table t_musicList (musicName varchar(260) , musicPath varchar(260));
    //t_volumn 包含的字段为 volumn int
    //sql 语句:
    //create table t_volumn ( volumn int);
    //insert into t_volumn (volumn ) values( 30 );

    QFile tempFile ;
    if( tempFile.exists( DBDir + FileName) )
    {//有数据库 加载
        qDebug()<< "有数据库";
        m_sql->ConnectSql( DBDir + FileName  ); //传入数据库绝对路径
        QStringList resList;
        QString sqlStr = "select musicName , musicPath from t_musicList;";
        bool res = m_sql->SelectSql( sqlStr , 2 , resList );

        if( !res )
        {
            return;
        }
//        //把 查到的结果放到 数组和控件
//        for( int i  =0 ; i < resList.count() ; i+=2 )
//        {
//            ui->lw_musicList->addItem( resList[i] );//名字
//            m_musicList[ m_musicCount++ ] = resList[i + 1];//全路径
//        }

        }
    else
    {
        //没有数据库 要创建
                qDebug()<< "没有数据库";
                tempFile.setFileName( DBDir + FileName );
                if( !tempFile.open(QIODevice::WriteOnly | QIODevice::Text )) //创建文件
                {
                    qDebug()<< "数据库创建失败";
                    QMessageBox::information(this , "提示","数据库创建失败,会导致无法写入歌曲");

                }else
                {
                    qDebug()<< "数据库创建";
                    tempFile.close();
                    m_sql->ConnectSql( DBDir + FileName  ); //传入数据库绝对路径
                    //创建表
                    QString sqlStr = "create table t_game (gameName varchar(260) , gamePath varchar(260));";
                    m_sql->UpdateSql( sqlStr );


                }
            }
        }



//登陆槽函数
void mygp::slot_loginCommit(QString name,QString password)
{
    //发送给服务器
    STRU_LOGIN_RQ rq;
    std::string strTmp = name.toStdString();
    char* buf = (char*) strTmp.c_str();
   strcpy_s(  rq.m_szUser ,    MAX_SIZE , buf);

    //转存用户名
    strcpy_s(m_userName , rq.m_szUser);
//    strTmp = password.toStdString();
//    buf = (char*) strTmp.c_str();
//    strcpy_s( rq.m_szPassword , buf);

    QByteArray ba = GetMD5(password);
    memcpy ( rq.m_szPassword , ba.data() , ba.length());

    m_client->SendData( (char*)&rq , sizeof(rq));
    qDebug()<<rq.m_szPassword;
    qDebug()<<rq.m_szUser;
}


//注册槽函数
void mygp::slot_registerCommit(QString name,QString password)
{
    //发送给服务器
    STRU_REGISTER_RQ rq;
    std::string strTmp = name.toStdString();
    char* buf = (char*) strTmp.c_str();
     strcpy_s(  rq.m_szUser ,    MAX_SIZE , buf);

//    strTmp = password.toStdString();
//    buf = (char*) strTmp.c_str();
//    strcpy_s( rq.m_szPassword , buf);

    QByteArray ba = GetMD5(password);
    memcpy ( rq.m_szPassword , ba.data() , ba.length());

    m_client->SendData( (char*)&rq , sizeof(rq));
    qDebug()<<rq.m_szPassword;
    qDebug()<<rq.m_szUser;
}







//客户端接收服务器信息处理
void mygp::slot_clientDealData(char* buf,int len)
{
    int nType = *(int*) buf; //*(int*) 按四个字节取
    switch( nType )
    {
        case DEF_PACK_LOGIN_RS:
            dealLoginRs(buf,len);
        break;
        case DEF_PACK_REGISTER_RS:
            dealRegisterRs(buf,len);
            break;
          case DEF_PACK_GAMEUP_RS:
             dealGameup(buf,len);
        break;
    case DEF_PACK_GAMEUPUP_RS:
        dealgameupup(buf ,len);
       dealGameup(buf,len);
  break;
    case  DEF_PACK_GAMEDEL_RS:
     delgamerq(buf,len);
 break;
    case  DEF_PACK_DOWNLOAD_RS:
       downloadinfoRs(buf,len);
 break;
    case  DEF_PACK_GAMEDOWN_RS:
        downloadblock(buf,len);
 break;
     case  DEF_PACK_CREATEROOM_RS:
            CreateRoomRS(buf,len);
        break;
    case DEF_PACK_JOINROOM_RS:
            JionRoomRS(buf,len);
        break;
    case DEF_PACK_ROOM_MEMBER:
        getMemeber(buf,len);
        break;
    case DEF_PACK_CHAT_RQ:
        dealChatRq(buf,len);
                break;

    case DEF_PACK_CHAT_RS:
        dealChatRs(buf,len);
        break;
    }
    delete[] buf;
}


////登录请求结果
//#define userid_no_exist      0
//#define password_error       1
//#define login_sucess         2
//#define user_online          3




void mygp::dealGameup(char *buf, int len)
{

     STRU_GAMEUP_RS*rs=(STRU_GAMEUP_RS *)buf;
    if(rs->result==true)
    {
         m_anager->flag=true;
        QMessageBox::about( this->m_anager , "提示", "服务器中已存在上传完毕"  );//秒传

    }
    else
    {

     emit  managerDlg::  m_anager->SIG_UploadFile(m_anager->filePath );

       m_anager-> count=1;
    }
}

void mygp::dealgameupup(char *buf, int len)
{
    STRU_GAMEUPUP_RS*rs=(STRU_GAMEUPUP_RS*)buf;
    if(rs->result==true)
    {
        QMessageBox::about( this->m_anager , "提示", "上传完毕"  );
    }
}

void mygp::delgamerq(char *buf, int len)
{
    STRU_GAMEDEL_RS*rs=(STRU_GAMEDEL_RS*)buf;
    if(rs->result==true)
    {
        QMessageBox::about( this->m_anager , "提示", "删除成功"  );
    }
    else{
       QMessageBox::about( this->m_anager , "提示", "文件不存在"  );
        }

}

//处理服务器登录回复
void mygp::dealLoginRs(char* buf,int len)
{
    STRU_LOGIN_RS * rs = (STRU_LOGIN_RS *)buf;
    switch( rs->m_lResult )
    {
    case userid_no_exist:
        QMessageBox::information( this->m_login , "提示", "用户不存在, 登录失败"  );
        break;
    case password_error:
        QMessageBox::information( this->m_login , "提示", "密码错误, 登录失败"  );
        break;
    case login_sucess:
        this->m_login->hide();
        this->show();
//        this->ui->lb_name->setText( m_userName );
        m_userID = rs->m_UserID;
        break;
    }
}

////注册请求结果
//#define userid_is_exist      0
//#define register_sucess      1


//处理服务器注册回复
void mygp::dealRegisterRs(char* buf,int len)
{
    STRU_REGISTER_RS * rs = (STRU_REGISTER_RS *)buf;
    switch( rs->m_lResult )
    {
    case userid_is_exist:
        QMessageBox::information( this->m_login , "提示", "用户已存在, 注册失败"  );
        break;
    case register_sucess:
        QMessageBox::information( this->m_login , "提示", "注册成功"  );
        break;
    }
}



void mygp::on_toolButton_2_clicked()
{
//    myProcess = new QProcess();
//    myProcess->start("D:/IT/GroupProject/1128/mygp/炸弹人.exe");
//    this->hide();

}




//添加游戏
void mygp::on_pushButton_2_clicked()
{
    m_addgame->show();

//    //弹窗 选择音乐文件 获取文件的绝对路径 存路径 需要字符串的数组
//    QStringList path = QFileDialog::getOpenFileNames( this,"选择歌曲","./../music");
//    //1.父类 2.标题 3.默认路径 4.文件过滤    //去重
//    bool hasSame = false;
//    for( int i = 0 ; i < path.count() ; ++i)
//    {
//        hasSame = false;
//        for( int j = 0 ; j < m_gameCount ; ++j) //遍历歌单看有没有重复的
//        {
//            if( m_mNameToPostion.find(gname) = m_mNameToPostion.end())
//            {

//            }
//            if( path[i] == m_musicList[j])
//            {
//                hasSame = true;
//                break;
//            }
//        }
//        if( !hasSame)
//        {
//            m_musicList[ m_musicCount++ ]  = path[i];
//            //同时添加到控件  -- 添加歌曲名
//            QFileInfo  info(path[i]);
//            //info.baseName(); // 文件名   /music/稻香.mp3  -->baseName 是 稻香            ui->lw_musicList->addItem( info.baseName() );
//                // 插入sql
//            QString sqlStr =QString("insert into t_musicList (musicName , musicPath) values ('%1','%2')")
//                    .arg(info.baseName()).arg(path[i]); //格式化字符串            m_sql->UpdateSql( sqlStr );        }    }    if( m_musicCount != 0 )    {        ui->lw_musicList->setCurrentRow(0);    }

//        }
//    }
}

void mygp::on_toolButton_3_clicked()
{
//    Process = new QProcess();
//    Process->start("D:/IT/GroupProject/1128/mygp/BnB.exe");
    this->hide();

}
//管理员
void mygp::on_pushButton_5_clicked()
{
     m_anager->show();
}

#include "gamesup.h"

void mygp::on_gamelist_doubleClicked(const QModelIndex &index)
{

    qDebug()<<resList2[ui->gamelist->currentIndex().row()];

//    myProcess = new QProcess();
//    myProcess->start(resList2[ui->gamelist->currentIndex().row()]);

    Process1 = new QProcess();
    Process1->start(resList2[ui->gamelist->currentIndex().row()]);

    qDebug()<< ui->gamelist->currentIndex().row();
    qDebug()<<resList2[ui->gamelist->currentIndex().row()];
   // gamesup *gs = new gamesup;

    m_gamesup->move(1450,400);
    m_gamesup->show();
}

void mygp::slot_killProcess()
{
    qDebug()<<"kill 进程";
    if(myProcess)
    {
        myProcess->close();
        myProcess->kill();
    }
    if(Process1)
    {
        Process1->close();
        Process1->kill();
    }
    //m_gamesup->hide();
}

void mygp::on_pushButton_6_clicked()
{
    sqlStr =QString("delete from t_game where rowid = '%1'").arg(indexs)  ;

    res = m_sql->UpdateSql(sqlStr);
    if(res >0)
        slot_updatalist();
    else
        qDebug()<<"fail del ";
}


void mygp::on_gamelist_clicked(const QModelIndex &index)
{
     indexs= index.row();
    qDebug()<<index.row();

}

void mygp::on_pushButton_3_clicked()
{
//    QString str = ui->lineEdit->text();
//    ui->textBrowser->append("我说："+str);
//    ui->lineEdit->clear();

//    ui->tab_2->textBrowser->append("坦克大战");
//    ui->lineEdit_2->setText("11");
//   ui->lin
}


//创建房间请求
void mygp::on_pushButton_14_clicked()
{
    QString roomNum = QInputDialog::getText(this,"创建房间","输入房间号");
    QRegExp reg("[0-9]{1,10}");
    if(reg.exactMatch(roomNum) )
    {
        STRU_CREATEROOM_RQ rq;
        rq.m_UserID = m_userID;
        rq.m_roomID = roomNum.toInt();
        m_RoomID = roomNum.toInt();
        m_client->SendData((char*)&rq , sizeof(rq));
    }
    else
        QMessageBox::about(this, "提示","必须是纯数字");

}

//创建房间回复
void mygp::CreateRoomRS(char* buf,int len)
{
    STRU_CREATEROOM_RS * rs = (STRU_CREATEROOM_RS *)buf;
    if(rs->m_lResult == 0)
    {
        QMessageBox::about(this, "提示","已存在，请重新输入");
    }
    else
    {
        QMessageBox::about(this, "提示","创建成功");
        qDebug()<<rs->m_RoomId;

        m_gamesup->ui->label_3->setNum(m_RoomID);
        m_gamesup->show();
    }


}

//下载请求包
void mygp::on_pb_down_clicked()
{
    QString name = ui-> le_down->text();
    STRU_DOWNLOAD_RQ rq;
    std::string strTmp = name.toStdString();
    char* buf = (char*) strTmp.c_str();
     strcpy_s(  rq.m_name ,    MAX_SIZE , buf);
     m_client->SendData( (char*)&rq , sizeof(rq));


}

void mygp::downloadinfoRs(char *buf, int len)
{
    STRU_DOWNLOAD_RS * rs = (STRU_DOWNLOAD_RS *)buf;


    FileInfo * info = new FileInfo;
    // videoid 作为文件的标示 , fileid 用来区分不同控件

    info->pFile=nullptr;
    info->filePos=0;
    strcpy_s(info->fileName,MAX_SIZE,rs->m_name);
    info->fileSize=rs->m_nFileSize;
    sprintf(info->filePath ,"D:\\44444444444\\downexe\\%s",info->fileName);
    info->pFile =new QFile(info->filePath);


    if( info->pFile->open(QIODevice::WriteOnly))
    {
        m_mapfileTOFileInfo[info->fileName ] = info;
         qDebug()<<info->fileName;
    }else
    {
        delete info->pFile;
        delete info;
    }
    qDebug()<<info->fileName;




}
void mygp::downloadblock(char *buf, int len)
{
    STRU_GAMEDOWN_RS * rs = (STRU_GAMEDOWN_RS *)buf;


    auto ite = m_mapfileTOFileInfo.find( rs->m_filename );
    qDebug()<<rs->m_filename;

    if(  ite ==  m_mapfileTOFileInfo.end())  return;
 qDebug()<<rs->m_filename;
    FileInfo* info = m_mapfileTOFileInfo[ rs->m_filename ];

    int64_t res =  info->pFile->write( rs->m_file , rs->blocksize );
    info->filePos += res;
  qDebug()<<info->filePos;
  qDebug()<<info->fileSize;
    if( info->filePos >= info->fileSize )
    {
        //关闭文件
        info->pFile->close();
        //删除该节点
        m_mapfileTOFileInfo.erase( ite );
        //回收info
        delete  info->pFile;
        delete info;
        info = NULL;
         QMessageBox::about(this, "提示","下载完成");
    }


}

//加入房间请求
void mygp::on_pushButton_15_clicked()
{
    QString roomNum = QInputDialog::getText(this,"加入房间","输入房间号");
    QRegExp reg("[0-9]{1,10}");
    if(reg.exactMatch(roomNum) )
    {
        STRU_JOINROOM_RQ rq;
        rq.m_UserID = m_userID;
        rq.m_RoomID = roomNum.toInt();
        m_RoomID = roomNum.toInt();
        m_client->SendData((char*)&rq , sizeof(rq));
    }
    else
        QMessageBox::about(this, "提示","必须是纯数字");

}


//加入房间回复
void mygp::JionRoomRS(char* buf,int len)
{
    //加入房间结果
    STRU_JOINROOM_RS *rs = (STRU_JOINROOM_RS *)buf;
    if(rs->m_lResult == room_no_exist)
    {
       QMessageBox::about(this, "提示","房间不存在");
    }
    else if(rs->m_lResult == join_success )
    {
        QMessageBox::about(this, "提示","加入成功");
        m_gamesup->ui->label_3->setNum(m_RoomID);
        m_gamesup->show();
    }
}

//获取成员信息请求
void mygp::getMemeber(char* buf,int len)
{
    //解析包
    STRU_ROOM_MEMBER_RQ * rq = (STRU_ROOM_MEMBER_RQ *)buf;
    //显示加入房间的用户列表
     //m_gamesup->ui->Userlist->clear();
    m_gamesup->ui->label_4->setText("在线用户"+rq->m_UserID);
    m_gamesup->ui->label_6->setText("正在讲话"+rq->m_UserID);
     m_gamesup->ui->Userlist->addItem(rq->m_szUser);


    //创建对应的声音模块
    if( m_mapIDToAudioWrite.find(rq->m_UserID ) == m_mapIDToAudioWrite.end() )
    {
        Audio_Write * audioWrite = new Audio_Write;
        m_mapIDToAudioWrite[ rq->m_UserID ] = audioWrite;
    }
}
//获取gamesup里打出的聊天信息

QString mygp::slot_getChatLine(QString str)
{
    qDebug()<<"slot_getChatLine"<<str;

    STRU_CHAT_RQ rq;
    rq.m_userID = m_userID;
    rq.m_roomID = m_RoomID;
    std::string  str1 = str.toStdString();
    const char* buf = str1.c_str();
    strcpy( rq.m_ChatContent , buf );

    m_client->SendData( (char*)&rq , sizeof(rq));

    return str;

}



void mygp::dealChatRq(char* buf,int len)
{
    //解析
    STRU_CHAT_RQ *rq = (STRU_CHAT_RQ *)buf;
    QString str1;
    slot_getChatLine(str1);

    //m_gamesup->ui->ChatMsg->setText(str1);

      //  ChatDialog *chat = m_mapIDToChatDialog[ rq->m_userID ];
         m_gamesup->ui->ChatMsg->append( QString("[ %1 ] %2").arg(rq->m_userID)
                                        .arg(QTime::currentTime().toString("hh:mm:ss")) );
        QString content = rq->m_ChatContent;
        m_gamesup->ui->ChatMsg->append(str1);
}

void mygp::dealChatRs(char *buf,int len)
{
    //解析
    STRU_CHAT_RS *rs = (STRU_CHAT_RS *)buf;
//    if( rs->m_result == user_is_offline )//结果是不在线
//    {//找到聊天窗口 贴到聊天窗口中

//            m_gamesup->ui->ChatMsg->append(QString("%1 用户离线, 消息未送达")
//                                           .arg(QTime::currentTime().toString("hh:mm:ss")));
//            m_gamesup->ui->ChatMsg->append("");
//            m_gamesup->show();
//    }
}
