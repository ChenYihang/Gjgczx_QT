#include "httpapi.h"

httpAPI::httpAPI(QObject *parent) : QObject(parent)
{
    accessManager = new  QNetworkAccessManager(this);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));//finish为manager自带的信号，replyFinished是自定义的
}
httpAPI::~httpAPI()
{

}
void httpAPI::httpGET(QUrl url)
{
    request.setUrl(url);

    //HTTP Basic认证
    QString concatenated = "f6da8cc48bca:Mjg4MDM2NzgxOTIxNjUwMTc3Mjg4MjU3Mjg3NDE3MzY0NDI";
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    request.setRawHeader("Authorization", headerData.toLocal8Bit());
    reply = accessManager->get(request);//通过发送数据，返回值保存在reply指针里.
}

void httpAPI::replyFinished(QNetworkReply *reply)
{
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(reply->error() == QNetworkReply::NoError)
    {
       replyBytes = reply->readAll();  //获取字节
       //qDebug() << "aaa";
    }
    else
    {
        //处理错误
         qDebug()<<"error";
    }
    reply->deleteLater();//要删除reply，但是不能在repyfinished里直接delete，要调用deletelater;
}


