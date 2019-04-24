#include "mymqtt.h"
#include "ui_mymqtt.h"
#include <QWebEngineView>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif


MyMQTT::MyMQTT(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyMQTT)
{
    ui->setupUi(this);
    this->initForm();
    this->initCmd();
   // this->initWebMap();
    //main ui set client connect
    m_client = new QMqttClient(this);
    for(int i = 0;i < 5;i++){
     serverInfo[i] = new infoUpdate(this);
     serverInfo[i]->hflag = i;
     connect(serverInfo[i],&infoUpdate::infoToUi,this,&MyMQTT::updateInfo);

    }
    for(int i=0;i<10;i++){
        fileRcv[i]= new normalFileRcv();
        sFile[i] = new sendFile();
    }

    dataRcv = nullptr;
    curSubItem = -1;
    connect(m_client, &QMqttClient::stateChanged, this, &MyMQTT::updateStateLog);
    connect(m_client, &QMqttClient::disconnected, this, &MyMQTT::brokerDisconnected);


    connect(ui->showMsg,&QPushButton::clicked,this,[this](){if(curSubItem >=0)subList.at(curSubItem)->show();});
    connect(ui->topicList,&QListWidget::doubleClicked,this,[this](){subList.at(curSubItem)->show();});

}

MyMQTT::~MyMQTT()
{
    delete ui;
    for(int i =0;i<subList.count();i++)
        delete subList.at(i);
    delete m_client;
    for(int i =0;i<5;i++)
     delete serverInfo[i];
    if(dataRcv){
        delete  dataRcv;
    }
    for(int i=0;i<10;i++){
        delete fileRcv[i];
        delete sFile[i];
    }

}

void MyMQTT::initForm(){


    ui->listView->setIcoColorBg(false);
    ui->listView->setColorLine(QColor(254, 255, 255));
    ui->listView->setColorBg(QColor(72, 82, 127), QColor(100, 114, 176), QColor(218, 223, 237, 150));
    ui->listView->setColorText(QColor(254, 255, 255), QColor(252, 252, 252), QColor(0, 0, 0));
    //设置数据方式
    QStringList listItem;

    listItem.append("MQTT||0|");
    listItem.append("设置连接|MQTT||");
    listItem.append("服务器信息||0|");
    listItem.append("节点信息|服务器信息||");
    listItem.append("连接列表|服务器信息||");
    listItem.append("会话列表|服务器信息||");
    listItem.append("订阅列表|服务器信息||");

    listItem.append("订阅/发布||0|");
    listItem.append("订阅主题|订阅/发布||");
    listItem.append("发布信息|订阅/发布||");
    listItem.append("参数指令|订阅/发布||");

    listItem.append("文件传输||0|");
    listItem.append("ARM_SD|文件传输||");

    listItem.append("传感信息||0|");

    listItem.append("位置信息|传感信息||");
    listItem.append("帮助信息||1|");



   ui->listView->setIcoStyle(NavListView::IcoStyle_Triangle);

   ui->listView->setData(listItem);


  ui->portEdit->setText("1883");
  ui->hostEdit->setText("139.199.208.33");
  ui->userEdit->setText("cehang");
  ui->pwEdit->setText("cehang001");
  ui->clientIDEdit->setText("MyMQTT1");
  ui->HearBeatEdit->setText("60");
  ui->stackedWidget->setCurrentIndex(0);


  ui->topicEdit->setText("/CC3200@SHIP/1/SHIPDATA/SENSORDATA");
  ui->pubMsgEdit->setText("Hello World!");
  ui->pubTopicEdit->setText("/CC3200@SHIP/1/SHIPDATA/SENSORDATA");
  //initalize clientlist tablewidget
  QStringList clientheader;
  clientheader<<"客户端ID"<<"用户名"<<"IP地址"<<"端口"<<"清除会话"<<"协议版本"<<"心跳(秒)"<<"连接时间";
  ui->clientList->setColumnCount(8);
  ui->clientList->setHorizontalHeaderLabels(clientheader);
  ui->clientList->horizontalHeader()->setStretchLastSection(true);

  //initalize sessions tablewidget
  QStringList sessionsheader;
  sessionsheader<<"客户端ID"<<"清除会话"<<"订阅数"<<"最大拥塞"<<"当前拥塞"<<"当前缓存信息"<<"丢弃消息"<<"等待释放";
  sessionsheader<<"投递消息"<<"入队消息"<<"创建时间";
  ui->sessionList->setColumnCount(11);
  ui->sessionList->setHorizontalHeaderLabels(sessionsheader);
  ui->sessionList->horizontalHeader()->setStretchLastSection(true);
  //initalize subscription tablewidget
  QStringList subscriptheader;
  subscriptheader<<"客户端ID"<<"主题"<<"安全质量";
  ui->subscriptionList->setColumnCount(3);
  ui->subscriptionList->setHorizontalHeaderLabels(subscriptheader);
  ui->subscriptionList->horizontalHeader()->setStretchLastSection(true);


   //initalize the filepath
  filePath = QDir::currentPath();
  ui->currentPath->setText(filePath);
  ui->currentPath->setReadOnly(true);

  ui->openInfo->setEnabled(false);
  ui->topicSet->setEnabled(false);

  ui->fileTopicNum->setRange(0,9);


}

