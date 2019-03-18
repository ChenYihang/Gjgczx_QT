#include "csocketclient.h"
#include "csocketprocess.h"

CsocketClient::CsocketClient(int iflag, CsocketProcess *socketPce, QObject *parent) : QObject(parent)
{
    socketHoldFlag = false;
    socketClientFlag = iflag;
    audioFlag = 0;
    sp = socketPce;
    connect(this,SIGNAL(loseConnect(int)),sp,SLOT(loseConnect(int)));
    connect(this,SIGNAL(signalDisPlay(QByteArray,QString)),sp,SLOT(slotDisPlay(QByteArray,QString)));
    connect(this,SIGNAL(signalForward(QByteArray,QString)),sp,SLOT(slotForward(QByteArray,QString)));

    clientConnect.tcpClient =0;
    buffSize =800;
}

CsocketClient::~CsocketClient(){

}

void CsocketClient::dealInitStart(){

    dataProcessHandle = new CdataProcess();
    fileHandle = new CfileHandle();
    connect(dataProcessHandle,SIGNAL(signalWriteFile(QByteArray)),fileHandle,SLOT(slotWriteFile(QByteArray)));\
    connect(fileHandle,SIGNAL(fileInfoDisplay(QByteArray)),this,SLOT(fileInfoCommit(QByteArray)));

    dataProcessHandle->start();


}


void CsocketClient::socketRead(){

    QMutexLocker locker(&clientConnect.mutex);

   if(fileHandle->getType()== 0||fileHandle->getType()== 3){  // 普通数据 / 下位机主动上传 未通知上位机
       while(1){
            QByteArray socketData = clientConnect.tcpClient->read(buffSize);
            if(socketData.size()==buffSize && socketData.startsWith("#W")){
                if(audioFlag == 0){  //首次标志位
                  fileHandle->setTrans(3);
                   emit signalDisPlay("audioData",clientConnect.ip);
                  audioFlag =1;
                  }
                 dataProcessHandle->dataIn(socketData);
                 if(socketData.endsWith("%$#@"))
                     audioFlag =0;
                 if(clientConnect.tcpClient->bytesAvailable()==0)
                     break;
            }
            else if(socketData.startsWith("#")&&socketData.endsWith("#")){   //转发功能

               emit signalForward(socketData,clientConnect.ip);
                if(clientConnect.tcpClient->bytesAvailable()==0)
                    break;

            }
            else{    //普通数据接收：

                emit signalDisPlay(socketData,clientConnect.ip); //普通数据
                if(clientConnect.tcpClient->bytesAvailable()==0)
                    break;
            }

       }
    }
   else if(fileHandle->getType() == 1){ //船数据接收   主动上传
      // qDebug()<<clientConnect.tcpClient->bytesAvailable();
       QByteArray shipData = clientConnect.tcpClient->readAll();

        dataProcessHandle->dataIn(shipData);

   }
   else if(fileHandle->getType() == 2){ //脉搏数据接收  主动上传
       qDebug()<<"ava = "<<clientConnect.tcpClient->bytesAvailable();
       QByteArray pulseData = clientConnect.tcpClient->readAll();

       dataProcessHandle->dataIn(pulseData);


   }
   else if(fileHandle->getType() == 4 || fileHandle->getType() == 5){  //传感器数据
       QByteArray sensordata = clientConnect.tcpClient->read(960);

       QByteArrayMatcher matcher(QByteArray("\x4F\x5A"));
       int pos = matcher.indexIn(sensordata);
       if(sensordata.startsWith(QByteArray("\x68"))&&pos != -1)
       {
          // qDebug()<<pos;
           sensordata.remove(pos+2,960-pos-2);
           //socketData.prepend("SensorData:");
           sensordata  = "$W"+sensordata;
           sensordata.append("W$");
           //qDebug()<<sensordata;
       }
         emit signalDisPlay(sensordata,clientConnect.ip);
       if(fileHandle->getType() == 4)
       fileHandle->setTrans(0);

   }
   else if(fileHandle->getType() == 6){   //停止上传
       clientConnect.tcpClient->readAll();
       fileHandle->setTrans(0);


   }


}

//下位机断开连接
void CsocketClient::socketDisconnect(){

      QMutexLocker locker(&clientConnect.mutex);
      if(!clientConnect.tcpClient)
          return;
      disconnect(clientConnect.tcpClient, SIGNAL(readyRead()),this,SLOT(socketRead()));
      disconnect(clientConnect.tcpClient, SIGNAL(disconnected()),this,SLOT(socketDisconnect()));
      clientConnect.tcpClient->disconnect();
      clientConnect.tcpClient->close();
      clientConnect.tcpClient = 0;
      socketHoldFlag = false;

      emit loseConnect(socketClientFlag);

}

//手动断开连接

void CsocketClient::handDisconnect(){
    if(!clientConnect.tcpClient)
        return;
    disconnect(clientConnect.tcpClient, SIGNAL(readyRead()),this,SLOT(socketRead()));
    disconnect(clientConnect.tcpClient, SIGNAL(disconnected()),this,SLOT(socketDisconnect()));
    clientConnect.tcpClient->disconnect();
    clientConnect.tcpClient->close();
    clientConnect.tcpClient = 0;
    socketHoldFlag = false;
    qDebug()<<"disconnect";
}

bool CsocketClient::hold(QTcpSocket *socketTcpClient){

    QMutexLocker locker(&clientConnect.mutex);
    if(socketHoldFlag)
        return false;
    socketHoldFlag = true;
    clientConnect.tcpClient =  socketTcpClient;
    connect(clientConnect.tcpClient,SIGNAL(readyRead()),this,SLOT(socketRead()));
    connect(clientConnect.tcpClient,SIGNAL(disconnected()),this,SLOT(socketDisconnect()));
    clientConnect.ip = clientConnect.tcpClient -> peerAddress().toString();
    clientConnect.ip = clientConnect.ip.mid(clientConnect.ip.lastIndexOf(":")+1);
    qDebug()<<"accept new client:"<<clientConnect.ip;
    qDebug()<<socketClientFlag;

    return true;

}

void CsocketClient::sendData(QByteArray qbdata){
     QMutexLocker locker(&clientConnect.mutex);
     if(!clientConnect.tcpClient)
         return;
     clientConnect.tcpClient->write(qbdata);
         //qDebug()<<"error";



}


void CsocketClient::clearConnect(){

    if(!clientConnect.tcpClient)
        return;
    qDebug()<<"1";
    disconnect(clientConnect.tcpClient, SIGNAL(readyRead()),this,SLOT(socketRead()));
  qDebug()<<"2";
     disconnect(clientConnect.tcpClient, SIGNAL(disconnected()),this,SLOT(socketDisconnect()));
    clientConnect.tcpClient->disconnect();
    clientConnect.tcpClient->close();
    clientConnect.tcpClient = 0;
    socketHoldFlag = false;
}

void CsocketClient::fileInfoCommit(QByteArray fileInfo){

    emit signalDisPlay(fileInfo,clientConnect.ip);
}

