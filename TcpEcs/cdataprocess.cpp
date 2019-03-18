#include "cdataprocess.h"
#include <qDebug>


CdataProcess::CdataProcess():freeData(buffSize)  //只能用这种构造方式对该信号量初始化？//Qsemaphore缺点?
{
 qDebug()<<"creat dataProcess handle";
 //dataHandle.transType = 0;
// dataHandle.endFlag = 0;
 qDebug()<<freeData.available();
}

CdataProcess::~CdataProcess(){

}


void CdataProcess::run(){
    while(1){
        dataOut();
    }

}

void CdataProcess::dataIn(QByteArray sInData){  //producer

     QMutexLocker locker(&(filein_mutex));
    freeData.acquire();
//   qDebug()<<"fr"<<freeData.available();
   // qDebug()<<sData;
    socketData.enqueue(sInData);
    usedData.release();

//   qDebug()<<usedData.available();


}

void CdataProcess::dataOut(){ //consumer

    QMutexLocker locker(&(fileout_mutex));

    usedData.acquire();
    QByteArray sOutdata;
    sOutdata = socketData.dequeue();
    qDebug()<<"deq = "<<sOutdata.size();


    emit signalWriteFile(sOutdata);

   // qDebug()<<"to write";
    freeData.release();
   // qDebug()<<"endflag"<<dataHandle.endFlag;


}