void MyMQTT::initWebMap(){

   mapJs = new jsDocument(this);
   webChnl = new QWebChannel(this);
   webChnl->registerObject(QStringLiteral("content"),mapJs);
   ui->mapView->page()->setWebChannel(webChnl);
   ui->mapView->setUrl(QUrl("qrc:/test.html"));
   connect(mapJs, &jsDocument::recvdMsg, this,[this](const QString& msg) {
   });

}


void MyMQTT::initCmd(){

    ui->cmdTopic1->addItem("/PCMQTT");
    ui->cmdTopic2->addItem("/CC3200@SHIP");
    ui->cmdTopic3->addItem("/CMD");

    ui->fileTopic1->addItem("/PCMQTT");
    ui->fileTopic2->addItem("/CC3200@PAD");
    ui->fileTopic3->addItem("/DLFILE");



    ui->adNum->addItem(tr(" "));
    ui->adNum->addItem(tr("1K"));
    ui->adNum->addItem(tr("10K"));
    ui->adNum->addItem(tr("100K"));

    ui->adRate->addItem(tr(" "));
    ui->adRate->addItem(tr("2.1MSPS"));
    ui->adRate->addItem(tr("1.4MSPS"));
    ui->adRate->addItem(tr("0.7MSPS"));
    ui->adRate->addItem(tr("0.35MSPS"));

    ui->daRate->addItem(tr(" "));
    ui->daRate->addItem(tr("2.00MSPS"));
    ui->daRate->addItem(tr("1.33MSPS"));
    ui->daRate->addItem(tr("0.68MSPS"));
    ui->daRate->addItem(tr("0.33MSPS"));

    ui->folders->setPlaceholderText("num1");
    ui->files->setPlaceholderText("num2");

    //comboBox信号
    connect(ui->adNum,&QComboBox::currentTextChanged,this,&MyMQTT::adNumChange);
    connect(ui->adRate,&QComboBox::currentTextChanged,this,&MyMQTT::adRateChange);
    connect(ui->daRate,&QComboBox::currentTextChanged,this,&MyMQTT::daRateChange);

    //按键信号
    cmdSignal = new QSignalMapper(this);   //信号映射器
     //设置signalmapper的转发规则
    connect(ui->sigle, SIGNAL(clicked()), cmdSignal, SLOT(map()));
    cmdSignal->setMapping(ui->sigle, "1");
    connect(ui->upSensor, SIGNAL(clicked()), cmdSignal, SLOT(map()));
    cmdSignal->setMapping(ui->upSensor, "2");
    connect(ui->upHistory, SIGNAL(clicked()), cmdSignal, SLOT(map()));
    cmdSignal->setMapping(ui->upHistory, "3");
    connect(ui->upRealTime, SIGNAL(clicked()), cmdSignal, SLOT(map()));
    cmdSignal->setMapping(ui->upRealTime, "4");
    connect(ui->upCollect, SIGNAL(clicked()), cmdSignal, SLOT(map()));
    cmdSignal->setMapping(ui->upCollect, "5");
    connect(ui->Stop, SIGNAL(clicked()), cmdSignal, SLOT(map()));
    cmdSignal->setMapping(ui->Stop, "6");
    connect(ui->reset, SIGNAL(clicked()), cmdSignal, SLOT(map()));
    cmdSignal->setMapping(ui->reset, "7");


    //将转发的信号连接到最终的槽函数
    connect(cmdSignal, SIGNAL(mapped(const QString &)), this, SLOT(cmdDeal(const QString &)));





}
void MyMQTT::on_listView_pressed()
{

    QModelIndex index = ui->listView->currentIndex();
    QString text = index.data().toString();
    ui->logShow->setHidden(false);
    if (text == "设置连接"|| text =="MQTT") {
        ui->stackedWidget->setCurrentIndex(0);
    }
    else if (text == "节点信息"||text =="服务器信息") {
        ui->stackedWidget->setCurrentIndex(1);

    }
    else if(text =="连接列表"){
        ui->stackedWidget->setCurrentIndex(2);
    }
    else if(text =="会话列表"){
        ui->stackedWidget->setCurrentIndex(3);
    }
    else if(text =="订阅列表"){
        ui->stackedWidget->setCurrentIndex(4);
    }
     else if(text =="参数指令"){
            ui->stackedWidget->setCurrentIndex(5);
      }
    else if(text == "ARM_SD"){

        ui->stackedWidget->setCurrentIndex(6);

    }
    else if(text == "订阅主题"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    else if(text == "发布信息"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    else if(text == "位置信息"){

        ui->stackedWidget->setCurrentIndex(9);
        ui->logShow->setHidden(true);
       // mapJs->sendMsg(ui->mapView->page(),"hello world!");

    }


}

void MyMQTT::updateStateLog(){
    const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
                    + QLatin1String(": State Change")
                    + QString::number(m_client->state());
    ui->logShow->appendPlainText(content);
    if(m_client->state()==2){  //thread starts when connected
       for(int i=0;i<5;i++){
        if (serverInfo[i]->isRunning())
        {
            serverInfo[i]->wait();  //wait for thread ending
        }
      serverInfo[i]->start();
       }
      //clientList widget need
      ui->clientList->horizontalHeader()->setStretchLastSection(false);
      for(int i = 0; i<8; i++){
        ui->clientList->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
      }
      //
      //sessionsList widget need
      ui->sessionList->horizontalHeader()->setStretchLastSection(false);
      for(int i = 0; i<11; i++){
        ui->sessionList->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
      }
      //
      //subscriptList widget need
     // ui->subscriptionList->horizontalHeader()->setStretchLastSection(false);
      for(int i = 0; i<3; i++){
        ui->subscriptionList->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
      }
      //

      ui->topicSet->setEnabled(true);


     // qDebug()<<"thread started!";
    }
    else if(m_client->state()==0) { //thread stops when disconnect
        for(int i=0;i<5;i++){
        serverInfo[i]->requestInterruption();
        serverInfo[i]->quit();
        }
        qDebug()<<"ok";

       // clientList widget need
       while(ui->clientList->rowCount()>0){
        ui->clientList->removeRow(0);
       }
        ui->clientList->horizontalHeader()->setStretchLastSection(true);
        //

        //sessions widget need
       while(ui->sessionList->rowCount()>0){
        ui->sessionList->removeRow(0);
       }
        ui->sessionList->horizontalHeader()->setStretchLastSection(true);
        //
        //subscriptList widget need
       while(ui->subscriptionList->rowCount()>0){
        ui->subscriptionList->removeRow(0);
       }
        ui->subscriptionList->horizontalHeader()->setStretchLastSection(true);
        //


        ui->topicSet->setEnabled(false);


    }
}

void MyMQTT::brokerDisconnected(){
    qDebug()<<"disconnet";
     ui->connectButton->setText(tr("Connect"));
     ui->topicList->clear();
     ui->pubList->clear();
     for(int i =0;i<subList.count();i++)
         delete subList.at(i);
     subList.clear();
     if(dataRcv){

         dataRcv->close();
         dataRcv = nullptr;
         //qDebug()<<dataRcv;

     }



}

void MyMQTT::on_connectButton_clicked()
{
    m_client->setHostname(ui->hostEdit->text());
    m_client->setPort((ui->portEdit->text().toInt()));
    m_client->setUsername(ui->userEdit->text());
    m_client->setPassword(ui->pwEdit->text());
    m_client->setClientId(ui->clientIDEdit->text());
    m_client->setKeepAlive(ui->HearBeatEdit->text().toInt());
    if (m_client->state() == QMqttClient::Disconnected) {
        ui->hostEdit->setEnabled(false);
        ui->portEdit->setEnabled(false);
        ui->userEdit->setEnabled(false);
        ui->pwEdit->setEnabled(false);
        ui->connectButton->setText(tr("Disconnect"));
        m_client->connectToHost();
    } else {
        ui->hostEdit->setEnabled(true);
        ui->portEdit->setEnabled(true);
        ui->userEdit->setEnabled(true);
        ui->pwEdit->setEnabled(true);
        ui->connectButton->setText(tr("Connect"));
        m_client->disconnectFromHost();

    }
}

void MyMQTT::pingResponseReceived(){
    const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
                + QLatin1String(" PingResponse");
    ui->logShow->appendPlainText(content);
    disconnect(m_client, &QMqttClient::pingResponseReceived,this,&MyMQTT::pingResponseReceived);  //避免心跳包频繁显示

}

void MyMQTT::on_pingButton_clicked()
{
    connect(m_client, &QMqttClient::pingResponseReceived,this,&MyMQTT::pingResponseReceived);

    m_client->requestPing();
}

void MyMQTT::updateInfo(QStringList info,int type){

 //   qDebug()<<type;
    switch(type){
    case 0:{
        ui->version->setText(info.at(0));
        ui->brokenName->setText(info.at(1));
        ui->uptime->setText(info.at(2));
        ui->uptime->setCursorPosition(0);
        ui->datetime->setText(info.at(3));

        break;
    }
    case 1:{
        ui->nodename->setText(info.at(0));
        ui->memory->setText(info.at(2)+'/'+info.at(1));
        ui->eprocess->setText(info.at(4)+'/'+info.at(3));
        ui->maxfds->setText(info.at(5));
        ui->systatus->setText(info.at(6));
        ui->otpversion->setText(info.at(7));
        break;
    }
    case 2:{
        int rcout = ui->clientList->rowCount();
        while(rcout < info.size()/8){         // insert ROW when add new client // need testcase
            ui->clientList->insertRow(rcout++);
        }
        for(int i = 0; i<info.size(); i=i+8){
            for(int j =0 ;j< 8;j++){
                QTableWidgetItem *tableItem =new QTableWidgetItem();
                tableItem->setText(info.at(i+j));
                ui->clientList->setItem(i/8,j,tableItem);
            }
                        // ui->clientList->item(i,0)->setText(info.at(i));
        }
        if(info.size()/8 < rcout){  // clients disconnect

            for(int k = 0; k < rcout; k++){
                QTableWidgetItem *deleteItem =new QTableWidgetItem();
                deleteItem = ui->clientList->item(k,0);
                //qDebug()<<deleteItem->text();
                if(!info.contains(deleteItem->text())){
                    ui->clientList->removeRow(k);
                    break;
                }
            }
        }

        break;
    }
    case 3:{
        int rcout = ui->sessionList->rowCount();
        while(rcout < info.size()/11){         // insert ROW when add new client // need testcase
            ui->sessionList->insertRow(rcout++);
        }
        for(int i = 0; i<info.size(); i=i+11){
            for(int j =0 ;j< 11;j++){
                QTableWidgetItem *tableItem =new QTableWidgetItem();
                tableItem->setText(info.at(i+j));
                ui->sessionList->setItem(i/11,j,tableItem);
            }
        }
        if(info.size()/11 < rcout){  // clients disconnect

            for(int k = 0; k < rcout; k++){
                QTableWidgetItem *deleteItem =new QTableWidgetItem();
                deleteItem = ui->sessionList->item(k,0);
                //qDebug()<<deleteItem->text();
                if(!info.contains(deleteItem->text())){
                    ui->sessionList->removeRow(k);
                    break;
                }
            }
        }

        break;
    }
    case 4:{
        int rcout = ui->subscriptionList->rowCount();
        while(rcout < info.size()/3){         // insert ROW when add new client // need testcase
            ui->subscriptionList->insertRow(rcout++);
        }
        for(int i = 0; i<info.size(); i=i+3){
            for(int j =0 ;j< 3;j++){
                QTableWidgetItem *tableItem =new QTableWidgetItem();
                tableItem->setText(info.at(i+j));
                ui->subscriptionList->setItem(i/3,j,tableItem);
            }
        }
        if(info.size()/3 < rcout){  // clients disconnect

            for(int k = 0; k < rcout; k++){
                QTableWidgetItem *deleteItem =new QTableWidgetItem();
                deleteItem = ui->subscriptionList->item(k,0);
                //qDebug()<<deleteItem->text();
                if(!info.contains(deleteItem->text())){
                    ui->subscriptionList->removeRow(k);
                    break;
                }
            }
        }

        break;
    }
    default:
        break;
    }

}

void MyMQTT::on_subscription_clicked()
{
    qDebug()<<subList.size();
   QString topic = ui->topicEdit->text();
   quint8 qos = ui->qosEdit->value();
   for(int i=0;i<ui->topicList->count();i++)
   {
       QListWidgetItem *item_sel = ui->topicList->item(i);
       if(item_sel->text()== topic )
       {
           QFont font = item_sel->font();

          if(font.strikeOut()){
              font.setStrikeOut(false);
               item_sel->setFont(font);
               auto newSub = m_client->subscribe(topic,qos);
               auto m_sub = new subscription(newSub);

               subList.replace(i,m_sub);
               qDebug()<<subList.size();
               connect(m_sub,&subscription::rcvToMainUpdate,this,[this](){ui->rcvNumShow->setText(tr("%1").arg(subList.at(curSubItem)->rcvCount));});
               connect(m_sub,&subscription::staToMainUpdate,this,[this](){ui->subStaShow->setText(tr("%1").arg(subList.at(curSubItem)->subState));});

                ui->topicList->setCurrentItem(nullptr);
                ui->topicList->setCurrentItem(item_sel);   //refresh to update

               return;

          }
          else{
                  QMessageBox::critical(this,tr("Error"),tr("主题：%1已经订阅").arg(topic));
                  return;
          }

       }
   }
    auto newSub = m_client->subscribe(topic,qos);
    if(!newSub){
        qDebug()<<"error";
        return;
    }
   auto m_sub = new subscription(newSub);
   subList<<m_sub;
   connect(m_sub,&subscription::rcvToMainUpdate,this,[this](){ui->rcvNumShow->setText(tr("%1").arg(subList.at(curSubItem)->rcvCount));});
   connect(m_sub,&subscription::staToMainUpdate,this,[this](){ui->subStaShow->setText(tr("%1").arg(subList.at(curSubItem)->subState));});

   QListWidgetItem *item = new QListWidgetItem;
   item->setText(topic);
   ui->topicList->addItem(item);
   ui->topicList->setCurrentItem(item);

   // qDebug()<<newSub->topic().filter();
}



void MyMQTT::on_topicList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{

    curSubItem =  ui->topicList->row(current);

    if(curSubItem <0)  //when empty
        return;

    int rcvNum = subList.at(curSubItem)->rcvCount;
    ui->rcvNumShow->setText(tr("%1").arg(rcvNum));

    ui->topicShow->setText(subList.at(curSubItem)->subTopic);
    ui->qosShow->setText(tr("%1").arg(subList.at(curSubItem)->subqos));
    ui->subStaShow->setText(tr("%1").arg(subList.at(curSubItem)->subState));
}


void MyMQTT::on_unsubscription_clicked()
{
    subList.at(curSubItem)->m_sub->unsubscribe();
     QListWidgetItem *item = new QListWidgetItem;
     item = ui->topicList->currentItem();
      QFont font;
      font.setStrikeOut(true);
      item->setFont(font);



}

void MyMQTT::on_publish_clicked()
{
    if (m_client->publish(ui->pubTopicEdit->text(),
                          ui->pubMsgEdit->text().toUtf8(),
                          ui->pubQosEdit->text().toUInt(),
                          ui->retainCheck->isChecked()) == -1){
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));

    return;
    }
    auto newMsg = new publish;
    newMsg->topic=ui->pubTopicEdit->text();
    newMsg->message = ui->pubMsgEdit->text();
    newMsg->qos=ui->pubQosEdit->text().toUInt();
    newMsg->retain =ui->retainCheck->isChecked();

    pubMsgList<<newMsg;

    QListWidgetItem *item = new QListWidgetItem;
    item->setText(newMsg->message);
    item->setToolTip(tr("topic:%1,qos:%2,retain:%3").arg(newMsg->topic).arg(newMsg->qos).arg(newMsg->retain));
    item->setWhatsThis("hello");
    item->setStatusTip("hello");

    ui->pubList->addItem(item);
    ui->pubList->setCurrentItem(item);
}

