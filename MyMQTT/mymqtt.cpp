#include "mymqtt.h"
#include "ui_mymqtt.h"

//QWebEngineView控件使用load（）函数加载一个Web页面
//实际上就是使用HTTP Get方法加载web页面
//这个控件可以加载本地的web页面，也可以加载外部的WEb页面
#include <QWebEngineView>

//设置执行字符集，指示char的执行字符集是UTF-8编码
//如果源文件中出现中文，必须要设置为:
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

//另外Qt5中关于中文的建议解决方案是：
//1.保证源程序源码文件是UTF-8,然后用以上预处理命令
//2.不用保证源码文件是UTF-8，但是源文件中出现中文用QStringLiteral这个宏来包含起来，那么也不会乱码

MyMQTT::MyMQTT(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyMQTT)
{
    ui->setupUi(this);

    //init是初始化的意思，cmd是command
    this->initForm();
    this->initCmd();
    //this->initWebMap();//关于导航地图

    m_client = new QMqttClient(this);

    // dashboard（商业智能仪表盘，是向企业展示度量信息和关键业务指标（KPI）现状的数据虚拟化工具）
    //线程
    for(int i = 0; i < 5; i++)
    {
     serverInfo[i] = new infoUpdate(this);
     serverInfo[i]->hflag = i;

     connect(serverInfo[i],&infoUpdate::infoToUi,this,&MyMQTT::updateInfo);
    }

    /*
    //文件发送、接收类
    for(int i=0; i<10; i++)
    {
        fileRcv[i]= new normalFileRcv();
        sFile[i] = new sendFile();
    }
    */

    dataRcv = nullptr;
    curSubItem = -1;
    connect(m_client, &QMqttClient::stateChanged, this, &MyMQTT::updateStateLog);
    connect(m_client, &QMqttClient::disconnected, this, &MyMQTT::brokerDisconnected);
    connect(ui->showMsg, &QPushButton::clicked, this, [this](){if(curSubItem >=0)subList.at(curSubItem)->show();});
    connect(ui->topicList, &QListWidget::doubleClicked, this, [this](){subList.at(curSubItem)->show();});
    connect(this, &MyMQTT::destroyed, this, &MyMQTT::dealClose);
}

MyMQTT::~MyMQTT()//析构函数
{
    delete ui;

    for(int i =0; i<subList.count(); i++)
        delete subList.at(i);

    delete m_client;

    for(int i =0; i<5; i++)
        delete serverInfo[i];

    if(dataRcv)
    {
        delete  dataRcv;
    }
/*
    for(int i=0; i<10; i++)
    {
        delete fileRcv[i];
        delete sFile[i];
    }
    */
}


/**
 * @brief UI初始化(使用NavlistView库)//实现导航栏
 * @param
 * @return
 */

void MyMQTT::initForm()
{
    //导航栏初始化
    ui->listView->setIcoColorBg(false);//设置页面背景色
    ui->listView->setColorLine(QColor(254, 255, 255));//设置线条颜色
    ui->listView->setColorBg(QColor(72, 82, 127), QColor(100, 114, 176), QColor(218, 223, 237, 150));
    ui->listView->setColorText(QColor(254, 255, 255), QColor(252, 252, 252), QColor(0, 0, 0));

    //设置数据方式
    QStringList listItem;
    //设置导航栏菜单
    //MQTT
    listItem.append("MQTT||0|");
    listItem.append("设置连接|MQTT||");
    //服务器信息
    listItem.append("服务器信息||0|");
    listItem.append("节点信息|服务器信息||");
    listItem.append("连接列表|服务器信息||");
    listItem.append("会话列表|服务器信息||");
    listItem.append("订阅列表|服务器信息||");
    //订阅/发布
    listItem.append("订阅/发布||0|");
    listItem.append("订阅主题|订阅/发布||");
    listItem.append("发布信息|订阅/发布||");
    listItem.append("参数指令|订阅/发布||");
    //文件传输
    listItem.append("文件传输||0|");
    listItem.append("ARM_SD|文件传输||");
    //传感信息
    listItem.append("传感信息||0|");
    listItem.append("位置信息|传感信息||");
    //帮助信息
    listItem.append("帮助信息||1|");

    //设置导航菜单左边图标
    ui->listView->setIcoStyle(NavListView::IcoStyle_Triangle);

    //应用导航栏
    ui->listView->setData(listItem);

  //MQTT设置连接输入初始化
  ui->portEdit->setText("1883");//端口号
  ui->hostEdit->setText("139.199.208.33");//IP地址
  ui->userEdit->setText("cehang");//用户名
  ui->pwEdit->setText("cehang001");//密码
  ui->clientIDEdit->setText("MyMQTT1");//用户ID
  ui->HearBeatEdit->setText("60");//监听频率为60/s
  ui->stackedWidget->setCurrentIndex(0);
 //设置连接-路径
  filePath = QDir::currentPath();
  ui->currentPath->setText(filePath);
  ui->currentPath->setReadOnly(true);//只读方式

  //服务器信息
  //设置连接列表
  QStringList clientheader;
  clientheader<<"节点"<<"客户端ID"<<"用户名"<<"IP地址"<<"端口"<<"清除会话"<<"协议版本"<<"心跳(秒)"<<"连接时间";
  ui->clientList->setColumnCount(9);//设置9个格子
  ui->clientList->setHorizontalHeaderLabels(clientheader);//存放以上9条信息
  ui->clientList->horizontalHeader()->setStretchLastSection(true);

  //设置会话列表
  QStringList sessionsheader;
  sessionsheader<<"节点"<<"客户端ID"<<"清除会话"<<"订阅数"<<"ExpiryInterval"<<"最大拥塞"<<"当前拥塞"<<"当前缓存信息"<<"丢弃消息"<<"等待释放";
  sessionsheader<<"投递消息"<<"入队消息"<<"创建时间";
  ui->sessionList->setColumnCount(13);//设置13个格子
  ui->sessionList->setHorizontalHeaderLabels(sessionsheader);//存放以上13条信息
  ui->sessionList->horizontalHeader()->setStretchLastSection(true);

  //设置订阅列表
  QStringList subscriptheader;
  subscriptheader<<"节点"<<"客户端ID"<<"主题"<<"安全质量";
  ui->subscriptionList->setColumnCount(4);//设置4个格子
  ui->subscriptionList->setHorizontalHeaderLabels(subscriptheader);//存放以上4条信息
  ui->subscriptionList->horizontalHeader()->setStretchLastSection(true);

  //设置订阅/发布
  ui->topicEdit->setText("/CC3200@SHIP/1/SHIPDATA/SENSORDATA");//订阅主题
  ui->pubMsgEdit->setText("Hello World!");//发布信息
  ui->pubTopicEdit->setText("/CC3200@SHIP/1/SHIPDATA/SENSORDATA");//发布主题

  ui->openInfo->setEnabled(false);
  ui->topicSet->setEnabled(false);

  //文件传输（到pad）
  //ui->fileTopicNum->setRange(0,100);//文件主题下发数量限制在9个及以下
}

