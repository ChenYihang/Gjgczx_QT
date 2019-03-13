#include "filedeal.h"



fileDeal::fileDeal(QString Path, const QByteArray &data, enum dataType Type, QObject *parent)
{
    savePath = Path;
    rcvData = data;
    dType = Type;
}

void fileDeal::run(){

   // qDebug()<<dType;
    QDir dir(savePath);
    if(!dir.exists()){
        dir.mkpath(savePath);
    }
    //get file order
    QStringList filter;
    filter<<"*.txt";
    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir.entryInfoList(filter));
    int fileOrder = fileInfo->count();
   // qDebug()<<fileOrder;

    if(dType == sensorData){

        QFile *sensorFile = new QFile(savePath+QString("/SensorData_%1.temp").arg(fileOrder));  //对应到特定客户端主题文件夹内
        if(!sensorFile->isOpen())
            sensorFile->open(QIODevice::WriteOnly|QIODevice::Append);
        QTextStream writeStream(sensorFile);
        writeStream << rcvData << endl;
        //sensorFile->write(rcvData);
        sensorFile->flush();
         qDebug()<<sensorFile->size();
         if(sensorFile->size()>=1024*1){
            sensorFile->close();
            sensorFile->rename(savePath+QString("/SensorData_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss.")));
            delete sensorFile;
             return;
         }
         delete  sensorFile;

      }
    else if (dType == collectData ) {

        QFile *collectFile = new QFile(savePath+QString("/CollectData_%1.temp").arg(fileOrder));  //对应到特定客户端主题文件夹内
        if(!collectFile->isOpen())
            collectFile->open(QIODevice::WriteOnly|QIODevice::Append);
        QByteArrayMatcher matcher(QByteArray("\xFF\xFF\xFF\xFF\xFF\xFF")+"ET");
        int pos = matcher.indexIn(rcvData);
        QDataStream shipStream;
        shipStream.setDevice(collectFile);
        //qDebug()<<pos;
        if(pos == -1){
            QString temp;
            QByteArray temp1;
            temp =  multifunc::ByteArrayToHexStr(rcvData);
            temp1 = temp.toLocal8Bit();
            shipStream.writeRawData(temp1.data(),temp1.size());
        }
        else{
            rcvData.remove(pos+8,rcvData.size()-pos-8);
            QString temp;
            QByteArray temp1;
            temp =  multifunc::ByteArrayToHexStr(rcvData);
            temp1 = temp.toLocal8Bit();
            shipStream.writeRawData(temp1.data(),temp1.size());
            //fileDeal.shipFile->write(fileData.remove(pos+8,fileData.size()-pos-8));
          //  qDebug()<<fileData;
            collectFile->close();
            collectFile->rename(savePath+QString("/CollectData_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss.")));
            delete collectFile;
            return;
        }
        delete collectFile;

    }
    else if (dType == historyData ) {

        QFile *historyFile = new QFile(savePath+QString("/HistoryData_%1.temp").arg(fileOrder));  //对应到特定客户端主题文件夹内
        if(!historyFile->isOpen())
            historyFile->open(QIODevice::WriteOnly|QIODevice::Append);
        QByteArrayMatcher matcher(QByteArray("\xFF\xFF\xFF\xFF\xFF\xFF")+"ET");
        int pos = matcher.indexIn(rcvData);
        QDataStream shipStream;
        shipStream.setDevice(historyFile);
        //qDebug()<<pos;
        if(pos == -1){
            QString temp;
            QByteArray temp1;
            temp =  multifunc::ByteArrayToHexStr(rcvData);
            temp1 = temp.toLocal8Bit();
            shipStream.writeRawData(temp1.data(),temp1.size());
        }
        else{
            rcvData.remove(pos+8,rcvData.size()-pos-8);
            QString temp;
            QByteArray temp1;
            temp =  multifunc::ByteArrayToHexStr(rcvData);
            temp1 = temp.toLocal8Bit();
            shipStream.writeRawData(temp1.data(),temp1.size());
            //fileDeal.shipFile->write(fileData.remove(pos+8,fileData.size()-pos-8));
          //  qDebug()<<fileData;
            historyFile->close();
            historyFile->rename(savePath+QString("/HistoryData_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss.")));
            delete historyFile;
            return;
        }
        delete historyFile;

    }
    else if(dType == armFileData){


        QFile *armFile = new QFile(savePath+QString("/ARMFILE_%1.temp").arg(fileOrder));  //对应到特定客户端主题文件夹内
        if(!armFile->isOpen())
            armFile->open(QIODevice::WriteOnly|QIODevice::Append);
        if(rcvData.endsWith("%$#@")){  //文件结束
            qDebug()<<"here";
            QByteArray x("%$#@");
            armFile->write(rcvData.mid(2,rcvData.indexOf(x)-2));
            armFile->flush();
            armFile->close();
            armFile->rename(savePath+QString("/ARMFILE_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss.")));
            delete armFile;
            return;

        }
        else{
            armFile->write(rcvData.mid(2,794));
            armFile->flush();
        }
        delete armFile;
    }


   // qDebug()<<QThread::currentThreadId();
  //  qDebug()<<dType;
    // QThread::msleep(1000);
}