void MyMQTT::on_clearPub_clicked()
{
    ui->pubList->clear();
    pubMsgList.clear();
}




void MyMQTT::on_topicSet_clicked()
{
    cmdTopic = ui->cmdTopic1->currentText()+ui->cmdTopic2->currentText()+"/"
                    +QString::number(ui->cmdNum->value())+ui->cmdTopic3->currentText();
    const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")
                    + QLatin1String("Current CMDTopic: ")+cmdTopic;



    dataRcv = new dataReceive(filePath);
    connect(dataRcv,&dataReceive::setRcvSub,this,[this](QString topic){ QMqttSubscription *dataSub = m_client->subscribe(topic,0);dataRcv->newRcvSub(dataSub);}); //设置新的接收主题
    qDebug()<<"creat rcvhandle";
    ui->openInfo->setEnabled(true);

   ui->logShow->appendPlainText(content);


}


void MyMQTT::adNumChange(const QString &arg1){

    int result = 0;
    if(arg1 == "1K"){
       result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("AC 08 AC"),1,false);

    }else if(arg1 =="10K"){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("AC 10 AC"),1,false);

    }
    else if(arg1 =="100K"){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("AC 20 AC"),1,false);
    }

    if(result == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));


}
void MyMQTT::adRateChange(const QString &arg1){
    int result = 0;

    if(arg1 == "2.1MSPS"){
       result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("AD 00 AD"),1,false);

    }else if(arg1 =="1.4MSPS"){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("AD 02 AD"),1,false);

    }
    else if(arg1 =="0.7MSPS"){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("AD 04 AD"),1,false);
    }
    else if(arg1 =="0.35MSPS"){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("AD 08 AD"),1,false);
    }

    if(result == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));


}
void MyMQTT::daRateChange(const QString &arg1){
    int result = 0;

    if(arg1 == "2.00MSPS"){
       result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DA 00 DA"),1,false);

    }else if(arg1 =="1.33MSPS"){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DA 02 DA"),1,false);

    }
    else if(arg1 =="0.68MSPS"){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DA 04 DA"),1,false);
    }
    else if(arg1 =="0.33MSPS"){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DA 08 DA"),1,false);
    }

    if(result == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
}



void MyMQTT::cmdDeal(const QString &arg){

    int num = arg.toInt(nullptr,10);
    int result = 0;
    qDebug()<<num;

    switch (num) {
    case 1: //单帧采集
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("AA 00 AA"),1,false);
        break;
    case 2:  //上传传感器数据
        result= m_client->publish(cmdTopic+"/SENSORCMD", multifunc::HexStrToByteArray("D1 00 D1"),1,false);
        break;
    case 3:  //上传历史数据
        {
            QString folder =ui->folders->text();
            QString file =ui->files->text();
            int temp1 = multifunc::StrDecimalToDecimal(folder);
            int temp2 = multifunc::StrDecimalToDecimal(file);
            QString upload="D3";
            QString t1 = multifunc::IntDecToStrHex(temp1);
            QString t2 = multifunc::IntDecToStrHex(temp2);
           if(temp1<16&&temp1>0)
            upload+=" 0";
           else
             upload+=" ";
            upload+=t1;
            if(temp2<16&&temp2>0)
            upload+=" D1 D3 0";
            else
            upload+=" D1 D3 ";

            upload+=t2;
            upload+=" D2";

        result= m_client->publish(cmdTopic+"/HISTORYCMD", multifunc::HexStrToByteArray(upload),1,false);
        break;
        }
    case 4: {//实时上传
        if(ui->upRealTime->text()=="实时上传"){
            ui->upRealTime->setText("停止上传");
            QString upload4 = "D4 00 D4";
            result= m_client->publish(cmdTopic+"/SENSORCMD", multifunc::HexStrToByteArray(upload4),1,false);

        }
        else{
              ui->upRealTime->setText("实时上传");
              QString upload4 = "D4 FF D4";
              result= m_client->publish(cmdTopic+"/SENSORCMD", multifunc::HexStrToByteArray(upload4),1,false);

        }
        break;
    }
    case 5:  //上传采集数据
        result= m_client->publish(cmdTopic+"/COLLECTCMD", multifunc::HexStrToByteArray("AA 00 AA D2 00 D2"),1,false);
        break;
    case 6:  //电机stop
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4353FD00FD"),1,false);
        break;
    case 7://复位
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("660066"),0,false);
        break;
    default:
        break;

    }

    if(result == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));

}

