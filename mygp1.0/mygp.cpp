#include "mygp.h"
#include "ui_mygp.h"
#include<QMessageBox>


QMap<QString,QString> m_mNameToPostion;

mygp::mygp(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mygp)
{
    ui->setupUi(this);

    m_login = new LoginDialog();
//    m_login = new LoginDialog();

   // m_anager=new managerDlg();
   connect( m_login ,SIGNAL(SIG_loginCommit(QString,QString)) ,
            this , SLOT(slot_loginCommit(QString,QString)) );
   connect( m_login ,SIGNAL(SIG_registerCommit(QString,QString)) ,
            this , SLOT(slot_registerCommit(QString,QString)) );

   m_login->show();
   //数据库
   m_sql=new CSqlite;

   //加载数据库
       loadSqlAndSetMusicList();
        m_addgame = new addgame(m_sql,nullptr);

        slot_updatalist();

   m_client = new QMyTcpClient;
   m_client ->InitNetWork( "192.168.11.138" , _DEF_PORT);
  // m_anager=new managerDlg(m_client,nullptr);

   connect( m_client , SIGNAL(SIG_ReadyData(char*,int)) ,
            this , SLOT(slot_clientDealData(char*,int))  );
   connect( this , SIGNAL(SIG_ReadyData(char*,int)) ,
            this , SLOT(slot_clientDealData(char*,int))  );
   connect( this , SIGNAL(SIG_ReadyData(char*,int)) ,
             m_anager, SLOT(slot_clientDealData(char*,int))  );


   connect(  m_addgame,SIGNAL(SIG_updatalist()) ,
             this , SLOT(slot_updatalist())) ,
    m_gameCount = 1;
    m_mNameToPostion["炸弹人"] = "D:/IT/GroupProject/1128/mygp/炸弹人.exe";
    m_mNameToPostion["泡泡堂"] = "D:/IT/GroupProject/1128/mygp/BnB.exe";
   // m_mNameToPostion["贪吃蛇"] = "D:/IT/GroupProject/1128/mygp/炸弹人.exe";



}

mygp::~mygp()
{
    delete ui;
    if(myProcess)
        delete myProcess;
    if(Process)
        delete Process;
}

void  mygp:: slot_updatalist()
{
    sqlStr =QString("select * from t_game")  ;


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
    strcpy_s( rq.m_szUser , buf);

    //转存用户名
    strcpy_s(m_userName , rq.m_szUser);
    strTmp = password.toStdString();
    buf = (char*) strTmp.c_str();
    strcpy_s( rq.m_szPassword , buf);

//    QByteArray ba = GetMD5(password);
//    memcpy ( rq.m_szPassword , ba.data() , ba.length());

    m_client->SendData( (char*)&rq , sizeof(rq));
}

//注册槽函数
void mygp::slot_registerCommit(QString name,QString password)
{
    //发送给服务器
    STRU_REGISTER_RQ rq;
    std::string strTmp = name.toStdString();
    char* buf = (char*) strTmp.c_str();
    strcpy_s( rq.m_szUser , buf);

    strTmp = password.toStdString();
    buf = (char*) strTmp.c_str();
    strcpy_s( rq.m_szPassword , buf);

//    QByteArray ba = GetMD5(password);
//    memcpy ( rq.m_szPassword , ba.data() , ba.length());

    m_client->SendData( (char*)&rq , sizeof(rq));
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
    if(rs->result==false)
    {
        QMessageBox::information( this->m_anager , "提示", "上传完毕"  );
    }
    else{
        // 发送传输文件的信号
         emit SIG_sendfile(m_client);
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
    myProcess = new QProcess();
    myProcess->start("D:/IT/GroupProject/1128/mygp/炸弹人.exe");
    this->hide();

}


void mygp::on_pushButton_clicked()
{

    QString gname = ui->lineEdit->text();
    if(gname == NULL)
    {
        QMessageBox::about(this,"提示","请输入游戏名");
        return;
    }
    auto ite = m_mNameToPostion.begin();
    while(ite  != m_mNameToPostion.end())
    {
        qDebug()<<ite.key()<<ite.value();
        ++ite;
    }
    qDebug()<<gname;
    qDebug()<< m_mNameToPostion[gname];
   if( m_mNameToPostion.find(gname) != m_mNameToPostion.end())
   {
      myProcess = new QProcess();
       myProcess->start(m_mNameToPostion[gname]);
       if (myProcess)

         this->hide();
       else
           QMessageBox::about(this,"提示","游戏启动失败/请查看路径是否正确");

   }
   else
       QMessageBox::about(this,"提示","没找到");
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


void mygp::on_gamelist_doubleClicked(const QModelIndex &index)
{

    qDebug()<<resList2[ui->gamelist->currentIndex().row()];

//    myProcess = new QProcess();
//    myProcess->start(resList2[ui->gamelist->currentIndex().row()]);

    Process1 = new QProcess();
    Process1->start("D:\\千斤顶数值计算.exe");


}

