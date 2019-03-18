#ifndef CSOCKETPROCESS_H
#define CSOCKETPROCESS_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include "csocketclient.h"
#include "common.h"
class CsocketProcess : public QObject
{
    Q_OBJECT
public:
    explicit CsocketProcess(QObject *parent = nullptr);
    static CsocketProcess *instance();
    inline CsocketClient* getClientHandle(int clientNum){return proClientHandle[clientNum];}
    void clearAllConnect();

signals:
    void signalAddNewClient(QString qsip,int iflag);
    void signalDelClient(int iflag);
    void disPlay(QByteArray socketData,QString ip);
    void forward(QByteArray cmd,QString ip);
public slots:
    void acceptNewConnect(QTcpSocket* newConnect);
    void loseConnect(int iflag);
    void proClientDisconnect(int iflag);
    void slotDisPlay(QByteArray socketData, QString ip);
    void slotForward(QByteArray cmd,  QString ip);

private:
    static CsocketProcess *socketPro;
    CsocketClient  *(proClientHandle[MAX_CLIENT_COUNT]);  //10个下位机句柄



};

#endif // CSOCKETPROCESS_H
