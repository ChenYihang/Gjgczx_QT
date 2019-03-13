#include <QCoreApplication>
#include <SourceManager/sourcemanager.h>
#include <SocketServer/socketServer.h>
#include <DataProcessor/dataprocessor.h>
#include <../Common/common.h>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(argc < 3)
        return a.exec();

    SourceManager *sm = SourceManager::GetInstance();
    sm->SetDataPath(argv[2]);

    #if HIGH_PERFORMANCE
    const static int MCCO_D_PROC_COUNT = 50;
    #else
    const static int MCCO_D_PROC_COUNT = 20;
    #endif
    DataProcessor *(mocc_dataProcH[MCCO_D_PROC_COUNT]);
    for(int dpCursor = 0; dpCursor < MCCO_D_PROC_COUNT; dpCursor ++){
        mocc_dataProcH[dpCursor] = new DataProcessor();
        mocc_dataProcH[dpCursor]->start();
    }

    SocketServer *mcco_socketServerH = SocketServer::GetInstance();
    mcco_socketServerH->SSStart(QString(argv[1]).toInt());
//    mcco_socketServerH->SSStart(8888);
    return a.exec();
}
