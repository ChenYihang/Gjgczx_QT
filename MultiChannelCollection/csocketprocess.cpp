#include "csocketprocess.h"

CsocketProcess *CsocketProcess::socketPro = 0;

CsocketProcess::CsocketProcess(QObject *parent) : QObject(parent)
{
    for(int slave = 0; slave<MAX_CLIENT_COUNT; slave++){
        proClientHandle[slave] = new CsocketClient(slave,this);

    }
}

CsocketProcess *CsocketProcess::instance(){
    if(!socketPro)
        socketPro = new CsocketProcess();
    return socketPro;

}

void CsocketProcess::acceptNewConnect(QTcpSocket *newConnect){
    for(int iflag = 0;iflag<MAX_CLIENT_COUNT;iflag++)
        if(proClientHandle[iflag]->hold(newConnect)){
          //信号 添加新下位机
           emit signalAddNewClient(proClientHandle[iflag]->GetIP(),iflag);
            proClientHandle[iflag]->dealInitStart();  //创建数据处理进程句柄


           break;
        }
}

void CsocketProcess::proClientDisconnect(int iflag){

    proClientHandle[iflag]->handDisconnect();
}

void CsocketProcess::loseConnect(int iflag){
    //可进行断开连接后的操作
    //...
    emit signalDelClient(iflag);//删除下位机信号

}



void CsocketProcess::clearAllConnect(){
    for(int slave = 0; slave<MAX_CLIENT_COUNT; slave++){
        proClientHandle[slave]->clearConnect();
   }

}

void CsocketProcess::fileDealInfo(QString info){

    emit signalToMainUI(info);

}
