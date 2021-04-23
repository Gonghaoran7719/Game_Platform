#ifndef GAMESUP_H
#define GAMESUP_H

#include <QWidget>
#include "audio_read.h"
#include "audio_write.h"

namespace Ui {
class gamesup;
}

class gamesup : public QWidget
{
    Q_OBJECT

public:
    explicit gamesup(QWidget *parent = nullptr);
    ~gamesup();

public:
     Audio_Read *m_pAudioRead;
    QMap<int , Audio_Write*> m_mapIDToAudioWrite;

signals:
    void SIG_killProcess();
    void SIG_SendMsg(QString);

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    //打开音频
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();



    //void slot_closeAudio();

    void dealAudioFrame(char *buf, int len);


public:
    Ui::gamesup *ui;

};

#endif // GAMESUP_H
