#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <DataProcessor/messagequeue.h>
#include <QSemaphore>

class SourceManager;

class SocketClient:public QObject
{
    Q_OBJECT
    friend class SourceManager;
    protected:
        SocketClient(int flag,SourceManager *manager,QObject *parent = 0);
    public:
        ~SocketClient();
    signals:
        void LoseConnect(int flag);
    public slots:
        void DoSocketRead();
        void DoSocketDisconnect();
    public:
        inline int GetFlag(){return sc_flag;}
        inline QString GetIP(){return sc_clientConnect.ip;}
        inline bool IsHold(){return sc_isHold;}
        void SendData(QByteArray &content);
        bool Hold(QTcpSocket *sc_tcpClient);
        void CheckAlive();

    private:
        //标识，最大支持64路下位机
        int sc_flag;
        //是否已经被占有
        bool sc_isHold;
        //管理者句柄
        SourceManager *sc_SM;
        //网络连接句柄
        struct{
            QMutex mutex;
            QTcpSocket *sc_tcpClient;
            QString ip;
        } sc_clientConnect;
        //消息传输句柄
        struct{
            bool isError;
            int lessData;
            MessageQueue *mq;
            MSG_UNIT message;
        }sc_MQ;

        int temp;
};

#endif // SOCKETCLIENT_H
