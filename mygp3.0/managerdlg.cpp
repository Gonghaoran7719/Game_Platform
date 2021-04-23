#include "managerdlg.h"
#include "ui_managerdlg.h"
#include<QFileDialog.h>
#include<QCoreApplication.h>
 #include <QByteArray>
#include <QCryptographicHash>
#include <QFile>
#include <QDebug>
#include <QMessageBox>

managerDlg::managerDlg(QMyTcpClient*p,QWidget *parent ) :
    QWidget(parent),
    ui(new Ui::managerDlg)
{
    ui->setupUi(this);
    tcp=p;
    filesize = 0;
    sendsize = 0;
    m_anager=this;
    memset(filename,0,MAX_SIZE);

flag=false;
count=0;
   connect( this , SIGNAL( SIG_updateProcess(qint64,qint64) ) ,
             this , SLOT( slot_updateProcess(qint64,qint64)) );

    m_uploadWorker = new UploadWorker;
    m_uploadThread = new QThread;
    connect( this , SIGNAL( SIG_UploadFile(QString )) ,
    m_uploadWorker , SLOT( slot_UploadFile(QString )) );
    m_uploadWorker ->moveToThread( m_uploadThread );
    m_uploadThread ->start();



}
managerDlg*managerDlg::m_anager=NULL;

managerDlg::~managerDlg()
{
    delete ui;
    delete   m_uploadThread;
    m_uploadThread=NULL;
     delete  m_uploadWorker;
    m_uploadWorker=NULL;
}
//更新进度条
void managerDlg::slot_updateProcess(qint64 cur, qint64 max)
{
    ui->progressBar->setMaximum( max);
    ui->progressBar->setValue( cur );
}

void managerDlg::slot_UploadFile(QString filePath )//上传文件信息
{
    //兼容中文
    if(flag==false&&count==0)
    {
    QFileInfo info(filePath);
    //游戏名字 XX.exe
    QString FileName = info.fileName();
    std::string strName = FileName.toStdString();
    const char* file_name = strName.c_str();
    //游戏大小
     filesize=info.size();
     qDebug()<<"filesize";
     qDebug()<<filesize;

//获得文件类型
//    QByteArray bt =  filePath.right( filePath.length() - filePath.lastIndexOf('.') -1 ).toLatin1();
//    memcpy(rq.m_szFileType  ,bt.data() , bt.length() );

    QString str_version = ui->game_version->text();//版本

    QString str_os = ui->gam_os->text();//系统


     STRU_GAMEUP_RQ rq;//发送请求包
     rq.m_nType=DEF_PACK_GAMEUP_RQ;

      strcpy_s( rq.m_name , MAX_SIZE,(char*)file_name);//游戏名字

         strcpy_s( filename , MAX_SIZE,(char*)file_name);//游戏名字
      std::string strversion = str_version.toStdString();
      char* str_ver=(char*) strversion.c_str();
      strcpy_s( rq.m_version , MAX_SIZE,str_ver);//游戏版本
      qDebug()<<rq.m_version;

      std::string stros = str_os.toStdString();
       char* str_oos=(char*) stros.c_str();
      strcpy_s( rq.m_os , MAX_SIZE,str_oos);//适用系统
      qDebug()<<rq.m_os;

      QFile theFile(filePath);
          theFile.open(QIODevice::ReadOnly);
          QByteArray ba = QCryptographicHash::hash(theFile.readAll(), QCryptographicHash::Md5);
          theFile.close();
         QByteArray baa= ba.toHex();
         //strcpy(rq.m_md5,ba.data());
          memcpy ( rq.m_md5 , baa.data() , baa.length());//md5值
             qDebug()<<rq.m_md5;
        rq.filesize=filesize;
        rq.m_UserId=1;

     tcp->SendData( (char*)&rq , sizeof(rq));//发送文件信息
     Sleep(500);
    }

     if(flag==true)
     {
         return ;
     }

     file= new QFile(filePath);
    if( file-> open(QIODevice::ReadOnly))
    {
        while(1)
        {
              STRU_GAMEUPUP_RQ rrq;
               // memcpy ( rrq.m_md5 , ba.data() , ba.length());//md5值
                strcpy( rrq.m_filename , filename);
               rrq.m_nUserId=1;
             int64_t res =file->read(  rrq. m_file , MAX_CONTENT_LEN);
             rrq.blocksize=res;
             sendsize += res;
             tcp->SendData( (char* ) &rrq , sizeof( rrq));
             emit SIG_updateProcess( sendsize , filesize);

             qDebug()<<"上传文件2";
             if( sendsize >=filesize)
             {
                file->close();
                 delete file;
                 file = NULL;
                 break;
             }
        }
    }

clear();
//count=0;
//flag=false;

}

//点击浏览
void managerDlg::on_pb_select_clicked()
{
    QString path = QFileDialog::getOpenFileName( this, "打开", "./","(*.exe)");
    if( path.isEmpty() ) return ;
    filePath=path;
    ui->gamepath->setText( path );
}

void managerDlg::on_pb_up_clicked()//上传
{
qDebug()<<"上传文件";
count=0;
flag=false;
   emit   managerDlg::SIG_UploadFile(filePath );
}


void managerDlg::on_pb_del_clicked()//删除
{
  QString str_name=ui->gamename->text();
 STRU_GAMEDEL_RQ  rq;
 rq.m_nType=DEF_PACK_GAMEDEL_RQ;   //包类型
 // int m_gameID;
  std::string stros = str_name.toStdString();
  strcpy_s(rq.m_name, (char*) stros.c_str());

     if(str_name!=nullptr)
     {
         //发送游戏名字

         tcp->SendData( (char*)&rq , sizeof(rq));

     }
     qDebug()<<rq.m_name;
     ui->gamename->setText(" ");
}


//工作者 上传流程
void UploadWorker::slot_UploadFile(QString filePath )
{
  managerDlg:: m_anager->  slot_UploadFile( filePath  );
}
//清除文件框
void managerDlg ::clear()
{
    ui->gamepath->setText("");
    ui->game_version->setText("");
    ui->gam_os->setText("");
    ui->progressBar->setValue( 0 );
}
