#ifndef MYMQTT_H
#define MYMQTT_H

#include <QWidget>
#include <QModelIndex>
#include <QtMqtt/QMqttClient>
#include <QDateTime>
#include <QDebug>
#include <QThread>
#include <QWebEngineView>
#include "navlistview.h"
#include "infoupdate.h"
#include "subscription.h"
#include <QMessageBox>
#include <QListWidgetItem>
#include <QtWebChannel>
#include <QWebEnginePage>
#include <QSignalMapper>
#include <QMutexLocker>
#include <QDir>
#include "jsdocument.h"
#include "multifunc.h"
#include "datareceive.h"
namespace Ui {
class MyMQTT;

}

class sendFile;
class MyMQTT : public QWidget
{
    Q_OBJECT

public:
    explicit MyMQTT(QWidget *parent = nullptr);
    ~MyMQTT();
    friend class sendFile;
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
    void on_connectButton_clicked();

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

    void on_downDataFile_clicked();


    void on_fileTopicSet_clicked();

    void fileLogUpdate(unsigned int size, bool);



private:
    Ui::MyMQTT *ui;
    infoUpdate *serverInfo[5];
    jsDocument *mapJs;
    QWebChannel *webChnl;

    QList<subscription *> subList;

    dataReceive *dataRcv;
    normalFileRcv *fileRcv;


    int curSubItem;

    typedef struct {
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


    int speedflag;
    int speedtemp;
    int speedflag_lr;
    int speedtemp_lr;
    sendFile *sFile;
    QMqttClient *m_client;
    QFile *dataFile;
};

class sendFile : public QThread
{
    Q_OBJECT
public:
    explicit sendFile(QFile *tFile,QObject *parent = nullptr):fileHandle(tFile){}

signals:
    void sendFinish();
    void toPublish(const QByteArray &data);
public slots:
    void run();

    void stopSend(){
        QMutexLocker locker(&m_lock);
        runFlag = false;

    }
private:
    QFile *fileHandle;
    QMutex m_lock;
    bool runFlag;


};

#endif // MYMQTT_H