void MyMQTT::on_verticalSlider_valueChanged(int value)
{   int result =0;

    if(value>=1&&value <=25)
        speedflag = 1;
    else if(value >=26&& value<=50)
        speedflag = 2;
    else if(value >=51 && value <=75)
        speedflag =3;
    else if(value >=76 && value <=100)
        speedflag =4;
    else if(value >=101 && value <=125)
        speedflag =5;
    else if(value >=126 && value <=150)
        speedflag =6;
    else if(value >=151 && value <=175)
        speedflag =7;
    else if(value >=176 && value <=200)
        speedflag =8;
    else if(value >=-25&& value<=-1)
        speedflag = -1;
    else if(value >=-50&& value<=-26)
        speedflag = -2;
    else if(value >=-75 && value <=-51)
        speedflag =-3;
    else if(value >=-100 && value <=-76)
        speedflag =-4;
    else if(value >=-125 && value <=-101)
        speedflag =-5;
    else if(value >=-150&& value<=-126)
        speedflag = -6;
    else if(value >=-175&& value<=-151)
        speedflag = -7;
    else if(value >=-200 && value <=-176)
        speedflag =-8;
    else
        speedflag = 0;

    ui->label_vspeed->setText(tr("%1\%").arg(speedflag*12.5));
    if(speedflag == 1 && speedtemp!=1){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4601FD00FD"),1,false);
        speedtemp = 1;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 2 && speedtemp !=2){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4602FD00FD"),1,false);
        speedtemp = 2;
        qDebug()<<speedtemp;

    }
    else if(speedflag == 3 && speedtemp !=3){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4603FD00FD"),1,false);
        speedtemp = 3;
        qDebug()<<speedtemp;

    }
    else if(speedflag == 4 && speedtemp !=4){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4604FD00FD"),1,false);
        speedtemp = 4;
        qDebug()<<speedtemp;

    }
    else if(speedflag == 5 && speedtemp !=5){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4605FD00FD"),1,false);
        speedtemp =5;
        qDebug()<<speedtemp;

    }
    else if(speedflag == 6 && speedtemp !=6){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4606FD00FD"),1,false);
        speedtemp = 6;
        qDebug()<<speedtemp;

    }
    else if(speedflag == 7 && speedtemp !=7){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4607FD00FD"),1,false);
        speedtemp = 7;
        qDebug()<<speedtemp;

    }
    else if(speedflag == 8 && speedtemp !=8){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4608FD00FD"),1,false);
        speedtemp = 8;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -1 && speedtemp != -1){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4201FD00FD"),1,false);

        speedtemp = -1;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -2 && speedtemp !=-2){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4202FD00FD"),1,false);
        speedtemp = -2;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -3 && speedtemp !=-3){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4203FD00FD"),1,false);
        speedtemp = -3;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -4 && speedtemp != -4){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4204FD00FD"),1,false);
        speedtemp = -4;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -5 && speedtemp != -5){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4205FD00FD"),1,false);
        speedtemp = -5;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -6 && speedtemp !=-6){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4206FD00FD"),1,false);
        speedtemp = -6;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -7 && speedtemp !=-7){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4207FD00FD"),1,false);
        speedtemp = -7;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -8 && speedtemp != -8){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4208FD00FD"),1,false);
        speedtemp = -8;
        qDebug()<<speedtemp;

    }

    if(result == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));


}


