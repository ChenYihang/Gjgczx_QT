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
    explicit normalFileRcv(QString path,QMqttSubscription *sub,QObject *parent = nullptr);
    ~normalFileRcv();
signals:
    void sglWrite(int,bool);

private slots:
    void receiveFile(const QMqttMessage &msg);


private:
    QString filePath;
    QMqttSubscription *fileSub;
    QThreadPool *threadPool;
    enum dataType rcvType;


};



#endif // DATARECEIVE_H
