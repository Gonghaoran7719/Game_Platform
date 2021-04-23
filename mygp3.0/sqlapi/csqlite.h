#ifndef CSQLITE_H
#define CSQLITE_H

#include<QStringList>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlDriver>
#include<QSqlRecord>
#include<QSqlError>
#include<QMutex>
#include<QTextCodec>
#include<QDebug>

class CSqlite
{
public:
    CSqlite();
    ~CSqlite(){DisConnect();}

    void ConnectSql(QString db);//传入数据库绝对路径, 建立连接
    void DisConnect();//断开数据库连接

    //查询 参数介绍: sqlstr 是执行的sql语句, nColumn表示你要查找的列数, list用来存储查找结果
    bool SelectSql(QString sqlStr , int nColumn , QStringList & list);//查找操作

    //更新: 删除, 插入 , 修改
    bool UpdateSql(QString sqlStr);

private:

    QSqlDatabase m_db;
    QMutex m_mutex;
};

#endif // CSQLITE_H