void MyMQTT::on_horizontalSlider_valueChanged(int value)
{
    int result = 0;
    if(value>=1&&value <=25)
        speedflag_lr = 1;
    else if(value >=26&& value<=50)
        speedflag_lr = 2;
    else if(value >=51 && value <=75)
        speedflag_lr =3;
    else if(value >=76 && value <=100)
        speedflag_lr =4;
    else if(value >=101 && value <=125)
        speedflag_lr =5;
    else if(value >=126 && value <=150)
        speedflag_lr =6;
    else if(value >=151 && value <=175)
        speedflag_lr =7;
    else if(value >=176 && value <=200)
        speedflag_lr =8;
    else if(value >=-25&& value<=-1)
        speedflag_lr = -1;
    else if(value >=-50&& value<=-26)
        speedflag_lr = -2;
    else if(value >=-75 && value <=-51)
        speedflag_lr =-3;
    else if(value >=-100 && value <=-76)
        speedflag_lr =-4;
    else if(value >=-125 && value <=-101)
        speedflag_lr =-5;
    else if(value >=-150&& value<=-126)
        speedflag_lr = -6;
    else if(value >=-175&& value<=-151)
        speedflag_lr = -7;
    else if(value >=-200 && value <=-176)
        speedflag_lr =-8;
    else
        speedflag_lr = 0;

     ui->label_hspeed->setText(tr("%1\%").arg(speedflag_lr*12.5));

    if(speedflag_lr == 1 && speedtemp_lr!=1){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4C01FD00FD"),1,false);
        speedtemp_lr = 1;
        qDebug()<<speedtemp_lr;
    }
    else if(speedflag_lr == 2 && speedtemp_lr !=2){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4C02FD00FD"),1,false);

        speedtemp_lr = 2;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 3 && speedtemp_lr !=3){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4C03FD00FD"),1,false);

        speedtemp_lr = 3;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 4 && speedtemp_lr !=4){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4C04FD00FD"),1,false);

        speedtemp_lr = 4;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 5 && speedtemp_lr !=5){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4C05FD00FD"),1,false);

        speedtemp_lr =5;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 6 && speedtemp_lr !=6){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4C06FD00FD"),1,false);

        speedtemp_lr = 6;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 7 && speedtemp_lr !=7){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4C07FD00FD"),1,false);
                speedtemp_lr = 7;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 8 && speedtemp_lr !=8){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4C08FD00FD"),1,false);

        speedtemp_lr = 8;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -1 && speedtemp_lr != -1){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B5201FD00FD"),1,false);

        speedtemp_lr = -1;
       qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -2 && speedtemp_lr !=-2){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B5202FD00FD"),1,false);

        speedtemp_lr = -2;
       qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -3 && speedtemp_lr !=-3){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B5203FD00FD"),1,false);

        speedtemp_lr = -3;
       qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -4 && speedtemp_lr != -4){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B5204FD00FD"),1,false);

        speedtemp_lr = -4;
      qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -5 && speedtemp_lr != -5){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B5205FD00FD"),1,false);

        speedtemp_lr = -5;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -6 && speedtemp_lr !=-6){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B5206FD00FD"),1,false);

        speedtemp_lr = -6;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -7 && speedtemp_lr !=-7){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B5207FD00FD"),1,false);
        speedtemp_lr = -7;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -8 && speedtemp_lr != -8){
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B5208FD00FD"),1,false);
        speedtemp_lr = -8;
        qDebug()<<speedtemp_lr;

    }

    if(result == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
}



void MyMQTT::on_doubleSpinBox_valueChanged(double arg1)
{
    int result = 0;
    static double helper = 0;
    if(arg1 > helper)
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B6C46FD00FD"),1,false);

    else
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B6C52FD00FD"),1,false);
    helper = arg1;

    if(result == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
}

void MyMQTT::on_doubleSpinBox_2_valueChanged(double arg1)
{
    int result = 0;
    static double helper1 = 0;
    if(arg1 > helper1)
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B7246FD00FD"),1,false);

    else
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B7252FD00FD"),1,false);
    helper1 = arg1;

    if(result == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));

}



void MyMQTT::on_setPath_clicked()
{
     filePath = QFileDialog::
            getExistingDirectory(this,tr("设置文件保存目录"),".", QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(filePath.size() == 0)
        return;
    filePath += "/";
    ui->currentPath->setText(filePath);
}

void MyMQTT::on_openInfo_clicked()
{
    if(dataRcv)
    dataRcv->show();
}

void MyMQTT::on_downFile_clicked()  //DA数据文件 较小
{
    QString DAFileFullPath;
    QFileInfo DAFileInfo;
    QString DAFileName;
    int result = 0;

    if(ui->downFile->text()=="下发数据文件")
    DAFileFullPath = QFileDialog::getOpenFileName(this);

     QFile *DAFile = new QFile;;
    if(ui->downFile->text()=="发送中")
        {
        ui->downFile->setText("下发数据文件");  //发送不成功时复位按键
        DAFile->close();
        //return;

        }

    if(!DAFileFullPath.isEmpty())
    {
        DAFileInfo = QFileInfo(DAFileFullPath);
        DAFileName = DAFileInfo.absoluteFilePath();
        ui->downFile->setText("发送中");
        DAFile=new QFile(DAFileName);

        if(DAFileInfo.suffix()=="dat")    //判断文件后缀名，.dat则不能使用TEXT_IODEVICE格式打开，否则会丢失0x0d字符，导致文件发送出错
            DAFile->open(QIODevice::ReadOnly);
        else
        DAFile->open(QIODevice::ReadOnly | QIODevice::Text);//读取DA数据源文件，格式为TXT

        do{
        QByteArray DAPack =DAFile->read(794); //返回QBytearrary;

       result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray(DAPack),1,false);
         qDebug()<<"read";
        if(result == -1){
            QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("error"));
            DAFile->close();
             ui->downFile->setText("下发数据文件");
            return;
        }

        }while(DAFile->pos()<DAFile->size());

        qDebug()<<"read";
       ui->downFile->setText("下发数据文件");

    }

}