/**
 * @brief 地图类初始化  目前没用到
 * 使用 QWebChannel
 * @param
 * @return
 */

void MyMQTT::initWebMap()
{
   mapJs = new jsDocument(this);//分配内存空间，实现QT与HTML页面通信
   webChnl = new QWebChannel(this);//借助QWebChannel，实现浏览器与页面的通信交互
   webChnl->registerObject(QStringLiteral("content"),mapJs);//向网页登记一个对象
   ui->mapView->page()->setWebChannel(webChnl);
   ui->mapView->setUrl(QUrl("qrc:/test.html"));//设置网页链接
   connect(mapJs, &jsDocument::recvdMsg, this,
           [this](const QString& msg)
           {
           });
}

/**
 * @brief 指令窗口初始化
 * QSignalMapper的使用
 * @param
 * @return
 */

void MyMQTT::initCmd()
{
    //设置指令主题（订阅/发布-参数指令）
    ui->cmdTopic1->addItem("/PCMQTT");
    ui->cmdTopic2->addItem("/CC3200@SHIP");
    ui->cmdTopic3->addItem("/CMD");

    //设置文件主题（文件传输）
    ui->fileTopic1->addItem("/PCMQTT");
    ui->fileTopic2->addItem("/CC3200@PAD");
    ui->fileTopic3->addItem("/DLFILE");

    //采集参数设置
    //AD采集点数
    ui->adNum->addItem(tr(" "));
    ui->adNum->addItem(tr("1K"));
    ui->adNum->addItem(tr("10K"));
    ui->adNum->addItem(tr("100K"));

    //AD采样率
    ui->adRate->addItem(tr(" "));
    ui->adRate->addItem(tr("0.35MSPS"));
    ui->adRate->addItem(tr("0.7MSPS"));
    ui->adRate->addItem(tr("1.4MSPS"));
    ui->adRate->addItem(tr("2.1MSPS"));

    //DA转换率
    ui->daRate->addItem(tr(" "));
    ui->daRate->addItem(tr("0.33MSPS"));
    ui->daRate->addItem(tr("0.68MSPS"));
    ui->daRate->addItem(tr("1.33MSPS"));
    ui->daRate->addItem(tr("2.00MSPS"));

    //用comboBox组合框，节省空间
    connect(ui->adNum,&QComboBox::currentTextChanged,this,&MyMQTT::adNumChange);
    connect(ui->adRate,&QComboBox::currentTextChanged,this,&MyMQTT::adRateChange);
    connect(ui->daRate,&QComboBox::currentTextChanged,this,&MyMQTT::daRateChange);

    //按键信号
    cmdSignal = new QSignalMapper(this);//信号映射器

    //设置signalmapper的转发规则
    connect(ui->sigle, SIGNAL(clicked()), cmdSignal, SLOT(map()));//单帧采集
    cmdSignal->setMapping(ui->sigle, "1");

    connect(ui->upSensor, SIGNAL(clicked()), cmdSignal, SLOT(map()));//上传传感器数据
    cmdSignal->setMapping(ui->upSensor, "2");

    connect(ui->upHistory, SIGNAL(clicked()), cmdSignal, SLOT(map()));//上传历史数据
    cmdSignal->setMapping(ui->upHistory, "3");
    //上传历史数据
    ui->folders->setPlaceholderText("num1");//上传文件夹
    ui->files->setPlaceholderText("num2");//上传文件

    connect(ui->upRealTime, SIGNAL(clicked()), cmdSignal, SLOT(map()));//实时上传
    cmdSignal->setMapping(ui->upRealTime, "4");

    connect(ui->upCollect, SIGNAL(clicked()), cmdSignal, SLOT(map()));//上传采集数据
    cmdSignal->setMapping(ui->upCollect, "5");

    connect(ui->Stop, SIGNAL(clicked()), cmdSignal, SLOT(map()));//停止上传
    cmdSignal->setMapping(ui->Stop, "6");

    connect(ui->reset, SIGNAL(clicked()), cmdSignal, SLOT(map()));//复位
    cmdSignal->setMapping(ui->reset, "7");

    //将转发的信号连接到最终的槽函数
    connect(cmdSignal, SIGNAL(mapped(const QString &)), this, SLOT(cmdDeal(const QString &)));
}
void MyMQTT::on_listView_pressed()//点击导航菜单栏，切换
{
    QModelIndex index = ui->listView->currentIndex();
    QString text = index.data().toString();
    ui->logShow->setHidden(false);//连接服务器显示框设置不隐藏

    //setCurrentIndex保留可见小部件的索引位置
    //如果没有当前的小部件，则当前索引为-1
    //默认情况下，此属性包含值-1，因为堆栈最初是空的
    if (text == "设置连接"|| text =="MQTT")
    {
        ui->stackedWidget->setCurrentIndex(0);//把设置连接设置为第1个索引
    }
    else if (text == "节点信息"||text =="服务器信息")
    {
        ui->stackedWidget->setCurrentIndex(1);//节点信息设置为第2个索引
    }
    else if(text =="连接列表")
    {
        ui->stackedWidget->setCurrentIndex(2);//连接列表设置为第3个索引
    }
    else if(text =="会话列表")
    {
        ui->stackedWidget->setCurrentIndex(3);//回话列表设置为第4个索引
    }
    else if(text =="订阅列表")
    {
        ui->stackedWidget->setCurrentIndex(4);//订阅列表设置为第5个索引
    }
    else if(text =="参数指令")
    {
        ui->stackedWidget->setCurrentIndex(5);//订阅主题设置为第6个索引
    }
    else if(text == "ARM_SD")
    {
        ui->stackedWidget->setCurrentIndex(6);//ARM_SD设置为第7个索引
    }
    else if(text == "订阅主题")
    {
        ui->stackedWidget->setCurrentIndex(7);//订阅主题设置为第8个索引
    }
    else if(text == "发布信息")
    {
        ui->stackedWidget->setCurrentIndex(8);//发布信息设置为第9个索引
    }
    else if(text == "位置信息")
    {
        ui->stackedWidget->setCurrentIndex(9);//位置信息设置为第10个索引
        ui->logShow->setHidden(true);//连接服务器显示框设置隐藏，因为地图要比较大的位置
       //mapJs->sendMsg(ui->mapView->page(),"hello world!");//弹出Javascript Alert对话框，显示(接收到Qt发送的消息：hello world!)
    }
}

