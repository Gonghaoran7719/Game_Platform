#ifndef MANAGERDLG_H
#define MANAGERDLG_H

#include"qmytcpclient.h"
#include<QFileInfo>
#include <QWidget>
#include"Packdef.h"

namespace Ui {
class managerDlg;
}

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


private:
    Ui::managerDlg *ui;
    QFileInfo fi;
    QMyTcpClient*tcp;
    int filesize ;//传输文件的大小
    int sendsize;//暂时发送的大小
    QFile file;
};

#endif // MANAGERDLG_H
