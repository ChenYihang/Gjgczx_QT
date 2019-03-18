#include "cfilehandle.h"

QString CfileHandle::filePath = QDir::currentPath();  //静态成员需要在类定义体外定义
CfileHandle::CfileHandle(QObject *parent) : QObject(parent),startFlag(true),DataSize(0),fileCount(1),fileSize(0)
{

}


void CfileHandle::WriteFile(QByteArray fileData){

    if(startFlag == true){
       // qDebug()<<fileData;
        if(fileData.startsWith(QByteArray("\x00"))&&fileData.size()==1024){
            bool ok;
            DataSize = fileData.mid(1,4).toHex().toInt(&ok,16);
            QString fileName = filePath+"/Data"+QDateTime::currentDateTime().toString("yyyyMMddhhmmss.")+QString("%1").arg((fileCount),10,10,QChar('0'))+QString(".txt");
            qDebug()<<"DataSize = " <<DataSize;
            emit signalShowState("正在接收数据,数据大小: "+QString::number(DataSize));
            file = new QFile(fileName);
            file->open(QIODevice::WriteOnly);
            startFlag = false;

        }

    }
    else{

        file->write(fileData,fileData.size());
        file->flush();
        fileSize += fileData.size();
        if(fileSize==16777216){  //每16M形成一个文件
            qDebug()<<"close file";
            emit signalShowState("已接受数据大小: "+QString::number(fileSize*fileCount));
            file->close();
            if(16777216*fileCount == DataSize) //所有数据接收完成
            {
                fileCount =1;
                fileSize = 0;
                startFlag = true;
                emit signalShowState("数据接收完成!");

                return ;
            }

            fileCount ++;
            fileSize = 0;

            QString fileName = filePath+"/Data"+QDateTime::currentDateTime().toString("yyyyMMddhhmmss.")+QString("%1").arg((fileCount),10,10,QChar('0'))+QString(".txt");
            file = new QFile(fileName);
            file->open(QIODevice::WriteOnly);




        }


    }
}