/**
 * @brief 更新状态信息（连接与断开）
 * @param void
 * @return  null
 */

void MyMQTT::updateStateLog()
{
    const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
                    + QLatin1String(": State Change")
                    + QString::number(m_client->state());//设置日期、时间及刷新状态
    ui->logShow->appendPlainText(content);//显示到连接服务器显示框中（2019-06-20 15:15:23: State Change0）

    if(m_client->state()==2)//更新服务器信息
    {  //连接服务器之后线程启动
       for(int i=0; i<5; i++)
       {
            if (serverInfo[i]->isRunning())//如果线程已启动
            {
                serverInfo[i]->wait();  //等待线程结束
            }
           serverInfo[i]->start();//否则启动线程
       }
      //更新连接列表9格
      ui->clientList->horizontalHeader()->setStretchLastSection(false);
      for(int i = 0; i<9; i++)
      {
        ui->clientList->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
      }
      //更新会话列表13格
      ui->sessionList->horizontalHeader()->setStretchLastSection(false);
      for(int i = 0; i<13; i++)
      {
        ui->sessionList->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
      }
      //更新订阅列表4格
      // ui->subscriptionList->horizontalHeader()->setStretchLastSection(false);
      for(int i = 0; i<4; i++)
      {
        ui->subscriptionList->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
      }
      ui->topicSet->setEnabled(true);//连接后方能进行主题订阅
    }
    else if(m_client->state()==0)
    { //断开连接后，进程停止
        for(int i=0; i<5; i++)
        {
         serverInfo[i]->requestInterruption();//请求中断
         serverInfo[i]->quit();//等待进程结束，回收空间
        }
       //更新连接列表
       while(ui->clientList->rowCount()>0)
       {
         ui->clientList->removeRow(0);
       }
        ui->clientList->horizontalHeader()->setStretchLastSection(true);
       //更新会话列表
       while(ui->sessionList->rowCount()>0)
       {
         ui->sessionList->removeRow(0);
       }
        ui->sessionList->horizontalHeader()->setStretchLastSection(true);
       //更新订阅列表
       while(ui->subscriptionList->rowCount()>0)
       {
        ui->subscriptionList->removeRow(0);
       }
        ui->subscriptionList->horizontalHeader()->setStretchLastSection(true);

        ui->topicSet->setEnabled(false);//断开连接后不能再订阅主题
    }
}

/**
 * @brief 断开连接
 * @param void
 * @return  null
 */

void MyMQTT::brokerDisconnected()//回调函数，解决异常
{
    qDebug()<<"disconnet";//应用程序输出disconnect
     ui->connectButton->setText(tr("Connect"));//设置连接由disconnect文本切换为connect
     ui->topicList->clear();//订阅主题列表清空
     ui->pubList->clear();//发布信息列表清空
     for(int i =0; i<subList.count(); i++)
         delete subList.at(i);
     subList.clear();
     if(dataRcv)//清空数据接收
     {
         dataRcv->close();
         dataRcv = nullptr;
     }
}

//按下连接按键
void MyMQTT::on_connectButton_clicked()
{
    //连接列表显示服务器信息
    m_client->setHostname(ui->hostEdit->text());
    m_client->setPort((ui->portEdit->text().toInt()));
    m_client->setUsername(ui->userEdit->text());
    m_client->setPassword(ui->pwEdit->text());
    m_client->setClientId(ui->clientIDEdit->text());
    m_client->setKeepAlive(ui->HearBeatEdit->text().toInt());
    //如果服务器没连接
    if (m_client->state() == QMqttClient::Disconnected)
    {
        ui->hostEdit->setEnabled(false);
        ui->portEdit->setEnabled(false);
        ui->userEdit->setEnabled(false);
        ui->pwEdit->setEnabled(false);
        ui->connectButton->setText(tr("Disconnect"));
        m_client->connectToHost(); //连接
     }
    else //若已连接，再按下
    {
        ui->hostEdit->setEnabled(true);
        ui->portEdit->setEnabled(true);
        ui->userEdit->setEnabled(true);
        ui->pwEdit->setEnabled(true);
        ui->connectButton->setText(tr("Connect"));
        m_client->disconnectFromHost();//断开
    }
}

/**
 * @brief MQTT PING响应
 * @param
 * @return
 */
//Ping是Windows、Unix和Linux系统下的一个命令
//属于一个通信协议，是TCP/IP协议的一部分
//利用“ping”命令可以检查网络是否连通，帮助我们分析和判定网络故障
void MyMQTT::pingResponseReceived()
{
    //设置连接——Ping
    const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
                + QLatin1String(" PingResponse");
    ui->logShow->appendPlainText(content);//按下Ping按键，显示（2019-06-20 17:57:15 PingResponse）
    disconnect(m_client, &QMqttClient::pingResponseReceived, this, &MyMQTT::pingResponseReceived);//避免心跳包频繁显示
}

void MyMQTT::on_pingButton_clicked()
{
    connect(m_client, &QMqttClient::pingResponseReceived, this, &MyMQTT::pingResponseReceived);

    m_client->requestPing();//按下Ping按键，客户端发送ping请求指令
}

