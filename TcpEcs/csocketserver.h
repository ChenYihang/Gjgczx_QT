#ifndef CSOCKETSERVER_H
#define CSOCKETSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "csocketprocess.h"


class CsocketServer : public QObject
{
    Q_OBJECT
public:
    explicit CsocketServer(QObject *parent = nullptr);

public:
    ~CsocketServer();

signals:
    void SubmitNewConnect(QTcpSocket *newConnect);

public slots:
    void PendingNewConnect();

public:
    static CsocketServer *instance();
    bool serverStart(int iport);
    void serverStop();

private:
    QTcpServer *tcpServer;  //tcp对象
    static CsocketServer *serverHandle;  //控制句柄
    CsocketProcess *sktPro;

};

#endif // CSOCKETSERVER_H