void MyMQTT::on_selectDataFile_clicked()
{
    ui->sendFile->setEnabled(false);
    QStringList fileNames;
    QFileInfo dataFileInfo;
    QString dataFileName;
    fileNames = QFileDialog::getOpenFileNames(this);
    for(int i =0;i < fileNames.size();i++){
        if(!dataFileList.contains(fileNames.at(i)))
        dataFileList.append(fileNames.at(i));
    }
    ui->sendFile->setEnabled(true);
    ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") + "添加" + QString::number(fileNames.size())+"个下发文件!");

}

void MyMQTT::on_sendFile_clicked()
{
    if(dataFileList.isEmpty())
        return;
    int topicIndex = ui->fileTopicNum->value();

    if(ui->sendFile->text()=="停止")
    {
    ui->sendFile->setText("下发");  //发送不成功时复位按键
        sFile[ui->fileTopicNum->value()]->stopSend();  //停止发送 终止线程
    return;
    }

    if(ui->sendFile->text()=="下发")     
        if(!dataFileList.isEmpty())
        {
            ui->sendFile->setText("停止");
            sFile[topicIndex]->init(dataFileList);


            sFile[topicIndex]->start();
            const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")
                            + "开始下发数据文件!";
            ui->fileLogShow->appendPlainText(content);


         }

}

