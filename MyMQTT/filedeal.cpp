#include "filedeal.h"

static QMutex mutex;

fileDeal::fileDeal(QString Path, const QByteArray &data, enum dataType Type, QObject *parent)
{
    savePath = Path;
    rcvData = data;
    dType = Type;
}

void fileDeal::run()//解决参数指令文件上传/下发相关问题
{
    mutex.lock();//Mutex是互斥类，用于多线程访问同一个资源的时候，保证一次只有一个线程能访问该资源

    QDir dir(savePath);//QDir类使用相对或绝对文件路径来指向一个文件/目录。
    if(!dir.exists())
    {
       dir.mkpath(savePath);//新建该路径
    }
    //获取文件指令
    QStringList filter;
    filter << "*.txt";
    QList < QFileInfo > *fileInfo = new QList<QFileInfo > (dir.entryInfoList(filter));
    int fileOrder = fileInfo->count();
   // qDebug()<<fileOrder;

    if(dType == sensorData)
    {
        QFile sensorFile(savePath+QString("/SensorData_%1.temp").arg(fileOrder));  //对应到特定客户端主题文件夹内
        if(!sensorFile.isOpen())
            sensorFile.open(QIODevice::WriteOnly|QIODevice::Append);
        QTextStream writeStream(&sensorFile);
        writeStream << rcvData << endl;
        //sensorFile->write(rcvData);
        sensorFile.flush();
         qDebug()<<sensorFile.size();
         if(sensorFile.size()>=1024*1)
         {
            sensorFile.close();
            sensorFile.rename(savePath+QString("/SensorData_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss")));
         }
      }
    else if (dType == collectData )
    {
        QFile collectFile(savePath+QString("/CollectData_%1.temp").arg(fileOrder));  //对应到特定客户端主题文件夹内
        if(!collectFile.isOpen())
            collectFile.open(QIODevice::WriteOnly|QIODevice::Append);
        QByteArrayMatcher matcher(QByteArray("\xFF\xFF\xFF\xFF\xFF\xFF")+"ET");
        int pos = matcher.indexIn(rcvData);
        QDataStream shipStream;
        shipStream.setDevice(&collectFile);
        //qDebug()<<pos;
        if(pos == -1)
        {
            QString temp;
            QByteArray temp1;
            temp =  multifunc::ByteArrayToHexStr(rcvData);
            temp1 = temp.toLocal8Bit();
            shipStream.writeRawData(temp1.data(),temp1.size());
        }
        else
        {
            rcvData.remove(pos+8,rcvData.size()-pos-8);
            QString temp;
            QByteArray temp1;
            temp =  multifunc::ByteArrayToHexStr(rcvData);
            temp1 = temp.toLocal8Bit();
            shipStream.writeRawData(temp1.data(),temp1.size());
            //fileDeal.shipFile->write(fileData.remove(pos+8,fileData.size()-pos-8));
          //  qDebug()<<fileData;
            collectFile.close();
            collectFile.rename(savePath+QString("/CollectData_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss")));
        }

    }
    else if (dType == historyData )
    {
        QFile historyFile(savePath+QString("/HistoryData_%1.temp").arg(fileOrder));  //对应到特定客户端主题文件夹内
        if(!historyFile.isOpen())
            historyFile.open(QIODevice::WriteOnly|QIODevice::Append);
        QByteArrayMatcher matcher(QByteArray("\xFF\xFF\xFF\xFF\xFF\xFF")+"ET");
        int pos = matcher.indexIn(rcvData);
        QDataStream shipStream;
        shipStream.setDevice(&historyFile);
        //qDebug()<<pos;
        if(pos == -1){
            QString temp;
            QByteArray temp1;
            temp =  multifunc::ByteArrayToHexStr(rcvData);
            temp1 = temp.toLocal8Bit();
            shipStream.writeRawData(temp1.data(),temp1.size());
        }
        else
        {
            rcvData.remove(pos+8,rcvData.size()-pos-8);
            QString temp;
            QByteArray temp1;
            temp =  multifunc::ByteArrayToHexStr(rcvData);
            temp1 = temp.toLocal8Bit();
            shipStream.writeRawData(temp1.data(),temp1.size());
            //fileDeal.shipFile->write(fileData.remove(pos+8,fileData.size()-pos-8));
          //  qDebug()<<fileData;
            historyFile.close();
            historyFile.rename(savePath+QString("/HistoryData_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss")));
        }

    }
    else if(dType == armFileData)
    {
        QFile armFile(savePath+QString("/ARMFILE_%1.temp").arg(fileOrder));  //对应到特定客户端主题文件夹内
        if(!armFile.isOpen())
            armFile.open(QIODevice::WriteOnly|QIODevice::Append);
        if(rcvData.endsWith("%$#@"))
        {  //文件结束
            QByteArray x("%$#@");
            armFile.write(rcvData.mid(2,rcvData.indexOf(x)-2));
            armFile.flush();
            armFile.close();
            armFile.rename(savePath+QString("/ARMFILE_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss")));
        }
        else
        {
            armFile.write(rcvData.mid(2,794));
            armFile.flush();
        }
    }
   mutex.unlock();
   // qDebug()<<QThread::currentThreadId();
   //  qDebug()<<dType;
   // QThread::msleep(1000);
   // qDebug()<<"u";
}
