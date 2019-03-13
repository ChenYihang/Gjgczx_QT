#include "interfaceconnect.h"
#include <SourceManager/sourcemanager.h>

InterfaceConnect::InterfaceConnect(SourceManager *sm){
    ic_SM = sm;
    connect(this,SIGNAL(SendCommand(int,QByteArray)),ic_SM,SLOT(ReceiveCommandFromInterface(int,QByteArray)));
}

InterfaceConnect::~InterfaceConnect(){

}

void InterfaceConnect::run(){
    while(1){
        scanf("%d%s",&ic_command,ic_content);
        emit SendCommand(ic_command,QByteArray(ic_content));
    }
}

void InterfaceConnect::WriteMessage(char *buff,int len){
    QFile fileout;
    fileout.open(stdout, QIODevice::WriteOnly);
    fileout.write(buff, len);
    fileout.write(",,,", 3);
    fileout.close();
   // qDebug()<<buff;
}
