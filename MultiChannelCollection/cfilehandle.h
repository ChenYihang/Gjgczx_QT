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


    void signalShowState(QString info);

public slots:
     void WriteFile(QByteArray fileData);
     void initFile(QString path){ filePath = path;}
private:
    //文件信息

   QString fileName;
   unsigned int DataSize;
   unsigned int fileSize;
   static QString filePath;
   bool startFlag;

   QFile *file;          //采集船数据
   unsigned int fileCount;



};



#endif // CFILEHANDLE_H
