#include "infoupdate.h"

infoUpdate::infoUpdate(QObject *parent) : QThread(parent)
{

    requestInfo = new httpAPI(this);
    jsAna = new jsonSolve(this);
    connect(this,SIGNAL(requestSignal(QUrl)),requestInfo,SLOT(httpGET(QUrl)),Qt::QueuedConnection);

}

infoUpdate::~infoUpdate(){
   // delete nodeInfo;

    // 请求终止
    requestInterruption();
    quit();
    wait();
}



void infoUpdate::run(){



    // 是否请求终止
    while (!isInterruptionRequested())
    {
        switch(hflag){
        case 0:
            emit requestSignal(url1);
            sleep(2);
            emit infoToUi(jsAna->jsonAnalyse(requestInfo->replyBytes,basicInfo),basicInfo);//analyse json
            break;
        case 1:
            emit requestSignal(url2);
            sleep(2);

            emit infoToUi(jsAna->jsonAnalyse(requestInfo->replyBytes,monitorInfo),monitorInfo);  //analyse json
            break;
        case 2:
            emit requestSignal(url3);
            sleep(2);

            // usleep(500);
            emit infoToUi(jsAna->jsonAnalyse(requestInfo->replyBytes,clientList),clientList);  //analyse json
            break;
        case 3:
            emit requestSignal(url4);
            sleep(2);

           // usleep(500);
            emit infoToUi(jsAna->jsonAnalyse(requestInfo->replyBytes,sessionsList),sessionsList);  //analyse json
            break;
        case 4:
            emit requestSignal(url5);
            sleep(2);

           // usleep(500);
            emit infoToUi(jsAna->jsonAnalyse(requestInfo->replyBytes,subscriptionList),subscriptionList);  //analyse json
            break;
        default:
            break;


        }

    }
}


