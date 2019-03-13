#include <SocketServer/socketserver.h>

SocketServer *SocketServer::ss_SS = 0;
SocketServer::SocketServer(QObject *parent) : QObject(parent){
    ss_Server = 0;
    ss_SM = SourceManager::GetInstance();
}
SocketServer::~SocketServer(){
    disconnect(this,SIGNAL(SubmitNewConnect(QTcpSocket *)),ss_SM,SLOT(AcceptNewConnect(QTcpSocket *)));
    disconnect(ss_Server,SIGNAL(newConnection()),this,SLOT(PendingNewConnect()));
    ss_Server->close();
    delete ss_Server;
}

SocketServer *SocketServer::GetInstance(){
    if(!ss_SS)
        ss_SS =  new SocketServer();
    return ss_SS;
}

void SocketServer::PendingNewConnect(){
    QTcpSocket *socketClient;
    while((socketClient = ss_Server->nextPendingConnection()))
        emit SubmitNewConnect(socketClient);
}

bool SocketServer::SSStart(int port){
    SSStop();
    ss_Server = new QTcpServer(this);
    ss_Server->setMaxPendingConnections(MAX_SLAVE_COMPUTER_COUNT*2);
    connect(ss_Server,SIGNAL(newConnection()),this,SLOT(PendingNewConnect()));
    connect(this,SIGNAL(SubmitNewConnect(QTcpSocket *)),ss_SM,SLOT(AcceptNewConnect(QTcpSocket *)));

    return ss_Server->listen(QHostAddress::Any,port);
}
void SocketServer::SSStop(){
    if(ss_Server){
        disconnect(this,SIGNAL(SubmitNewConnect(QTcpSocket *)),ss_SM,SLOT(AcceptNewConnect(QTcpSocket *)));
        disconnect(ss_Server,SIGNAL(newConnection()),this,SLOT(PendingNewConnect()));
        ss_Server->close();
        delete ss_Server;
        ss_Server = 0;
    }
}
