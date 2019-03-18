#ifndef CSOCKETCLIENT_H
#define CSOCKETCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QTimer>
#include "cdataprocess.h"
#include "timechartplot.h"
#include "updatecharts.h"

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


public slots:
    void socketRead();
    void socketDisconnect();

    void handDisconnect();
    void dealInitStart();



public:
//      inline int GetFlag(){return sc_flag;}

//        inline bool IsHold(){return sc_isHold;}
        inline QString GetIP(){return clientConnect.ip;}
        void sendData(QByteArray qbdata);
        bool hold(QTcpSocket *socketTcpClient);
        void clearConnect();
        updateCharts *udCharts;


private:
    //socket下位机标识
    int socketClientFlag;
    //是否已经被占有
    bool socketHoldFlag;
    //管理者句柄
    int audioFlag;
    int buffSize;
    //网络连接句柄
    struct{
        QMutex mutex;
        QTcpSocket *tcpClient;
        QString ip;
    } clientConnect;



     CdataProcess *dataProcessHandle;
     CsocketProcess *sp;






};

#endif // CSOCKETCLIENT_H