//设置连接——ChangePath
void MyMQTT::on_setPath_clicked()
{
     filePath = QFileDialog::
            getExistingDirectory(this,tr("设置文件保存目录"),".", QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(filePath.size() == 0)
        return;
    filePath += "/";
    ui->currentPath->setText(filePath);
}

/**
 * @brief 更新dashboard信息
 * 在主UI中更新抓取到的MQTT dashboard信息
 * @param void
 * @return  null
 */
void MyMQTT::updateInfo(QStringList info,int type)
{
    switch(type)
    {
    case 0://设置连接
    {
        ui->version->setText(info.at(0));//版本号
        ui->brokenName->setText(info.at(1));//代理名称
        ui->uptime->setText(info.at(2));//运行时间
        ui->uptime->setCursorPosition(0);
        ui->datetime->setText(info.at(3));//系统时间
        break;
    }
    case 1://节点信息
    {
        ui->nodename->setText(info.at(0));//节点名称
        ui->memory->setText(info.at(2)+'/'+info.at(1));//内存
        ui->eprocess->setText(info.at(4)+'/'+info.at(3));// Erlang进程
        ui->maxfds->setText(info.at(5));//最大句柄数
        ui->systatus->setText(info.at(6));//系统状态
        ui->otpversion->setText(info.at(7));//OTP版本
        ui->cpu->setText(info.at(8)+'/'+info.at(9)+'/'+info.at(10));//CPU
        break;
    }
    case 2://连接列表
    {
        int rcout = ui->clientList->rowCount();
        while(rcout < info.size()/9)
        {   //用户增加时，添加一行
            ui->clientList->insertRow(rcout++);
        }
        for(int i = 0; i<info.size(); i=i+9)//更新列表内容
        {
            for(int j =0 ;j< 9;j++)
            {
                QTableWidgetItem *tableItem =new QTableWidgetItem();
                tableItem->setText(info.at(i+j));
                ui->clientList->setItem(i/9,j,tableItem);
            }
          // ui->clientList->item(i,0)->setText(info.at(i));
        }

        if(info.size()/9 < rcout)//停止更新
        {
            for(int k = 0; k < rcout; k++)
            {
                QTableWidgetItem *deleteItem =new QTableWidgetItem();
                deleteItem = ui->clientList->item(k,0);
                //qDebug() << deleteItem->text();
                if(!info.contains(deleteItem->text()))
                {
                    ui->clientList->removeRow(k);
                    break;
                }
            }
        }
        break;
    }
    case 3://会话列表
    {
        int rcout = ui->sessionList->rowCount();
        while(rcout < info.size()/11)
        {    //客户端会话增加时，添加一行
            ui->sessionList->insertRow(rcout++);
        }
        for(int i = 0; i<info.size(); i=i+13)//更新列表
        {
            for(int j =0 ;j< 13;j++)
            {
                QTableWidgetItem *tableItem =new QTableWidgetItem();
                tableItem->setText(info.at(i+j));
                ui->sessionList->setItem(i/13,j,tableItem);
            }
        }
        if(info.size()/13 < rcout)//停止更新
        {
            for(int k = 0; k < rcout; k++)
            {
                QTableWidgetItem *deleteItem =new QTableWidgetItem();
                deleteItem = ui->sessionList->item(k,0);
                //qDebug()<<deleteItem->text();
                if(!info.contains(deleteItem->text()))
                {
                    ui->sessionList->removeRow(k);
                    break;
                }
            }
        }
        break;
    }
    case 4://订阅列表
    {
        int rcout = ui->subscriptionList->rowCount();
        while(rcout < info.size()/4)
        {   //用户订阅增加时，添加一行
            ui->subscriptionList->insertRow(rcout++);
        }
        for(int i = 0; i<info.size(); i=i+4)//更新列表
        {
            for(int j =0 ;j< 4;j++)
            {
                QTableWidgetItem *tableItem =new QTableWidgetItem();
                tableItem->setText(info.at(i+j));
                ui->subscriptionList->setItem(i/4,j,tableItem);
            }
        }
        if(info.size()/4 < rcout)//停止更新
        {
            for(int k = 0; k < rcout; k++)
            {
                QTableWidgetItem *deleteItem =new QTableWidgetItem();
                deleteItem = ui->subscriptionList->item(k,0);
                //qDebug()<<deleteItem->text();
                if(!info.contains(deleteItem->text()))
                {
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

//点击订阅按钮
void MyMQTT::on_subscription_clicked()
{
   qDebug()<<subList.size();//应用程序输出订阅数
   QString topic = ui->topicEdit->text();//获取订阅主题
   quint8 qos = ui->qosEdit->value();//获取消息传输方式
   for(int i=0; i< ui->topicList->count(); i++)
   {
       QListWidgetItem *item_sel = ui->topicList->item(i);
       if(item_sel->text()== topic)
       {
           QFont font = item_sel->font();
          if(font.strikeOut())//若还未订阅，初始化
          {
            font.setStrikeOut(false);
            item_sel->setFont(font);
            auto newSub = m_client->subscribe(topic,qos);//新的订阅
            auto m_sub = new subscription(newSub);//分配内存空间

            subList.replace(i,m_sub);
            qDebug()<<subList.size();//应用程序输出订阅数
            connect(m_sub, &subscription::rcvToMainUpdate, this,
                    [this]()
                    { //显示已接消息数
                        ui->rcvNumShow->setText(tr("%1").arg(subList.at(curSubItem)->rcvCount));
                    });
            connect(m_sub, &subscription::staToMainUpdate, this,
                    [this]()
                    {   //状态显示是否已订阅
                        ui->subStaShow->setText(tr("%1").arg(subList.at(curSubItem)->subState));
                    });

            ui->topicList->setCurrentItem(nullptr);//清空订阅主题列表
            ui->topicList->setCurrentItem(item_sel);   //更新订阅主题列表
            return;
          }
          else//若已订阅
          {  //弹出对话框显示error 主题：/CC3200@SHIP/1/SHIPDATA/SENSORDATA已订阅
             QMessageBox::critical(this,tr("Error"),tr("主题：%1已经订阅").arg(topic));
             return;
          }
       }
   }
    auto newSub = m_client->subscribe(topic,qos);
    if(!newSub)
    {
        qDebug()<<"error";
        return;
    }
   auto m_sub = new subscription(newSub);
   subList << m_sub;
   //更新已接消息数
   connect(m_sub, &subscription::rcvToMainUpdate, this, [this](){ui->rcvNumShow->setText(tr("%1").arg(subList.at(curSubItem)->rcvCount));});
   //更新订阅状态
   connect(m_sub, &subscription::staToMainUpdate,this, [this](){ui->subStaShow->setText(tr("%1").arg(subList.at(curSubItem)->subState));});

   QListWidgetItem *item = new QListWidgetItem;
   item->setText(topic);
   //订阅主题列表显示已订阅的主题
   ui->topicList->addItem(item);
   ui->topicList->setCurrentItem(item);
   // qDebug()<<newSub->topic().filter();
}

//当按下打开消息窗口按键
void MyMQTT::on_openInfo_clicked()
{
    if(dataRcv)
    dataRcv->show();//弹出子窗口
}

//当订阅主题改变时
void MyMQTT::on_topicList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{

    curSubItem =  ui->topicList->row(current);//获取当前主题数

    if(curSubItem <0)  //如果为空
        return;

    int rcvNum = subList.at(curSubItem)->rcvCount;//获取已接消息数
    ui->rcvNumShow->setText(tr("%1").arg(rcvNum));//显示已接消息数

    ui->topicShow->setText(subList.at(curSubItem)->subTopic);//显示当前订阅主题
    ui->qosShow->setText(tr("%1").arg(subList.at(curSubItem)->subqos));//显示当前QoS
    ui->subStaShow->setText(tr("%1").arg(subList.at(curSubItem)->subState));//显示当前状态
}
//当按下退订按键时
void MyMQTT::on_unsubscription_clicked()
{
    subList.at(curSubItem)->m_sub->unsubscribe();
    QListWidgetItem *item = new QListWidgetItem;//分配内存空间
    item = ui->topicList->currentItem();//获取当前订阅主题
    QFont font;//设置字体？
    font.setStrikeOut(true);
    item->setFont(font);
}

//当按下发布按键时
void MyMQTT::on_publish_clicked()
{
    if (m_client->publish(ui->pubTopicEdit->text(),
                          ui->pubMsgEdit->text().toUtf8(),
                          ui->pubQosEdit->text().toUInt(),
                          ui->retainCheck->isChecked()) == -1)
    {//如果已与服务器断开连接，则不能发布消息，出现error：Could not publish message
      QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
      return;
    }
    auto newMsg = new publish;
    //获取发布的信息
    newMsg->topic=ui->pubTopicEdit->text();
    newMsg->message = ui->pubMsgEdit->text();
    newMsg->qos=ui->pubQosEdit->text().toUInt();
    newMsg->retain =ui->retainCheck->isChecked();
    //更新消息
    pubMsgList << newMsg;

    QListWidgetItem *item = new QListWidgetItem;//插入一个列表
    item->setText(newMsg->message);
    item->setToolTip(tr("topic:%1,qos:%2,retain:%3").arg(newMsg->topic).arg(newMsg->qos).arg(newMsg->retain));
    item->setWhatsThis("hello");
    item->setStatusTip("hello");

    ui->pubList->addItem(item);
    ui->pubList->setCurrentItem(item);
}

//当按下发布信息的清空按键
void MyMQTT::on_clearPub_clicked()
{
    ui->pubList->clear();//清空列表
    pubMsgList.clear();
}

/**
 * @brief 指令窗口主题设置
 * @param void
 * @return  void
 */

//按下参数指令的确认按键
void MyMQTT::on_topicSet_clicked()
{
    //获取当前的指令主题，组包
    cmdTopic = ui->cmdTopic1->currentText() + ui->cmdTopic2->currentText()+"/"
                    + QString::number(ui->cmdNum->value()) + ui->cmdTopic3->currentText();
    //获取日期、时间、指令主题，组包
    const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")
                    + QLatin1String("Current CMDTopic: ") + cmdTopic;

    dataRcv = new dataReceive(filePath);
    connect(dataRcv, &dataReceive::setRcvSub, this,
            [this](QString topic)
            {  //设置新的接收主题
               QMqttSubscription *dataSub = m_client->subscribe(topic,0);
               dataRcv->newRcvSub(dataSub);
            });
    qDebug() << "creat rcvhandle" ;//应用程序输出creat rcvhandle

    ui->openInfo->setEnabled(true);//打开信息窗口使能

    ui->logShow->appendPlainText(content);//上位机显示窗显示当前日期、时间、指令主题
}

//设置AD采集点数
void MyMQTT::adNumChange(const QString &arg1)
{
    int result = 0;
    if(arg1 == "1K")
    {
       result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("AC 08 AC"), 1 ,false);
    }
    else if(arg1 =="10K")
    {
       result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("AC 10 AC"), 1 ,false);
    }
    else if(arg1 =="100K")
    {
       result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("AC 20 AC"), 1 ,false);
    }

    if(result == -1)//显示error：Could not publish message
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
}

//设置AD采样率
void MyMQTT::adRateChange(const QString &arg1)
{
    int result = 0;

    if(arg1 == "2.1MSPS")
    {
       result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("AD 00 AD"),1,false);
    }
    else if(arg1 =="1.4MSPS")
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("AD 02 AD"),1,false);
    }
    else if(arg1 =="0.7MSPS")
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("AD 04 AD"),1,false);
    }
    else if(arg1 =="0.35MSPS")
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("AD 08 AD"),1,false);
    }

    if(result == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));

}

