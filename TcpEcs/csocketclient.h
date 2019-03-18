#ifndef CSOCKETCLIENT_H
#define CSOCKETCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include "cdataprocess.h"
#include "cfilehandle.h"
class CsocketProcess;
class CsocketClient : public QObject
{
    Q_OBJECT
    friend class CsocketProcess;
protected:
    CsocketClient(int iflag,CsocketProcess *socketPce,QObject *parent = 0);
public:
    ~CsocketClient();

signals:
    void loseConnect(int flag);
    void signalDisPlay(QByteArray socketData,QString ip);
    void signalForward(QByteArray cmd,QString ip);

public slots:
    void socketRead();
    void socketDisconnect();

    void handDisconnect();
    void dealInitStart();
    void fileInfoCommit(QByteArray fileInfo);



public:
//      inline int GetFlag(){return sc_flag;}
        inline QString GetIP(){return clientConnect.ip;}
//        inline bool IsHold(){return sc_isHold;}
       void initTrans(int type){fileHandle->setTrans(type);}
       void setFile(QString path){fileHandle->initFile(path);}
       void setPulseSize(unsigned int size){fileHandle->pulseInit(size);}
        void sendData(QByteArray qbdata);
        bool hold(QTcpSocket *socketTcpClient);
        void clearConnect();


private:
    //socket下位机标识
    int socketClientFlag;
    //是否已经被占有
    bool socketHoldFlag;
    //管理者句柄
    CsocketProcess *sp;

    int audioFlag;
    //网络连接句柄
    struct{
        QMutex mutex;
        QTcpSocket *tcpClient;
        QString ip;
    } clientConnect;

     CdataProcess *dataProcessHandle;
     CfileHandle  *fileHandle;
     int buffSize;


};

#endif // CSOCKETCLIENT_H
