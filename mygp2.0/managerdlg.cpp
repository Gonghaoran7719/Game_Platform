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


//   /* void SIG_sendfile( char* szbuf , int nLen );
//public slots:
//  //  bool InitNetWork(char* szbuf , int nLen);
//    bool sendfile( char* szbuf , int nLen );
//    connect( m_login ,SIGNAL(SIG_loginCommit(QString,QString)) ,
//             this , SLOT(slot_loginCommit(QString,QString))*/ );

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
}
//更新进度条
void managerDlg::slot_updateProcess(qint64 cur, qint64 max)
{
    ui->progressBar->setMaximum( max);
    ui->progressBar->setValue( cur );
}

void managerDlg::slot_UploadFile(QString filePath )
{


    //兼容中文
    QFileInfo info(filePath);
    //游戏名字 XX.exe
    QString FileName = info.fileName();
    std::string strName = FileName.toStdString();
    const char* file_name = strName.c_str();
    //游戏大小
     filesize=info.size();

//获得文件类型
//    QByteArray bt =  filePath.right( filePath.length() - filePath.lastIndexOf('.') -1 ).toLatin1();
//    memcpy(rq.m_szFileType  ,bt.data() , bt.length() );

    QString str_version = ui->game_version->text();//版本

    QString str_os = ui->gam_os->text();//系统

     //获取文件信息  md5值 大小 名字  适合的系统 版本
//    QFile localFile(filePath);

//    if (!localFile.open(QFile::ReadOnly))
//    {
//        qDebug() << "file open error.";
//        return ;
//    }

//    QCryptographicHash ch(QCryptographicHash::Md5);

//    quint64 totalBytes = 0;
//    quint64 bytesWritten = 0;
//    quint64 bytesToWrite = 0;
//    quint64 loadSize = 1024 * 4;
//    QByteArray buf;

//    totalBytes = localFile.size();
//    bytesToWrite = totalBytes;

//    while (1)
//    {
//        if(bytesToWrite > 0)
//        {
//            buf = localFile.read(qMin(bytesToWrite, loadSize));
//            ch.addData(buf);
//            bytesWritten += buf.length();
//            bytesToWrite -= buf.length();
//            buf.resize(0);
//        }
//        else
//        {
//            break;
//        }

//        if(bytesWritten == totalBytes)
//        {
//            break;
//        }
//    }
//    localFile.close();
//    QByteArray md5 = ch.result();
    // QString MD5;
    // MD5.append(md5.toHex());//将MD5  16进制的数转换为字符串

     STRU_GAMEUP_RQ rq;//发送请求包
     rq.m_nType=DEF_PACK_GAMEUP_RQ;

      strcpy_s( rq.m_name , MAX_SIZE,(char*)file_name);//游戏名字

      std::string strversion = str_version.toStdString();
      char* str_ver=(char*) strversion.c_str();
      strcpy_s( rq.m_version , MAX_SIZE,str_ver);
      qDebug()<<rq.m_version;

      std::string stros = str_os.toStdString();
       char* str_oos=(char*) stros.c_str();
      strcpy_s( rq.m_os , MAX_SIZE,str_oos);
      qDebug()<<rq.m_os;

      QFile theFile(filePath);
          theFile.open(QIODevice::ReadOnly);
          QByteArray ba = QCryptographicHash::hash(theFile.readAll(), QCryptographicHash::Md5);
          theFile.close();
strcpy(rq.m_md5,ba.data());


//      std::string strmd5 = MD5.toStdString();
//      strcpy_s( rq.m_md5 , (char*) strmd5.c_str());
      rq.filesize=filesize;

      qDebug()<<"上传文件1";

     tcp->SendData( (char*)&rq , sizeof(rq));

     file= new QFile(filePath);
    if( file-> open(QIODevice::ReadOnly))
    {
        while(1)
        {
              STRU_GAMEUPUP_RQ rrq;
             rrq.m_nType=DEF_PACK_GAMEUPUP_RQ;   //包类型
             rrq. offest=sendsize;
             strcpy_s(rrq. name , (char*)file_name);//游戏名字

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
    //QMessageBox::about( NULL,"提示","上传成功");

}


  bool managerDlg::sendfile( QMyTcpClient*m_client)
  {
//      filesize = fi.size();
//      STRU_GAMEUPUP_RQ rq;
//       rq.m_nType=DEF_PACK_GAMEUPUP_RQ;   //包类型
//       QString  filename = fi.fileName();
//       std::string strname = filename.toStdString();
//       strcpy_s( rq.name , (char*) strname.c_str());//游戏名字
//                   //char md5[80];

//          //设置进度条最大值
//          ui->progressBar->setMaximum(filesize);

//          //打开文件准备读取数据发送
//          file.setFileName(ui->gamepath->text());
//          file.open(QIODevice::ReadOnly);
//      while(sendsize < filesize)
//      {
//          if(sendsize < filesize)
//              {
//                  rq.offest=sendsize;
//                  QByteArray array = file.read(1020);//读取一段内容
//                  QString  s = QString(array );
//                  std::string str = s.toStdString();
//                  strcpy_s( rq.m_file , (char*) str.c_str());

//                     //md5
//                  tcp->SendData( (char*)&rq , sizeof(rq));
//                  sendsize += array.size();
//                  file.seek(sendsize);
//                  //设置进度条
//                  ui->progressBar->setValue(sendsize);
//              }
//              if(sendsize == filesize)
//              {
//                  //file.frush();
//                  //file.close();//关闭文件
//              }

//      }
//      return true;

  }
//点击浏览
void managerDlg::on_pb_select_clicked()
{
    QString path = QFileDialog::getOpenFileName( this, "打开", "./","(*.exe)");
    if( path.isEmpty() ) return ;
    filePath=path;
    ui->gamepath->setText( path );
//        QString file_full, file_name, file_path;
//        //QFileInfo fi;//文件路径
//    //    file_full = QFileDialog::getOpenFileName(this);
//    //      打开文件        获取文件路径
//        file_full = QFileDialog::getSaveFileName(this,tr("Open File"),tr("*.*"));
//    //      保存文件        获取文件路径  “Save File” 对话框名        “*.* 默认文件名
//        fi= QFileInfo(file_full);
//    //      显示文件路径
//        file_name = fi.fileName();
//        file_path = fi.absolutePath();
//        ui->gamepath->setText(file_path+"//"+file_name);

}

void managerDlg::on_pb_up_clicked()//上传
{
qDebug()<<"上传文件";
   emit   managerDlg::SIG_UploadFile(filePath );
}


void managerDlg::on_pb_del_clicked()//删除
{
  QString str_name=ui->pb_del->text();
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
}


//工作者 上传流程
void UploadWorker::slot_UploadFile(QString filePath )
{
  managerDlg:: m_anager->  slot_UploadFile( filePath  );
}
