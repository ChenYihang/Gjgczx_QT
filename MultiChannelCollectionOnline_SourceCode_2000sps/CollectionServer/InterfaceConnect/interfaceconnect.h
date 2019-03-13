#ifndef INTERFACECONNECT_H
#define INTERFACECONNECT_H

#include <QThread>
#include <QDebug>
#include <../Common/common.h>
class SourceManager;
class InterfaceConnect : public QThread
{
    Q_OBJECT
    signals:
        void SendCommand(int,QByteArray);
    public:
        InterfaceConnect(SourceManager *);
        ~InterfaceConnect();
    protected:
        void run();
    public:
        void WriteMessage(char *buff,int len);
    private:
        SourceManager *ic_SM;
        int ic_command;
        char ic_content[512];
};

#endif // INTERFACECONNECT_H
