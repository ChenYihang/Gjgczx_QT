#include "datareceive.h"
#include "ui_datareceive.h"

dataReceive::dataReceive(QString path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dataReceive),filePath(path)
{
    ui->setupUi(this);
    dataSub = nullptr;
    //connect(dataSub, &QMqttSubscription::messageReceived, this, &dataReceive::receiveMessage);
   // this->setWindowTitle(dataSub->topic().filter());

    ///参数指令——打开信息窗口
    threadPool = new QThreadPool();//使用线程池
    threadPool->setMaxThreadCount(3);
    ui->rcvTopic1->addItem("/CC3200@SHIP");//初始化
    ui->rcvTopic1->addItem("/CC3200@PAD");
    ui->rcvTopic2->addItem("/SHIPDATA");
    ui->rcvTopic2->addItem("/PADDATA");

    //threadPool->setExpiryTimeout(-1);
}

dataReceive::~dataReceive()
{
    delete ui;
    delete  threadPool;
    if(dataSub)
       delete  dataSub;
}

///参数指令——打开信息窗口
void dataReceive::receiveMessage(const QMqttMessage &msg)
{
    QByteArray realMsg = msg.payload();
    QString topic = msg.topic().name();
    QStringList tList = topic.split('/');
    if(tList.last()=="SENSORDATA")
    {
        qDebug()<<"SENSORDATA";
        rcvType = sensorData;
        if(realMsg.startsWith("$W")) //处理传感器数据
        {
            QByteArrayMatcher matcher("W$");
            int pos = matcher.indexIn(realMsg);
            if(pos != -1)
                 realMsg.remove(pos+2,800-pos-2);  //删除多余字节
        }

        //示例：2019-06-24 17:08:46 Current Receive Topic: /CC3200@SHIP/1/SHIPDATA/#
        ui->dataRcvShow->appendHtml(tr("<font color=\"#3c2bff\">%1  %2</font>").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss  ")).arg(msg.topic().name()+"\n"));

        if(ui->ckHexReceive->isChecked())//若是十六进制选框已打勾
            ui->dataRcvShow->appendHtml(tr("<font color=\"#000000\">%1</font> ").arg(QString(multifunc::ByteArrayToHexStr(realMsg))));//类型转换
        else
            ui->dataRcvShow->appendHtml(tr("<font color=\"#000000\">%1</font> ").arg(realMsg.data()));

        if(ui->dataRcvShow->blockCount()>100)//若接收数据已超过100条
        {
            ui->dataRcvShow->clear();//自动清空
        }
    }
    else if(tList.last()=="COLLECTDATA")
    {
        qDebug()<<"COLLECTDATA";
        rcvType = collectData;
    }
    else if(tList.last()=="HISTORYDATA")
    {
        qDebug()<<"HISTORYDATA";
        rcvType = historyData;
    }
    //qDebug()<<tList;
    QString realPath = filePath + topic;
    fileDeal *newFileTask = new fileDeal(realPath,realMsg,rcvType);
    newFileTask->setAutoDelete(true);
    threadPool->start(newFileTask);
//   ui->dataRcvShow->appendPlainText(QLatin1String(realMsg));
}

//确认按键
void dataReceive::on_topicSet_clicked()
{
    QString topic;
    if(dataSub)//若已订阅
    {
        dataSub->unsubscribe();
        //disconnect(dataSub, &QMqttSubscription::messageReceived, this, &dataReceive::receiveMessage);
    }
    //指令主题组包
    topic = ui->rcvTopic1->currentText()+"/"+QString::number(ui->rcvNum->value())+
            ui->rcvTopic2->currentText()+"/#";
    //qDebug()<<topic;
    emit setRcvSub(topic);
   //dataSub->unsubscribe();
}

void dataReceive::newRcvSub(QMqttSubscription *sub)
{
    dataSub = sub;
    connect(dataSub, &QMqttSubscription::messageReceived, this, &dataReceive::receiveMessage);
   // qDebug()<<dataSub->topic();
    ui->dataRcvShow->appendHtml(tr("<font color=\"#3c2bff\">%1  </font><font color=\"#000000\">%2</font> ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss  ")).arg("Current Receive Topic: " + dataSub->topic().filter()));

    qDebug()<<"set";
}

///文件接收线程
///在一个应用程序中，需要多次使用线程，意味着需要多次创建并销毁线程
///而创建并销毁线程的过程会消耗内存，内存资源是及其宝贵的，所以，提出线程池的概念
normalFileRcv::normalFileRcv(QObject *parent) : QObject(parent)
{
    threadPool = new QThreadPool();
    threadPool->setMaxThreadCount(10);//设置最大线程数
}

normalFileRcv::~normalFileRcv(){
      delete  threadPool;
 }

void normalFileRcv::init(QString path, QMqttSubscription *sub)
{
    filePath=path;
    fileSub = sub;
    isTopicSet = true;
    connect(fileSub, &QMqttSubscription::messageReceived, this, &normalFileRcv::receiveFile);
    qDebug() << "接收";
}

void normalFileRcv::receiveFile(const QMqttMessage &msg)
{
    static bool flag = true;  // false -- end; true -- start
    static int writeSize = 0;
   // qDebug()<<writeSize;
    QByteArray realMsg = msg.payload();
   qDebug()<<realMsg;

    QString topic = msg.topic().name();
    //split将字符串拆分为SEP出现的任何子字符串，并返回这些字符串的列表
    //如果sep与字符串中的任何地方都不匹配，split（）将返回包含此字符串的单个元素列表
    QStringList tList = topic.split('/');
     writeSize += (realMsg.size()-6);
     if(tList.last()=="UPFILE")//pad到上位机
     {
         rcvType = armFileData;
     }
     else if(topic.contains("CMD"))
     {
         emit normalData(realMsg);
         return;
     }

     QString realPath = filePath + topic;
     fileDeal *newFileTask = new fileDeal(realPath, realMsg, rcvType);
     newFileTask->setAutoDelete(true);
     threadPool->start(newFileTask);

     if(flag)
     {
      emit sglWrite(writeSize,flag);
        flag = false;
     }

     if(realMsg.endsWith("%$#@"))
     {
         writeSize -= (realMsg.size()-realMsg.indexOf("%$#@")-2);
         writeSize += 2;
         emit sglWrite(writeSize,flag);
         writeSize = 0;
         flag = true;
     }
}

void dataReceive::on_clearScreen_clicked()
{
   ui->dataRcvShow->clear();
}