void MyMQTT::on_fileTopicSet_clicked()
{
    static QString Rcvtopic = "";

    Rcvtopic = "/CC3200@PAD/"+QString::number(ui->fileTopicNum->value())+"/PADDATA/#";

    int topicIndex = ui->fileTopicNum->value();
    if(ui->fileTopicSet->text()=="确认"){
        ui->fileTopicSet->setText("取消");

        fileTopic = ui->fileTopic1->currentText()+ui->fileTopic2->currentText()+"/"
              +QString::number(topicIndex)+ui->fileTopic3->currentText();
        const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")
                        + QLatin1String("Set SendFileTopic: ")+fileTopic;
      //  m_client->unsubscribe(Rcvtopic);

        const QString content1 = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")
                        + QLatin1String("Add RcvFileTopic: ")+Rcvtopic;

        QMqttSubscription *sub = m_client->subscribe(Rcvtopic,0);

        fileRcv[topicIndex]->init(filePath,sub);

        connect(sFile[topicIndex],&sendFile::sendFinish,this,[this](){ui->sendFile->setText("下发");   ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")
                                                                                                                                  + "数据文件下发成功!");dataFileList.clear();});
        connect(sFile[topicIndex],&sendFile::sendCancel,this,[this](){ui->sendFile->setText("下发");   ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")
                                                                                                                                  + "停止数据文件下发!");});
        connect(sFile[topicIndex],&sendFile::startSend,this,[this](QString fileName){ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")
                                                                                                                                 + "下发："+fileName);});
       connect(sFile[topicIndex],&sendFile::toPublish,this,[this](const QByteArray &data){if((m_client->publish(fileTopic,data,0,false))<0)qDebug()<<"publish error";});

       connect(fileRcv[topicIndex],&normalFileRcv::sglWrite,this,&MyMQTT::fileLogUpdate);
       connect(fileRcv[topicIndex],&normalFileRcv::normalData,this,[this,topicIndex](QByteArray data){ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+data);if(data.startsWith("OK")){sFile[topicIndex]->nextFile();};                                                                                                   });
       ui->logShow->appendPlainText(content);
       ui->logShow->appendPlainText(content1);
    }
    else{
        ui->fileTopicSet->setText("确认");
        disconnect(sFile[topicIndex],nullptr,this,nullptr);//断开连接
        disconnect(fileRcv[topicIndex],nullptr,this,nullptr);//断开连接

        m_client->unsubscribe(Rcvtopic);
        fileRcv[topicIndex]->disable();
        const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")
                        + QLatin1String("Remove SendFileTopic: ")+fileTopic;
      //  m_client->unsubscribe(Rcvtopic);

        const QString content1 = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")
                        + QLatin1String("Unsubscribe RcvFileTopic: ")+Rcvtopic;
        ui->logShow->appendPlainText(content);
        ui->logShow->appendPlainText(content1);

    }

}

