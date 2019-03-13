#include <SourceManager/socketclient.h>
#include <SourceManager/sourcemanager.h>

SocketClient::SocketClient(int flag,SourceManager *manager,QObject *parent) : QObject(parent){
    sc_isHold = false;
    sc_flag = flag;
    sc_SM = manager;
    connect(this,SIGNAL(LoseConnect(int)),sc_SM,SLOT(LoseConnect(int)));
    sc_MQ.mq = MessageQueue::GetInstance();
    sc_MQ.message.flag = sc_flag;
    sc_MQ.message.content[0] = '\0';
    sc_MQ.isError = false;
    sc_MQ.lessData = 0;
    sc_clientConnect.sc_tcpClient = 0;
}
SocketClient::~SocketClient(){

}

void SocketClient::DoSocketRead(){
    while(1){
        QMutexLocker locker(&sc_clientConnect.mutex);
        if(sc_MQ.isError){
            if(sc_clientConnect.sc_tcpClient->bytesAvailable() < sc_MQ.lessData)
                break;
            sc_clientConnect.sc_tcpClient->read(sc_MQ.message.content,sc_MQ.lessData);
            sc_MQ.lessData = 0;
            sc_MQ.isError = false;
        }else{
            if(sc_clientConnect.sc_tcpClient->bytesAvailable() < MQ_MESSAGE_LENGHT)
                break;
            if(sc_clientConnect.sc_tcpClient->read(sc_MQ.message.content,MQ_MESSAGE_LENGHT) != MQ_MESSAGE_LENGHT)
                break;
            if((sc_MQ.message.type = sc_MQ.mq->UplinkDataMatch(sc_MQ.message.content)) == LINK_MESSAGE_TYPE_USELESS){
                sc_MQ.isError = true;
                for(sc_MQ.lessData = 1; sc_MQ.lessData < MQ_MESSAGE_LENGHT - 2; sc_MQ.lessData ++){
                    if(sc_MQ.mq->UplinkDataMatch(sc_MQ.message.content+sc_MQ.lessData) != LINK_MESSAGE_TYPE_USELESS)
                        break;
                }
                continue;
            }


            sc_MQ.mq->Push(&(sc_MQ.message));
        }
    }
}

void SocketClient::DoSocketDisconnect(){
    QMutexLocker locker(&sc_clientConnect.mutex);
    if(!sc_clientConnect.sc_tcpClient)
        return;
    disconnect(sc_clientConnect.sc_tcpClient, SIGNAL(readyRead()),this,SLOT(DoSocketRead()));
    disconnect(sc_clientConnect.sc_tcpClient, SIGNAL(disconnected()),this,SLOT(DoSocketDisconnect()));
    sc_clientConnect.sc_tcpClient->disconnect();
    sc_clientConnect.sc_tcpClient->close();
    sc_clientConnect.sc_tcpClient = 0;
    sc_MQ.message.flag = -1;
    sc_MQ.message.content[0] = '\0';
    emit LoseConnect(sc_flag);
    sc_isHold = false;
}

bool SocketClient::Hold(QTcpSocket *tcpClient){
    QMutexLocker locker(&sc_clientConnect.mutex);
    if(sc_isHold)
        return false;
    sc_isHold = true;
    sc_clientConnect.sc_tcpClient = tcpClient;
    connect(sc_clientConnect.sc_tcpClient, SIGNAL(readyRead()),this,SLOT(DoSocketRead()));
    connect(sc_clientConnect.sc_tcpClient, SIGNAL(disconnected()),this,SLOT(DoSocketDisconnect()));
    sc_clientConnect.ip = sc_clientConnect.sc_tcpClient->peerAddress().toString();
    sc_clientConnect.ip = sc_clientConnect.ip.mid(sc_clientConnect.ip.lastIndexOf(":")+1);

    sc_MQ.message.flag = sc_flag;
    sc_MQ.message.content[0] = '\0';

    return true;
}

void SocketClient::SendData(QByteArray &content){
    QMutexLocker locker(&sc_clientConnect.mutex);
    if(!sc_clientConnect.sc_tcpClient)
        return;

    sc_clientConnect.sc_tcpClient->write(content);
}

void SocketClient::CheckAlive(){
    QMutexLocker locker(&sc_clientConnect.mutex);
    if(!sc_clientConnect.sc_tcpClient)
        return;
    if(!sc_clientConnect.sc_tcpClient->isValid() || !sc_clientConnect.sc_tcpClient->isWritable())
        emit sc_clientConnect.sc_tcpClient->disconnect();
}
