#include "cfilehandle.h"

QString CfileHandle::filePath = QDir::currentPath();  //静态成员需要在类定义体外定义
CfileHandle::CfileHandle(QObject *parent) : QObject(parent)
{
    fileDeal.shipFile =0;
    fileDeal.pulseFile =0;
    fileDeal.audioFile =0;

    fileDeal.num = 0;
    pulseTemp =0;
    fileDeal.transType =0;
}


void CfileHandle::setTrans(int type){

    fileDeal.transType = type;

    if(fileDeal.transType == 1){
        fileDeal.shipFile = new QFile(filePath+"/ship"+QDateTime::currentDateTime().toString("yyyyMMddhhmmss.")+QString("%1").arg((fileDeal.num),10,10,QChar('0'))+QString(".txt"));
        fileDeal.shipFile->open(QIODevice::WriteOnly);
        fileDeal.shipStream.setDevice(fileDeal.shipFile);
        emit fileInfoDisplay("CREATFILE");
    }
    else if(fileDeal.transType ==2){
        fileDeal.pulseFile = new QFile(filePath+"/pulse"+QDateTime::currentDateTime().toString("yyyyMMddhhmmss.")+QString("%1").arg((fileDeal.num),10,10,QChar('0'))+QString(".txt"));
        fileDeal.pulseFile->open(QIODevice::WriteOnly);
        fileDeal.pulseStream.setDevice(fileDeal.pulseFile);
        emit fileInfoDisplay("CREATFILE");
    }
    else if(fileDeal.transType ==3){
        fileDeal.audioFile = new QFile(filePath+"/audio"+QDateTime::currentDateTime().toString("yyyyMMddhhmmss.")+QString("%1").arg((fileDeal.num),10,10,QChar('0'))+QString(".txt"));
        fileDeal.audioFile->open(QIODevice::WriteOnly);
        emit fileInfoDisplay("CREATFILE");
    }
    else
        return;
    fileDeal.num ++;


}


void CfileHandle::slotWriteFile(QByteArray fileData){

    //qDebug()<<fileData;
    if(fileDeal.transType == 1){  //船数据接收
        QByteArrayMatcher matcher(QByteArray("\xFF\xFF\xFF\xFF\xFF\xFF")+"ET");
        int pos = matcher.indexIn(fileData);
        //qDebug()<<pos;
        if(pos == -1){
            QString temp;
            QByteArray temp1;
            temp =  multifunc::ByteArrayToHexStr(fileData);
            temp1 = temp.toLocal8Bit();
            fileDeal.shipStream.writeRawData(temp1.data(),temp1.size());
//            fileDeal.shipFile->write(fileData);
           // fileDeal.shipFile->flush();
        }
        else{
            fileDeal.transType =0;  //重置标志位
            fileData.remove(pos+8,fileData.size()-pos-8);
            QString temp;
            QByteArray temp1;
            temp =  multifunc::ByteArrayToHexStr(fileData);
            temp1 = temp.toLocal8Bit();
            fileDeal.shipStream.writeRawData(temp1.data(),temp1.size());
            //fileDeal.shipFile->write(fileData.remove(pos+8,fileData.size()-pos-8));
          //  qDebug()<<fileData;
            if(fileDeal.shipFile)
                fileDeal.shipFile->flush();
                delete fileDeal.shipFile;
            emit fileInfoDisplay("SHIPDATA");
                qDebug()<<"writed shipData";
        }

     }
    else if(fileDeal.transType == 2){  //脉搏数据接收
        //需要转换写
        QString temp;
        QByteArray temp1;
        temp =  multifunc::ByteArrayToHexStr(fileData);
        temp1 = temp.toLocal8Bit();
        fileDeal.pulseStream.writeRawData(temp1.data(),temp1.size());

        pulseTemp += fileData.size();
        if(pulseTemp == fileDeal.pulseSize){//文件结束
             fileDeal.transType =0;  //重置标志位
             if(fileDeal.pulseFile){
                 fileDeal.pulseFile->flush();
                 delete fileDeal.pulseFile;
             }
             pulseTemp = 0;
             emit fileInfoDisplay("PULSEDATA");
            // qDebug()<<"writed pulseData";
        }

    }
    else if(fileDeal.transType == 3){

        if(fileData.endsWith("%$#@")){  //文件结束
            QByteArray x("%$#@");
            fileDeal.audioFile->write(fileData.mid(2,fileData.indexOf(x)-2));
           // qDebug()<<fileData.mid(2,fileData.indexOf(x)-2);
            fileDeal.audioFile->flush();
               fileDeal.transType =0;  //重置标志位
               if(fileDeal.audioFile){
                   fileDeal.audioFile->flush();
                   delete fileDeal.audioFile;
           }
               emit fileInfoDisplay("AUDIODATA");

        }
        else{
            fileDeal.audioFile->write(fileData.mid(2,794));
            fileDeal.audioFile->flush();

        }


    }

}

