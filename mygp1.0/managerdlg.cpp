#include "managerdlg.h"
#include "ui_managerdlg.h"
#include<QFileDialog.h>
#include<QCoreApplication.h>
 #include <QByteArray>
#include <QCryptographicHash>
#include <QFile>
#include <QDebug>

managerDlg::managerDlg(QMyTcpClient*p,QWidget *parent ) :
    QWidget(parent),
    ui(new Ui::managerDlg)
{
    ui->setupUi(this);
    tcp=p;
    filesize = 0;
    sendsize = 0;

//   /* void SIG_sendfile( char* szbuf , int nLen );
//public slots:
//  //  bool InitNetWork(char* szbuf , int nLen);
//    bool sendfile( char* szbuf , int nLen );
//    connect( m_login ,SIGNAL(SIG_loginCommit(QString,QString)) ,
//             this , SLOT(slot_loginCommit(QString,QString))*/ );

}

managerDlg::~managerDlg()
{
    delete ui;
}

  bool managerDlg::sendfile( QMyTcpClient*m_client)
  {


      filesize = fi.size();
      STRU_GAMEUPUP_RQ rq;
       rq.m_nType=DEF_PACK_GAMEUPUP_RQ;   //包类型
       QString  filename = fi.fileName();
       std::string strname = filename.toStdString();
       strcpy_s( rq.name , (char*) strname.c_str());//游戏名字
                   //char md5[80];



          //设置进度条最大值
          ui->progressBar->setMaximum(filesize);

          //打开文件准备读取数据发送
          file.setFileName(ui->gamepath->text());
          file.open(QIODevice::ReadOnly);
      while(sendsize < filesize)
      {
          if(sendsize < filesize)
              {
                  rq.offest=sendsize;
                  QByteArray array = file.read(1020);//读取一段内容
                  QString  s = QString(array );
                  std::string str = s.toStdString();
                  strcpy_s( rq.m_file , (char*) str.c_str());

                     //md5
                  tcp->SendData( (char*)&rq , sizeof(rq));
                  sendsize += array.size();
                  file.seek(sendsize);
                  //设置进度条
                  ui->progressBar->setValue(sendsize);
              }
              if(sendsize == filesize)
              {
                  //file.frush();
                  //file.close();//关闭文件
              }

      }
      return true;

  }

void managerDlg::on_pb_select_clicked()
{
   //  QStringList path = QFileDialog::getOpenFileNames( this,"选择上传的游戏","./");
  //  QString  path = QCoreApplication:: applicationFilePath();//获取当前应用程序的路径

        QString file_full, file_name, file_path;
        //QFileInfo fi;//文件路径
    //    file_full = QFileDialog::getOpenFileName(this);
    //      打开文件        获取文件路径
        file_full = QFileDialog::getSaveFileName(this,tr("Open File"),tr("*.*"));
    //      保存文件        获取文件路径  “Save File” 对话框名        “*.* 默认文件名
        fi= QFileInfo(file_full);
    //      显示文件路径
        file_name = fi.fileName();
        file_path = fi.absolutePath();
        ui->gamepath->setText(file_path+"//"+file_name);

}

void managerDlg::on_pb_up_clicked()//上传
{
    //从linetext中得到文件的路径
   // QString str_path = ui->gamepath->toHtml();
   // QString str_version = ui->game_version->toHtml();
   // QString str_os = ui->game_os->toHtml();
    QString str_path = ui->gamepath->text();
    QString str_version = ui->game_version->text();
    QString str_os = ui->gam_os->text();
     QString  file_name = fi.fileName();
               filesize = fi.isFile();
     //获取文件信息  md5值 大小 名字  适合的系统 版本
    QFile localFile(str_path);
    if (!localFile.open(QFile::ReadOnly))
    {
        qDebug() << "file open error.";
        return ;
    }

    QCryptographicHash ch(QCryptographicHash::Md5);

    quint64 totalBytes = 0;
    quint64 bytesWritten = 0;
    quint64 bytesToWrite = 0;
    quint64 loadSize = 1024 * 4;
    QByteArray buf;

    totalBytes = localFile.size();
    bytesToWrite = totalBytes;

    while (1)
    {
        if(bytesToWrite > 0)
        {
            buf = localFile.read(qMin(bytesToWrite, loadSize));
            ch.addData(buf);
            bytesWritten += buf.length();
            bytesToWrite -= buf.length();
            buf.resize(0);
        }
        else
        {
            break;
        }

        if(bytesWritten == totalBytes)
        {
            break;
        }
    }
    localFile.close();
    QByteArray md5 = ch.result();
     QString MD5;
     MD5.append(md5.toHex());//将MD5  16进制的数转换为字符串

     STRU_GAMEUP_RQ rq;//发送请求包
     rq.m_nType=DEF_PACK_GAMEUP_RQ;
      std::string strname = file_name.toStdString();
      strcpy_s( rq.m_name , (char*) strname.c_str());//游戏名字

      std::string strversion = str_version.toStdString();
      strcpy_s( rq.m_version , (char*) strversion.c_str());

      std::string stros = str_os.toStdString();
      strcpy_s( rq.m_os , (char*) stros.c_str());

      std::string strmd5 = MD5.toStdString();
      strcpy_s( rq.m_md5 , (char*) strmd5.c_str());

 //    QByteArray ba = GetMD5(password);
 //    memcpy ( rq.m_szPassword , ba.data() , ba.length());// 遍历比较字符串

     tcp->SendData( (char*)&rq , sizeof(rq));

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
