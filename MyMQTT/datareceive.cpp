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

    threadPool = new QThreadPool();
    threadPool->setMaxThreadCount(10);
    ui->rcvTopic1->addItem("/CC3200@SHIP");
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

void dataReceive::receiveMessage(const QMqttMessage &msg){  //main deal function

    QByteArray realMsg = msg.payload();
    QString topic = msg.topic().name();
    QStringList tList = topic.split('/');
    if(tList.last()=="SENSORDATA"){
        qDebug()<<"SENSORDATA";
        rcvType = sensorData;
        if(realMsg.startsWith("$W")){   //处理传感器数据
            QByteArrayMatcher matcher("W$");
            int pos = matcher.indexIn(realMsg);
            if(pos != -1)
                 realMsg.remove(pos+2,800-pos-2);  //删除多余字节
        }

        ui->dataRcvShow->appendHtml(tr("<font color=\"#3c2bff\">%1  %2</font>").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss  ")).arg(msg.topic().name()+"\n"));
        if(ui->ckHexReceive->isChecked())
            ui->dataRcvShow->appendHtml(tr("<font color=\"#000000\">%1</font> ").arg(QString(multifunc::ByteArrayToHexStr(realMsg))));
        else
            ui->dataRcvShow->appendHtml(tr("<font color=\"#000000\">%1</font> ").arg(realMsg.data()));

        if(ui->dataRcvShow->blockCount()>100){
            ui->dataRcvShow->clear();
        }


    }
    else if(tList.last()=="COLLECTDATA"){
        qDebug()<<"COLLECTDATA";
        rcvType = collectData;
    }
    else if(tList.last()=="HISTORYDATA"){
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
void dataReceive::on_topicSet_clicked()
{
    QString topic;
    if(dataSub){
        dataSub->unsubscribe();
        //disconnect(dataSub,&QMqttSubscription::messageReceived, this, &dataReceive::receiveMessage);
    }
        topic = ui->rcvTopic1->currentText()+"/"+QString::number(ui->rcvNum->value())+
            ui->rcvTopic2->currentText()+"/#";
    //qDebug()<<topic;
    emit setRcvSub(topic);
   // dataSub->unsubscribe();

}

void dataReceive::newRcvSub(QMqttSubscription *sub){

    dataSub = sub;
    connect(dataSub, &QMqttSubscription::messageReceived, this, &dataReceive::receiveMessage);
   // qDebug()<<dataSub->topic();
    ui->dataRcvShow->appendHtml(tr("<font color=\"#3c2bff\">%1  </font><font color=\"#000000\">%2</font> ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss  ")).arg("Current Receive Topic: "+dataSub->topic().filter()));

    qDebug()<<"set";

}


normalFileRcv::normalFileRcv(QObject *parent) : QObject(parent)
{
    threadPool = new QThreadPool();
    threadPool->setMaxThreadCount(10);

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


}



void normalFileRcv::receiveFile(const QMqttMessage &msg){

    static bool flag = true;  // flase -- end; true -- start
    static int writeSize = 0;
   // qDebug()<<writeSize;
    QByteArray realMsg = msg.payload();
   qDebug()<<realMsg;

    QString topic = msg.topic().name();
    QStringList tList = topic.split('/');
     writeSize += (realMsg.size()-6);
     if(tList.last()=="UPFILE"){
         rcvType = armFileData;

     }else if(topic.contains("CMD")){
         emit normalData(realMsg);
         return;
     }

     QString realPath = filePath + topic;
     fileDeal *newFileTask = new fileDeal(realPath,realMsg,rcvType);
     newFileTask->setAutoDelete(true);
     threadPool->start(newFileTask);


     if(flag){
      emit sglWrite(writeSize,flag);
        flag = false;
     }

     if(realMsg.endsWith("%$#@")){
         writeSize -= (realMsg.size()-realMsg.indexOf("%$#@")-2);
         emit sglWrite(writeSize,flag);
         writeSize = 0;
         flag = true;


     }



}

void dataReceive::on_clearScreen_clicked()
{
   ui->dataRcvShow->clear();

}
