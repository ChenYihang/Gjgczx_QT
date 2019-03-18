#include "csocketserver.h"
CsocketServer *CsocketServer::serverHandle =0;//控制句柄初始化

CsocketServer::CsocketServer(QObject *parent) : QObject(parent){

    tcpServer= 0;
    sktPro = CsocketProcess::instance();


}


CsocketServer::~CsocketServer(){

}

CsocketServer  *CsocketServer::instance(){
    if(!serverHandle)
        serverHandle = new CsocketServer();
    return serverHandle;

}

void CsocketServer::PendingNewConnect(){
    QTcpSocket *socketClient;
    while((socketClient = tcpServer->nextPendingConnection()))
        emit SubmitNewConnect(socketClient);
}

void CsocketServer::serverStop(){
    if(tcpServer){
        disconnect(this,SIGNAL(SubmitNewConnect(QTcpSocket *)),sktPro,SLOT(acceptNewConnect(QTcpSocket *)));
        disconnect(tcpServer,SIGNAL(newConnection()),this,SLOT(PendingNewConnect()));
        tcpServer->close();
        delete tcpServer;
        tcpServer = 0;
    }
}

//tcp开始监听
/*
The QTcpServer class provides a TCP-based server.
This class makes it possible to accept incoming TCP connections. You can specify the port or have QTcpServer pick one automatically.
You can listen on a specific address or on all the machine's addresses.
Call listen() to have the server listen for incoming connections. The newConnection() signal is then emitted each time a client connects to the server.
Call nextPendingConnection() to accept the pending connection as a connected QTcpSocket.
The function returns a pointer to a QTcpSocket in QAbstractSocket::ConnectedState that you can use for communicating with the client.

*/
bool CsocketServer::serverStart(int iport){

    serverStop();
    tcpServer = new QTcpServer(this);
    tcpServer->setMaxPendingConnections(2);
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(PendingNewConnect() ));
    connect(this,SIGNAL(SubmitNewConnect(QTcpSocket *)),sktPro,SLOT(acceptNewConnect(QTcpSocket *)));
    qDebug()<<"server started";
    return tcpServer->listen(QHostAddress::Any,iport);

}
