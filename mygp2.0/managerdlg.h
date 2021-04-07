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
    bool sendfile( QMyTcpClient*m_client);
    void slot_UploadFile(QString filePath);
    void slot_updateProcess( qint64 cur , qint64 max);

signals:
    void SIG_UploadFile(QString   );
    void  SIG_updateProcess(qint64,qint64);


private:
    Ui::managerDlg *ui;
    QFileInfo fi;
    QMyTcpClient*tcp;
    int filesize ;//传输文件的大小
    int sendsize;//暂时发送的大小
    QFile *file;
    QString filePath;
    QThread * m_uploadThread;
    UploadWorker* m_uploadWorker;
public:
    static managerDlg* m_anager;
};

#endif // MANAGERDLG_H
