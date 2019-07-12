#ifndef MYMQTT_H
#define MYMQTT_H

#include <QWidget>
#include <QModelIndex>
#include <QtMqtt/QMqttClient>
#include <QDateTime>
#include <QDebug>
#include <QThread>
#include <QWebEngineView>
#include "subscription.h"
#include <QMessageBox>
#include <QListWidgetItem>
#include <QtWebChannel>
#include <QWebEnginePage>
#include <QSignalMapper>
#include <QMutexLocker>
#include <QDir>
#include "datareceive.h"
#include "infoupdate.h"
#include "jsdocument.h"
#include "multifunc.h"
#include "navlistview.h"
#include <QCompleter>
#include <QVector>
#include <qiterator.h>

namespace Ui {
class MyMQTT;
}
class sendFile;
class topicHandler;
class MyMQTT : public QWidget
{
    Q_OBJECT

public:
    explicit MyMQTT(QWidget *parent = nullptr);
    ~MyMQTT();
    friend class sendFile;
    void dealClose();

private slots:
    void initForm();
    void initWebMap();
    void initCmd();
    void updateInfo(QStringList,int);
    void on_listView_pressed();

    void updateStateLog();

    void cmdDeal(const QString &arg);


    void brokerDisconnected();
    void pingResponseReceived();
    void on_connectButton_clicked();//连接MQTT服务器槽函数
    void on_pingButton_clicked();

    void on_subscription_clicked();
    void on_topicList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_unsubscription_clicked();
    void on_publish_clicked();
    void on_clearPub_clicked();

    void adNumChange(const QString &arg1);
    void adRateChange(const QString &arg1);
    void daRateChange(const QString &arg1);

    void on_verticalSlider_valueChanged(int value);
    void on_horizontalSlider_valueChanged(int value);
    void on_doubleSpinBox_valueChanged(double arg1);
    void on_doubleSpinBox_2_valueChanged(double arg1);
    void on_topicSet_clicked();

    void on_setPath_clicked();
    void on_openInfo_clicked();
    void on_downFile_clicked();


    void on_fileTopicSet_clicked();
    void fileLogUpdate(unsigned int size, bool);
    void on_sendFile_clicked();
    void on_selectDataFile_clicked();

    void on_fileTopicChoose_clicked();
    void on_pushButton_clicked();

    void on_sendFileCancel_clicked();

private:
    Ui::MyMQTT *ui;

    infoUpdate *serverInfo[5];
    jsDocument *mapJs;
    QWebChannel *webChnl;

    QList < subscription *> subList;
    dataReceive *dataRcv;

    int curSubItem;

    typedef struct
    {
        QString topic;
        QString message;
        quint8 qos;
        bool retain;
    }publish;

    QList<publish *> pubMsgList;
    QSignalMapper *cmdSignal;

    QString filePath;
    QString cmdTopic;
    QString fileTopic ;
    QStringList dataFileList;

    int speedflag;
    int speedtemp;
    int speedflag_lr;
    int speedtemp_lr;

    QMqttClient *m_client;//mqtt client指针
    QFile *dataFile;

    QVector<topicHandler *> topicVector;
};



class topicHandler:QObject{
  Q_OBJECT
public:
    topicHandler(QString);
    ~topicHandler();
public slots:
    sendFile *getSendHandler(){return sFile; }
    normalFileRcv *getRcvHandler(){return fileRcv; }
private:
    QString fileTopic;
    sendFile *sFile;//线程, 指针
    normalFileRcv *fileRcv;
};



class sendFile : public QThread
{
    Q_OBJECT
public:
    sendFile();
    ~sendFile();

signals:
    void sendFinish();
    void sendCancel();
    void startSend(QString fileName);//开始发送文件名
    void toPublish(const QByteArray &data);//发布数据

public slots:
    void run();
    void init(QStringList &list){fileList = list;}//引用
    void stopSend(){ runFlag = false; }
    void nextFile(){fileWait = false;}
    void setHandShake(){handshake = true;}
    bool show(){return handshake;}

private:
    QStringList fileList;
    QMutex m_lock;
    bool runFlag;//标志位
    QFile *fileDeal;
    bool fileWait;
    bool handshake;
};

#endif // MYMQTT_H
