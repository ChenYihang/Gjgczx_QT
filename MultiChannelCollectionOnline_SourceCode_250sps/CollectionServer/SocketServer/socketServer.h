#ifndef SocketServer_H
#define SocketServer_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <SourceManager/sourcemanager.h>

class SocketServer : public QObject
{
    Q_OBJECT
    protected:
        SocketServer(QObject *parent = nullptr);
    public:
        ~SocketServer();

    signals:
        void SubmitNewConnect(QTcpSocket *newConnect);
    public slots:
        void PendingNewConnect();
    public:
        static SocketServer *GetInstance();
        bool SSStart(int port);
        void SSStop();
    private:
        QTcpServer *ss_Server;
        static SocketServer *ss_SS;
        SourceManager *ss_SM;
};

#endif // SocketServer_H
