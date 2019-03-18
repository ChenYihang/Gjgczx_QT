#ifndef CFILEHANDLE_H
#define CFILEHANDLE_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QDataStream>
#include "multifunc.h"
#include <QByteArrayMatcher>
class CfileHandle : public QObject
{
    Q_OBJECT
public:
    explicit CfileHandle(QObject *parent = nullptr);

signals:
    void fileInfoDisplay(QByteArray type);

public slots:
     void slotWriteFile(QByteArray fileData);
     void initFile(QString path){ filePath = path;}
     void pulseInit(unsigned int pulseNum){fileDeal.pulseSize = pulseNum;qDebug()<<fileDeal.pulseSize;}
     void setTrans(int type);
     inline int  getType(){return fileDeal.transType;}
private:
    //文件信息
    struct{
        unsigned int num;       //文件序列号

        QFile *shipFile;          //采集船数据
        QFile *pulseFile;        //脉搏数据
        QFile *audioFile;       //音频文件数据

        unsigned int pulseSize;
       int transType;
       QDataStream pulseStream;
       QDataStream shipStream;


    }fileDeal;

    static QString filePath;
   unsigned int pulseTemp;


};



#endif // CFILEHANDLE_H
