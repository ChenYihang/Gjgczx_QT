#include "csocketclient.h"
#include "csocketprocess.h"
CsocketClient::CsocketClient(int iflag, CsocketProcess *socketPce, QObject *parent) : QObject(parent),buffSize(1024)
{
    socketHoldFlag = false;
    socketClientFlag = iflag;
    audioFlag = 0;
    sp = socketPce;

    clientConnect.tcpClient =nullptr;
    udCharts = new updateCharts();
    connect(this,SIGNAL(loseConnect(int)),sp,SLOT(loseConnect(int)));

}

CsocketClient::~CsocketClient(){

}

void CsocketClient::dealInitStart(){
    dataProcessHandle = new CdataProcess();
    dataProcessHandle->start();
    udCharts = new updateCharts();

   //connect(dataProcessHandle->fileHandle,SIGNAL(signalNewFile(QString,unsigned int)),udCharts,SLOT(startPlotFile(QString,unsigned int)));
    connect(dataProcessHandle->fileHandle,SIGNAL(signalShowState(QString)),sp,SLOT(fileDealInfo(QString)));
}


void CsocketClient::socketRead(){

    QMutexLocker locker(&clientConnect.mutex);
    while(1){
        if(clientConnect.tcpClient->bytesAvailable()<buffSize)
            break;
         QByteArray socketData = clientConnect.tcpClient->read(buffSize);
         dataProcessHandle->dataIn(socketData);



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
      clientConnect.tcpClient = nullptr;
      socketHoldFlag = false;

      emit loseConnect(socketClientFlag);
      qDebug()<<"here";

}

//手动断开连接

void CsocketClient::handDisconnect(){
    if(!clientConnect.tcpClient)
        return;
    disconnect(clientConnect.tcpClient, SIGNAL(readyRead()),this,SLOT(socketRead()));
    disconnect(clientConnect.tcpClient, SIGNAL(disconnected()),this,SLOT(socketDisconnect()));
    clientConnect.tcpClient->disconnect();
    clientConnect.tcpClient->close();
    clientConnect.tcpClient = nullptr;
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
//    qDebug()<<"accept new client:"<<clientConnect.ip;
//    qDebug()<<socketClientFlag;

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
    disconnect(clientConnect.tcpClient, SIGNAL(readyRead()),this,SLOT(socketRead()));
     disconnect(clientConnect.tcpClient, SIGNAL(disconnected()),this,SLOT(socketDisconnect()));
    clientConnect.tcpClient->disconnect();
    clientConnect.tcpClient->close();
    clientConnect.tcpClient = nullptr;
    socketHoldFlag = false;
}


