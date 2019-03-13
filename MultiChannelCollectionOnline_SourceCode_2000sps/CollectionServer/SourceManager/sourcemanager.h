#ifndef SOURCEMANAGER_H
#define SOURCEMANAGER_H

#include <QObject>
#include <DataProcessor/messagequeue.h>
#include <SourceManager/socketclient.h>
#include <SourceManager/filemaintainer.h>
#include <InterfaceConnect/interfaceconnect.h>
#include <QSemaphore>
#include <QSocketNotifier>

class SourceManager : public QObject
{
        Q_OBJECT
    public:
        ~SourceManager();
    protected:
        SourceManager(QObject *parent = nullptr);

    public slots:
        void AcceptNewConnect(QTcpSocket* newConnect);
        void LoseConnect(int flag);
        void ReceiveCommandFromInterface(int,QByteArray);

    public:
        static SourceManager *GetInstance();
        void InData(MSG_UNIT *msg);
        void SetDataPath(QString path);
        inline QString GetDataPath(){return sm_dataPath;}
        InterfaceConnect *sm_IC;
    private:
        QString MakeGuid();
    private:
        static SourceManager *sm_SM;
        MessageQueue *sm_MQ;
        SocketClient *(sm_clientHandle[MAX_SLAVE_COMPUTER_COUNT]);
        FileMaintainer *(sm_fileHandle[MAX_SLAVE_COMPUTER_COUNT]);

        int sm_curFlag;
        QString sm_dataPath;
};

#endif // SOURCEMANAGER_H
