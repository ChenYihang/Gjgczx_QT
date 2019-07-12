#include "infoupdate.h"

infoUpdate::infoUpdate(QObject *parent) : QThread(parent)
{
    requestInfo = new httpAPI(this);
    jsAna = new jsonSolve(this);
    connect(this,SIGNAL(requestSignal(QUrl)),requestInfo,SLOT(httpGET(QUrl)),Qt::QueuedConnection);
}

infoUpdate::~infoUpdate()
{
   // delete nodeInfo;

    // 请求终止
    requestInterruption();
    quit();
    wait();
}

//json数据解析进程
//所有URI返回application/json格式的的资源，每个请求都需要HTTP基本认证
//JSON(JavaScript Object Notation) 是一种轻量级的数据交换格式
void infoUpdate::run()
{
    // 请求是否终止
    while (!isInterruptionRequested())//若请求没有终止
    {
        switch(hflag)
        {
        case 0://一共有5个URL, 所以有5个
            emit requestSignal(url1);//发送请求
            sleep(2);//等待回应
            emit infoToUi(jsAna->jsonAnalyse(requestInfo->replyBytes, basicInfo), basicInfo);//分析json
            break;
        case 1:
            emit requestSignal(url2);
            sleep(2);
            emit infoToUi(jsAna->jsonAnalyse(requestInfo->replyBytes,monitorInfo),monitorInfo); //分析json
            break;
        case 2:
            emit requestSignal(url3);
            sleep(2);
            // usleep(500);
            emit infoToUi(jsAna->jsonAnalyse(requestInfo->replyBytes,clientList),clientList);  //分析json
            break;
        case 3:
            emit requestSignal(url4);
            sleep(2);
           // usleep(500);
            emit infoToUi(jsAna->jsonAnalyse(requestInfo->replyBytes,sessionsList),sessionsList); //分析json
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