//设置DA转换率
void MyMQTT::daRateChange(const QString &arg1)
{
    int result = 0;

    if(arg1 == "2.00MSPS")
    {
       result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DA 00 DA"),1,false);
    }
    else if(arg1 =="1.33MSPS")
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DA 02 DA"),1,false);
    }
    else if(arg1 =="0.68MSPS")
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DA 04 DA"),1,false);
    }
    else if(arg1 =="0.33MSPS")
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DA 08 DA"),1,false);
    }

    if(result == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
}

//指令处理槽函数
void MyMQTT::cmdDeal(const QString &arg)
{
    int num = arg.toInt(nullptr,10);
    int result = 0;
    qDebug()<<num;

    switch (num)
    {
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
            int temp1 = multifunc::StrDecimalToDecimal(folder);//文件夹
            int temp2 = multifunc::StrDecimalToDecimal(file);//文件
            QString upload= "D3";//为啥要定义一个D3？
            QString t1 = multifunc::IntDecToStrHex(temp1);//类型转换
            QString t2 = multifunc::IntDecToStrHex(temp2);
            if(temp1<16 && temp1>0)//若选择文件夹少于16
              upload += " 0 ";
            else
              upload += " ";
              upload += t1;

            if(temp2<16 && temp2>0)//若选择文件少于16
              upload += " D1 D3 0";
            else
              upload += " D1 D3 ";
              upload += t2;
              upload += " D2";

            result = m_client->publish(cmdTopic+"/HISTORYCMD", multifunc::HexStrToByteArray(upload),1,false);
            break;
        }
    case 4: //实时上传
        {
        if(ui->upRealTime->text()=="实时上传")
        {
            ui->upRealTime->setText("停止上传");
            QString upload4 = "D4 00 D4";
            result= m_client->publish(cmdTopic+"/SENSORCMD", multifunc::HexStrToByteArray(upload4),1,false);
        }
        else
        {
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

//参数指令——电机控制
//垂直滑块
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

    ui->label_vspeed->setText(tr("%1\%").arg(speedflag*12.5));//将滑块值设定范围为-100% ~ 100%
    if(speedflag == 1 && speedtemp!=1)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4601FD00FD"),1,false);
        speedtemp = 1;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 2 && speedtemp !=2)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4602FD00FD"),1,false);
        speedtemp = 2;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 3 && speedtemp !=3)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4603FD00FD"),1,false);
        speedtemp = 3;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 4 && speedtemp !=4)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4604FD00FD"),1,false);
        speedtemp = 4;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 5 && speedtemp !=5)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4605FD00FD"),1,false);
        speedtemp =5;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 6 && speedtemp !=6)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4606FD00FD"),1,false);
        speedtemp = 6;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 7 && speedtemp !=7)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4607FD00FD"),1,false);
        speedtemp = 7;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 8 && speedtemp !=8)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4608FD00FD"),1,false);
        speedtemp = 8;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -1 && speedtemp != -1)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4201FD00FD"),1,false);
        speedtemp = -1;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -2 && speedtemp !=-2)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4202FD00FD"),1,false);
        speedtemp = -2;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -3 && speedtemp !=-3)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4203FD00FD"),1,false);
        speedtemp = -3;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -4 && speedtemp != -4)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4204FD00FD"),1,false);
        speedtemp = -4;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -5 && speedtemp != -5)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4205FD00FD"),1,false);
        speedtemp = -5;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -6 && speedtemp !=-6)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4206FD00FD"),1,false);
        speedtemp = -6;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -7 && speedtemp !=-7)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4207FD00FD"),1,false);
        speedtemp = -7;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -8 && speedtemp != -8)
    {
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B4208FD00FD"),1,false);
        speedtemp = -8;
        qDebug()<<speedtemp;
    }

    if(result == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
}

