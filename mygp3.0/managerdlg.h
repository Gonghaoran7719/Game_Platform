#ifndef MANAGERDLG_H
#define MANAGERDLG_H

#include"qmytcpclient.h"
#include<QFileInfo>
#include <QWidget>
#include"Packdef.h"
#include"QObject"
#include<QThread>


namespace Ui {
class managerDlg;
}

class UploadWorker: public QObject
{
    Q_OBJECT
public slots:
    void slot_UploadFile(QString filePath );
};

class managerDlg : public QWidget
{
    Q_OBJECT

public:
    explicit managerDlg(QMyTcpClient*p,QWidget *parent );
    ~managerDlg();

private slots:
    void on_pb_select_clicked();

    void on_pb_up_clicked();

    void on_pb_del_clicked();

public slots:
  //  bool InitNetWork(char* szbuf , int nLen);

    void slot_UploadFile(QString filePath);
    void slot_updateProcess( qint64 cur , qint64 max);
    void   clear();

signals:
    void SIG_UploadFile(QString   );//上传文件信息
    void  SIG_updateProcess(qint64,qint64);


public:
    Ui::managerDlg *ui;
    QFileInfo fi;
    QMyTcpClient*tcp;
    int filesize ;//传输文件的大小
    int sendsize;//暂时发送的大小
    QFile *file;
    QString filePath;
    QThread * m_uploadThread;
    UploadWorker* m_uploadWorker;
    char filename[MAX_SIZE];
public:
    static managerDlg* m_anager;
    bool  flag;
    int count;
};

#endif // MANAGERDLG_H
