#ifndef DATARECEIVE_H
#define DATARECEIVE_H

#include <QWidget>
#include <QtMqtt/QMqttSubscription>
#include <QDateTime>
#include <QDebug>
#include <QByteArrayMatcher>
#include <QTextCodec>
#include <QThreadPool>
#include "filedeal.h"
#include "common.h"

namespace Ui {
class dataReceive;
}

class dataReceive : public QWidget
{
    Q_OBJECT

public:
    explicit dataReceive(QString path,QWidget *parent = nullptr);
  //  explicit dataReceive(QString path,QMqttSubscription *sub,QWidget *parent = nullptr);
    ~dataReceive();

  //
    QString filePath;
public slots:
    void newRcvSub(QMqttSubscription *sub);

private slots:
    void receiveMessage(const QMqttMessage &msg);
    void on_topicSet_clicked();
    void on_clearScreen_clicked();

signals:
    void setRcvSub(QString topic);
    void sglWrite();
private:
    QMqttSubscription *dataSub;

    Ui::dataReceive *ui;
    QThreadPool *threadPool;
    enum dataType rcvType;

};

class normalFileRcv : public QObject
{
        Q_OBJECT
public:
    explicit normalFileRcv(QObject *parent = nullptr);
    ~normalFileRcv();

    bool isSet(){return isTopicSet;}
    void init(QString path,QMqttSubscription *sub);
    void disable(){isTopicSet =  false;};
signals:
    void sglWrite(int,bool);
    void normalData(QByteArray);

private slots:
    void receiveFile(const QMqttMessage &msg);


private:
    QString filePath;
    QMqttSubscription *fileSub;
    QThreadPool *threadPool;
    enum dataType rcvType;
    bool isTopicSet;

};



#endif // DATARECEIVE_H