//水平滑块
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

//垂直滑块微调框部件
void MyMQTT::on_doubleSpinBox_valueChanged(double arg1)
{
    int result = 0;
    static double helper = 0;
    if(arg1 > helper)//正值
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B6C46FD00FD"),1,false);
    else//负值
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B6C52FD00FD"),1,false);
    helper = arg1;

    if(result == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
}
//水平滑块微调框部件
void MyMQTT::on_doubleSpinBox_2_valueChanged(double arg1)
{
    int result = 0;
    static double helper1 = 0;
    if(arg1 > helper1)//正值
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B7246FD00FD"),1,false);
    else//负值
        result= m_client->publish(cmdTopic+"/USUALCMD", multifunc::HexStrToByteArray("DF00DF2B7252FD00FD"),1,false);
    helper1 = arg1;

    if(result == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
}

//参数指令——下发数据文件
void MyMQTT::on_downFile_clicked()  //DA数据文件 较小
{
    QString DAFileFullPath;
    QFileInfo DAFileInfo;
    QString DAFileName;
    int result = 0;

    if(ui->downFile->text()=="下发数据文件")
    DAFileFullPath = QFileDialog::getOpenFileName(this);

    QFile *DAFile = new QFile;
    if(ui->downFile->text()=="发送中")
        {
        ui->downFile->setText("下发数据文件");  //发送不成功时复位按键
        DAFile->close();
        //return;
        }

    if(!DAFileFullPath.isEmpty())//若DA文件不为空
    {
        DAFileInfo = QFileInfo(DAFileFullPath);//获取路径
        DAFileName = DAFileInfo.absoluteFilePath();//获取文件名
        ui->downFile->setText("发送中");
        DAFile = new QFile(DAFileName);//分配内存空间

        if(DAFileInfo.suffix()=="dat")//判断文件后缀名，.dat则不能使用TEXT_IODEVICE格式打开，否则会丢失0x0d字符，导致文件发送出错
            DAFile->open(QIODevice::ReadOnly);
        else
        DAFile->open(QIODevice::ReadOnly | QIODevice::Text);//读取DA数据源文件，格式为TXT

        do
        {
            QByteArray DAPack =DAFile->read(794); //返回QBytearrary;
            result= m_client->publish(cmdTopic + "/USUALCMD", multifunc::HexStrToByteArray(DAPack),1,false);
            qDebug()<<"read";
            if(result == -1)
            {
                QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("error"));
                DAFile->close();
                ui->downFile->setText("下发数据文件");
                return;
            }
        }while(DAFile->pos() < DAFile->size());
       // qDebug()<<"read";
       ui->downFile->setText("下发数据文件");
    }
}

/**
 * @brief 文件传输
 * @param void
 * @return  void
 */

//文件传输——确认按键1
void MyMQTT::on_fileTopicSet_clicked()//确认订阅
{
    //文件主题设置
    QString s = ui->fileTopicNum->text();
    fileTopic = ui->fileTopic1->currentText() + ui->fileTopic2->currentText()+"/" + s + ui->fileTopic3->currentText();

    //接收主题设置
    static QString Rcvtopic = "";
    Rcvtopic = "/CC3200@PAD/" + s + "/PADDATA/#";

    ui->fileTopicChoose->setEnabled(true);
    ui->selectDataFile->setEnabled(false);
    ui->sendFile->setEnabled(false);

    ///显示订阅主题 接收/下发
    //示例：2019-06-24 14:31:37 Set SendFileTopic: /PCMQTT/CC3200@PAD/1/DLFILE
    const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") + QLatin1String("Set SendFileTopic: ") + fileTopic;
    //示例：2019-06-24 14:31:37 Add RcvFileTopic: /CC3200@PAD/1/PADDATA/#
    const QString content1 = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") + QLatin1String("Add RcvFileTopic: ") + Rcvtopic;
    //logShow显示接收主题和下发主题
    ui->logShow->appendPlainText(content);
    ui->logShow->appendPlainText(content1);

    //将文件主题显示在下拉框中
    QStringList list;
    list.append(fileTopic);
    QCompleter *completer = new QCompleter(list, this);

    if(ui->topicDisplay->count() == 0)//若下拉框为空
    {
        qDebug()<<"还未订阅任何主题";
        ui->topicDisplay->addItems(list);
        ui->topicDisplay->setEditable(true);//把QComboBox设置为可以编辑输入
        ui->topicDisplay->setCompleter(completer);

       topicHandler *newTopic = new topicHandler(fileTopic);
       topicVector.push_back(newTopic);
       int number = ui->topicDisplay->findText(fileTopic);//给新主题编号
       qDebug() << "新主题编号：" << number;

       QMqttSubscription *sub = m_client->subscribe(Rcvtopic,0);//向服务器订阅
       topicVector[number]->getRcvHandler()->init(filePath,sub);

       connect(topicVector[number]->getSendHandler(), &sendFile::sendFinish, this, [this]()
          {ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+ "数据文件下发成功!"); dataFileList.clear();});

       connect(topicVector[number]->getSendHandler(), &sendFile::sendCancel, this, [this]()
          {ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") + "停止数据文件下发!");});

       connect(topicVector[number]->getSendHandler(), &sendFile::startSend, this, [this](QString fileName)
          {ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") + "下发："+fileName);});

       connect(topicVector[number]->getSendHandler(), &sendFile::toPublish, this, [this](const QByteArray &data)
          {if((m_client->publish(fileTopic,data,0,false))<0) qDebug()<<"publish error";});

       connect(topicVector[number]->getRcvHandler(), &normalFileRcv::sglWrite, this, &MyMQTT::fileLogUpdate);
       connect(topicVector[number]->getRcvHandler(), &normalFileRcv::normalData, this,
              [this,number](QByteArray data)
              {
                 //ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+data);
                 if(data.startsWith("OK")){topicVector[number]->getSendHandler()->nextFile();};
                 if(data.startsWith("P"))topicVector[number]->getSendHandler()->setHandShake();
              });
       qDebug()<< topicVector[number]->getSendHandler()->show();//打印出“true”
    }

    else//若不为空，判断内容是否已有
    {
        int flag = ui->topicDisplay->findText(fileTopic);
        if(flag > -1)//若已有，啥也不做
        {
            qDebug() << "该主题已订阅，请不要重复订阅";
            return;
        }
        else//若没有，则添加
        {
            qDebug() << "该主题还未订阅，可以订阅";
            //QComboBox显示
            ui->topicDisplay->addItems(list);
            ui->topicDisplay->setEditable(true);//把QComboBox设置为可以编辑输入
            ui->topicDisplay->setCompleter(completer);

            topicHandler *newTopic = new topicHandler(fileTopic);
            int number = ui->topicDisplay->findText(fileTopic);//找到新主题编号
            qDebug() << "新主题编号：" << number;
            topicVector.push_back(newTopic);

            QMqttSubscription *sub = m_client->subscribe(Rcvtopic,0);//向服务器订阅
            topicVector[number]->getRcvHandler()->init(filePath,sub);

            //这里要注意number的值，不能用当下索引，而应找到新主题编号
            connect(topicVector[number]->getSendHandler(), &sendFile::sendFinish, this, [this]()
              {ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+ "数据文件下发成功!"); dataFileList.clear();});

            connect(topicVector[number]->getSendHandler(), &sendFile::sendCancel, this, [this]()
              {ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") + "停止数据文件下发!");});

            connect(topicVector[number]->getSendHandler(), &sendFile::startSend, this, [this](QString fileName)
              {ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") + "下发："+fileName);});

            connect(topicVector[number]->getSendHandler(), &sendFile::toPublish, this, [this](const QByteArray &data)
              {if((m_client->publish(fileTopic,data,0,false))<0) qDebug()<<"publish error";});

            connect(topicVector[number]->getRcvHandler(), &normalFileRcv::sglWrite, this, &MyMQTT::fileLogUpdate);
            connect(topicVector[number]->getRcvHandler(), &normalFileRcv::normalData, this,
                   [this,number](QByteArray data)
                   {
                      //ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+data);
                      if(data.startsWith("OK")){topicVector[number]->getSendHandler()->nextFile();};
                      if(data.startsWith("P"))topicVector[number]->getSendHandler()->setHandShake();
                   });
            qDebug()<< topicVector[number]->getSendHandler()->show();//打印出“true”
        }
    }
}

void MyMQTT::on_pushButton_clicked()//取消订阅
{
    //获取当下主题
    QString s = ui->fileTopicNum->text();

    //下发主题设置
    fileTopic = ui->fileTopic1->currentText() + ui->fileTopic2->currentText()+"/" + s + ui->fileTopic3->currentText();

    //接收主题设置
    static QString Rcvtopic = "";
    Rcvtopic = "/CC3200@PAD/" + s + "/PADDATA/#";

    //取消订阅
    m_client->unsubscribe(Rcvtopic);

    //移除下拉框该主题
    int number = ui->topicDisplay->findText(fileTopic);//获取要删的主题编号
    qDebug() << "移除主题编号：" << number;
    ui->topicDisplay->removeItem(number);

    topicHandler *oldTopic = topicVector[number];
    topicVector.remove(number);//移除动态数组中该主题
    delete oldTopic;

    qDebug() << "QVector还剩：" << topicVector.count();

    //取消文件接收和下发功能
    disconnect(topicVector[number]->getSendHandler(), nullptr, this, nullptr);//断开连接
    disconnect(topicVector[number]->getRcvHandler(), nullptr, this, nullptr);//断开连接

    m_client->unsubscribe(Rcvtopic);
    topicVector[number]->getRcvHandler()->disable();

    //示例：2019-06-24 15:12:47 Remove SendFileTopic: /PCMQTT/CC3200@PAD/1/DLFILE
    const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")
                    + QLatin1String("Remove SendFileTopic: ")+fileTopic;

    //示例：2019-06-24 15:12:47 Unsubscribe RcvFileTopic: /CC3200@PAD/1/PADDATA/#
    const QString content1 = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")
                    + QLatin1String("Unsubscribe RcvFileTopic: ")+Rcvtopic;
    //logShow显示取消文件接收和下发
    ui->logShow->appendPlainText(content);
    ui->logShow->appendPlainText(content1);

    ui->fileTopicChoose->setEnabled(false);
    ui->selectDataFile->setEnabled(false);
    ui->sendFile->setEnabled(false);
}  

//文件传输——确定按键2
void MyMQTT::on_fileTopicChoose_clicked()//选择下发的订阅主题
{
    ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") + ui->topicDisplay->currentText());
    ui->selectDataFile->setEnabled(true);
}

//文件传输——选择文件
void MyMQTT::on_selectDataFile_clicked()
{
    dataFileList.clear();
    ui->sendFile->setEnabled(false);
    QStringList fileNames;
    QFileInfo dataFileInfo;
    QString dataFileName;
    fileNames = QFileDialog::getOpenFileNames(this);//获取文件名
    for(int i =0; i < fileNames.size(); i++)//文件个数，不是文件大小
    {
        if(!dataFileList.contains(fileNames.at(i)))
        dataFileList.append(fileNames.at(i));
    }
    ui->sendFile->setEnabled(true);
    //示例：2019-06-24 14:27:36 添加1个下发文件!
    ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") + "添加"
                                     + QString::number(fileNames.size())+"个下发文件!");
}

//文件传输——确定下发
void MyMQTT::on_sendFile_clicked()
{
    if(dataFileList.isEmpty())//若文件数目为0
        return;

    int number = ui->topicDisplay->currentIndex();//获取下拉框当前索引
    qDebug() << "下拉框当前索引：" << number;

    ////示例：2019-06-24 14:28:44 开始下发数据文件!
    const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") + "开始下发数据文件!";
    ui->fileLogShow->appendPlainText(content);

    topicVector[number]->getSendHandler()->setHandShake();//使handshake为真

    //开启线程
    topicVector[number]->getSendHandler()->init(dataFileList);
    topicVector[number]->getSendHandler()->start();//间接调用run(),run()不能直接调用
    qDebug() << "topicVector大小：" << topicVector.size();
}

//文件下发
sendFile::sendFile(){}
sendFile::~sendFile(){}

//文件下发线程
void sendFile::run()
{
     runFlag = true;
     fileWait = true;
     handshake = false;
     qDebug() << "文件发送线程开启";
     for(int i = 0; i < fileList.size(); i++)//fileList是文件数
     {
         fileDeal = new QFile(fileList.at(i));
         qDebug() << fileList.at(i);

         QFileInfo fileinfo = QFileInfo(*fileDeal);//获取fileDeal位置
         emit startSend(fileinfo.fileName());

         fileDeal->open(QIODevice::ReadOnly);//打开文件

          //判断文件后缀名，.dat则不能使用TEXT_IODEVICE格式打开，否则会丢失0x0d字符，导致文件发送出错
//         if(fileList.at(i).endsWith("dat")||fileList.at(i).endsWith("png"))
//            fileDeal->open(QIODevice::ReadOnly);
//         else
//            fileDeal->open(QIODevice::ReadOnly | QIODevice::Text);//读取DA数据源文件，格式为TXT
          //qDebug()<<fileDeal->isOpen();

          //发送首帧
//         QByteArray dataPack(794,0); //返回QBytearrary
//         int index = fileList.at(i).indexOf("/0/");//得到图片的相对路径
//         QString sendPath =fileList.at(i).mid(index+1);
//         sendPath.insert(1,':');       //路径分割
//         sendPath.replace('/','\\');  //斜杠替换
//         QString SendFileName = "&&"+sendPath+"**";//audioFileInfo.fileName()//路径拼接
//         int FileNameLen = strlen(SendFileName.toLatin1().data());

//         dataPack.replace(0,FileNameLen,SendFileName.toUtf8());
//         dataPack.prepend("#W");
//         dataPack.append("@#$%");
//         emit toPublish(dataPack);
          //qDebug()<<dataPack;

         //每一个数据包800B，格式为#W..数据..@#$%,  #W..最后一帧..%$#@
         do
         {
             QByteArray dataPack =fileDeal->read(794); //去掉帧头帧尾，返回QBytearrary
             if(fileDeal->pos() ==fileDeal->size() )//最后一个数据包
             {
                //qDebug()<<dataPack.size();
                dataPack.prepend("#W");//格式化
                dataPack.append("%$#@");
                emit toPublish(dataPack);
                qDebug()<<"最后一个";
             }
             else
             {
                dataPack.prepend("#W");//格式化
                dataPack.append("@#$%");
                emit toPublish(dataPack);
                qDebug()<< "稍等，还没发完";
             }
            while(!handshake)
            {
                 usleep(10);
                 if(!runFlag)
                 {
                     qDebug()<<"stop";
                     fileDeal->close();
                     emit sendCancel();
                     return;
                 }
            }
            handshake = false;
         }
         while(fileDeal->pos()<fileDeal->size());
         fileDeal->close();

         //while(fileWait)
         //sleep(1);
         fileWait = true;
     }
//        QByteArray dataPack("#Wcomplete%$#@");
//        dataPack.resize(800);
//        emit toPublish(dataPack);
          qDebug()<<"hah";
          emit sendFinish();
}

//停止下发
void MyMQTT::on_sendFileCancel_clicked()
{
    int number = ui->topicDisplay->currentIndex();//获取下拉框当前索引
    topicVector[number]->getSendHandler()->stopSend();//终止线程
    dataFileList.clear();
}

//接收文件槽函数
void MyMQTT::fileLogUpdate(unsigned int size,bool flag)
{
    if(flag == false)
    {
        //组包并显示在连接显示窗口
       ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+tr("数据文件接收完成,文件大小:%1 Bytes").arg(size));
    }
    else
    {
       ui->fileLogShow->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+"正在接收数据文件...");
    }
}

topicHandler::topicHandler(QString topic):fileTopic(topic){
    sFile = new sendFile;
    fileRcv = new normalFileRcv;
}

topicHandler::~topicHandler(){
    delete sFile;
    delete fileRcv;
}

void MyMQTT::dealClose()
{
//    if(sendFile.isRunning() == false)
//    {
//        return;
//    }
//    sendFile.quit();
//    sendFile.wait();
}



