#include "QMyTcpClient.h"

QMyTcpClient::QMyTcpClient(QObject *parent) : QObject(parent)
{
    m_pTcp = new TCPNet(this);
}

QMyTcpClient::~QMyTcpClient()
{
    m_pTcp->UnInitNetWork();
}
#include <qdebug.h>
bool QMyTcpClient::InitNetWork(char* szBufIP , unsigned short port)
{
        qDebug()<<"连接成功";
    return m_pTcp->InitNetWork(szBufIP,port);

}



void QMyTcpClient::UnInitNetWork()
{
    m_pTcp->UnInitNetWork();
}

int QMyTcpClient::SendData(char* szbuf,int nLen)
{
    return m_pTcp->SendData(szbuf,nLen);
}

int QMyTcpClient::ConnectToServer( char* szBufIP)
{
    return m_pTcp->ConnectToServer(szBufIP);
}


void QMyTcpClient::DealData(char* szbuf , int nLen)
{
    emit SIG_ReadyData(szbuf,nLen);//发送准备读的信号
}


