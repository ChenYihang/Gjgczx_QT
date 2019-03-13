#ifndef HTTPAPI_H
#define HTTPAPI_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

class httpAPI : public QObject
{
    Q_OBJECT
public:
    explicit httpAPI(QObject *parent = nullptr);
    ~httpAPI();

    QByteArray replyBytes;
    bool replyFlag;

signals:

public slots:
    void httpGET(QUrl url);

private slots:
    void replyFinished(QNetworkReply *reply);

private:
    bool httpRequestAbort; //请求停止的标志位
    QNetworkAccessManager *accessManager;
    QNetworkRequest request;
    QNetworkReply *reply;


};

#endif // HTTPAPI_H