void MyMQTT::on_fileTopicNum_valueChanged(int arg1)
{
    if(fileRcv[arg1]->isSet()==true){
        ui->fileTopicSet->setText("取消");
    }
    else{
        ui->fileTopicSet->setText("确认");

    }

    if(sFile[arg1]->isRunning())
     {
        ui->sendFile->setText("停止");
    }
    else{
        ui->sendFile->setText("下发");
    }

    fileTopic = ui->fileTopic1->currentText()+ui->fileTopic2->currentText()+"/"
        +QString::number(ui->fileTopicNum->value())+ui->fileTopic3->currentText();

}


void MyMQTT::fileLogUpdate(unsigned int size,bool flag){

    if(flag == false){
        ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+tr("数据文件接收完成,文件大小:%1 Bytes").arg(size));
    }
    else {

         ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+"正在接收数据文件...");
     }


}

sendFile::sendFile(){

}


void sendFile::run(){

     runFlag = true;
     fileWait = true;
     qDebug()<<"run";
     for(int i = 0;i < fileList.size(); i++){

         fileDeal = new QFile(fileList.at(i));
         qDebug()<<fileList.at(i);

         QFileInfo fileinfo = QFileInfo(*fileDeal);
         emit startSend(fileinfo.fileName());

         fileDeal->open(QIODevice::ReadOnly);

//         if(fileList.at(i).endsWith("dat")||fileList.at(i).endsWith("png"))    //判断文件后缀名，.dat则不能使用TEXT_IODEVICE格式打开，否则会丢失0x0d字符，导致文件发送出错
//            fileDeal->open(QIODevice::ReadOnly);
//         else
//            fileDeal->open(QIODevice::ReadOnly | QIODevice::Text);//读取DA数据源文件，格式为TXT
         // qDebug()<<fileDeal->isOpen();

          //发送首帧
         QByteArray dataPack(794,0); //返回QBytearrary
         int index = fileList.at(i).indexOf("/0/");//得到图片的相对路径
         QString sendPath =fileList.at(i).mid(index+1);
         sendPath.insert(1,':');       //路径分割
         sendPath.replace('/','\\');  //斜杠替换
         QString SendFileName = "&&"+sendPath+"**";//audioFileInfo.fileName()//路径拼接
         int FileNameLen = strlen(SendFileName.toLatin1().data());

         dataPack.replace(0,FileNameLen,SendFileName.toUtf8());
         dataPack.prepend("#W");
         dataPack.append("@#$%");
         emit toPublish(dataPack);
         //qDebug()<<dataPack;

         do{
             QByteArray dataPack =fileDeal->read(794); //返回QBytearrary
             if(fileDeal->pos() ==fileDeal->size() ){ //最后一个数据包
                // qDebug()<<dataPack.size();
                 dataPack.prepend("#W");
                dataPack.append("%$#@");
                emit toPublish(dataPack);

             }
             else{

                 dataPack.prepend("#W");
                dataPack.append("@#$%");
                emit toPublish(dataPack);
                //qDebug()<<"publish";

             }

             QMutexLocker locker(&m_lock);
            if(!runFlag){
                qDebug()<<"stop";
                emit sendCancel();
                return;
            }
            msleep(50);


         }
         while(fileDeal->pos()<fileDeal->size());
         fileDeal->close();
         while(fileWait)
            sleep(1);
        fileWait = true;

     }
        QByteArray dataPack("#Wcomplete%$#@");
        dataPack.resize(800);
        emit toPublish(dataPack);


    emit sendFinish();

}



