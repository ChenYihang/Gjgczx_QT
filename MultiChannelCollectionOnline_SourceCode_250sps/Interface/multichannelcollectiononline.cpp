#include <multichannelcollectiononline.h>
#include <QStringList>
MultiChannelCollectionOnline::MultiChannelCollectionOnline(QWidget *parent)
    : QWidget(parent){
    Init();
    setLayout(mcco_mainLayout);
    setMinimumSize(1600,900);
    setContentsMargins(2,2,2,2);

    connect(mcco_onlineInterface,SIGNAL(OrderSend(QString)), this, SLOT(SendOrderToCollectionServer(QString)));

    mcco_collectionServer = 0;


    DoStop();
}

MultiChannelCollectionOnline::~MultiChannelCollectionOnline(){
    if(mcco_collectionServer){
        mcco_collectionServer->close();
        mcco_collectionServer->waitForFinished();
        delete mcco_collectionServer;
        mcco_collectionServer = 0;
    }
}

void MultiChannelCollectionOnline::Init(){
    mcco_online = new QPushButton("在线");
    mcco_online->setFixedHeight(35);
    connect(mcco_online,SIGNAL(clicked()),this,SLOT(DoOnline()));
    mcco_offline = new QPushButton("离线");
    mcco_offline->setFixedHeight(35);
    connect(mcco_offline,SIGNAL(clicked()),this,SLOT(DoOffLine()));
    mcco_stop = new QPushButton("停止");
    mcco_stop->setFixedHeight(35);
    connect(mcco_stop,SIGNAL(clicked()),this,SLOT(DoStop()));
    mcco_baseName = new QLabel("采样率:");
    mcco_baseName->setFixedHeight(35);
    mcco_base = new QLineEdit("200");
    mcco_base->setValidator(new QIntValidator(50,10000));
    mcco_base->setPlaceholderText("200(100~10000)");
    mcco_base->setFixedSize(100,35);
    connect(mcco_base,SIGNAL(textChanged(const QString)),this,SLOT(DoTextChanged(const QString)));
    mcco_base->setText("2000");
    mcco_portName = new QLabel("在线采集端口:");
    mcco_portName->setFixedHeight(35);
    mcco_port = new QLineEdit("5001");
    mcco_port->setValidator(new QIntValidator(1000,65535));
    mcco_port->setPlaceholderText("5001(1000~65535)");
    mcco_port->setFixedSize(100,35);

    mcco_replayUnitName = new QLabel("单位数据读取量:");
    mcco_replayUnitName->setFixedHeight(35);
    mcco_replayUnit = new QLineEdit("300");
    mcco_replayUnit->setValidator(new QIntValidator(1,2000));
    mcco_replayUnit->setPlaceholderText("10(1~2000)");
    mcco_replayUnit->setFixedSize(100,35);

    mcco_dataRateName = new QLabel("数据抽取间隙:");
    mcco_dataRateName->setFixedHeight(35);
    mcco_dataRate = new QLineEdit("20");
    mcco_dataRate->setValidator(new QIntValidator(1,2000));
    mcco_dataRate->setPlaceholderText("10(1~2000)");
    mcco_dataRate->setFixedSize(100,35);

    mcco_replaySpeedName = new QLabel("时域刷新频率(MS):");
    mcco_replaySpeedName->setFixedHeight(35);
    mcco_replaySpeed = new QLineEdit("10");
    mcco_replaySpeed->setValidator(new QIntValidator(1,1000));
    mcco_replaySpeed->setPlaceholderText("10(1~1000)");
    mcco_replaySpeed->setFixedSize(100,35);

    mcco_dataPathEdit = new QLineEdit();
    mcco_dataPathEdit->setPlaceholderText("数据目录");
    mcco_dataPathEdit->setReadOnly(true);
    mcco_dataPathEdit->setText(QCoreApplication::applicationDirPath());
//    mcco_dataPathEdit->setText("C:/Users/Bernie.Liu/Desktop/data");
    mcco_dataPathChoice = new QPushButton("选择");
    mcco_dataPathChoice->setStyleSheet("border:1px groove gray;");
    mcco_dataPathChoice->setCursor(Qt::PointingHandCursor);
    mcco_dataPathChoice->setFixedSize(60, 35);
    connect(mcco_dataPathChoice,SIGNAL(clicked()),this,SLOT(ChoiceDataPath()));
    QMargins margins = mcco_dataPathEdit->textMargins();
    mcco_dataPathEdit->setTextMargins(margins.left(), margins.top(), mcco_dataPathChoice->width(), margins.bottom());
    mcco_dataPathLayout = new QHBoxLayout();
    mcco_dataPathLayout->addStretch();
    mcco_dataPathLayout->addWidget(mcco_dataPathChoice);
    mcco_dataPathLayout->setSpacing(0);
    mcco_dataPathLayout->setContentsMargins(0, 0, 0, 0);
    mcco_dataPathEdit->setLayout(mcco_dataPathLayout);
    mcco_dataPathEdit->setFixedHeight(35);

    mcco_onlineInterface = OnlineInterface::GetInstance();
    mcco_offlineInterface = OfflineInterface::GetInstance();
    mcco_stopInterface =  new QWidget();
    mcco_stopLabel = new QLabel("Stop state!\n请点击在线或离线开始工作");
    mcco_stopLabel->setAlignment(Qt::AlignHCenter);
    mcco_stopLaout = new QHBoxLayout();
    mcco_stopLaout->addStretch();
    mcco_stopLaout->addWidget(mcco_stopLabel);
    mcco_stopLaout->addStretch();
    mcco_stopInterface->setLayout(mcco_stopLaout);

    mcco_buttonLayout = new QHBoxLayout();
    mcco_buttonLayout->setContentsMargins(0,0,0,0);
    mcco_mainLayout = new QVBoxLayout();
    mcco_mainLayout->setContentsMargins(0,0,0,0);
    mcco_buttonLayout->addWidget(mcco_online);
    mcco_buttonLayout->addWidget(mcco_offline);
    mcco_buttonLayout->addWidget(mcco_stop);
    mcco_buttonLayout->addSpacing(10);
    mcco_buttonLayout->addWidget(mcco_portName);
    mcco_buttonLayout->addWidget(mcco_port);
    mcco_buttonLayout->addSpacing(10);
    mcco_buttonLayout->addWidget(mcco_baseName);
    mcco_buttonLayout->addWidget(mcco_base);
    mcco_buttonLayout->addSpacing(10);
    mcco_buttonLayout->addWidget(mcco_replayUnitName);
    mcco_buttonLayout->addWidget(mcco_replayUnit);
    mcco_buttonLayout->addSpacing(10);
    mcco_buttonLayout->addWidget(mcco_dataRateName);
    mcco_buttonLayout->addWidget(mcco_dataRate);
    mcco_buttonLayout->addSpacing(10);
    mcco_buttonLayout->addWidget(mcco_replaySpeedName);
    mcco_buttonLayout->addWidget(mcco_replaySpeed);
    mcco_buttonLayout->addSpacing(10);
    mcco_buttonLayout->addWidget(mcco_dataPathEdit);
    mcco_mainLayout->addLayout(mcco_buttonLayout);
    mcco_mainLayout->addWidget(mcco_onlineInterface);
    mcco_mainLayout->addWidget(mcco_offlineInterface);
    mcco_mainLayout->addWidget(mcco_stopInterface);
}

void MultiChannelCollectionOnline::DoOnline(){
//    if(mcco_base->text().toInt() < 100)
//        mcco_base->setText("100");
    if(mcco_replayUnit->text().toInt() < 1)
        mcco_replayUnit->setText("10");
    int replayUnit = mcco_replayUnit->text().toInt();
    int dataRate = mcco_dataRate->text().toInt();
    if(dataRate < 1)
        mcco_dataRate->setText("10");
    else if(dataRate > replayUnit)
        mcco_dataRate->setText(mcco_replayUnit->text());
    else if(replayUnit % dataRate != 0){
        while(replayUnit%dataRate)dataRate++;
        mcco_dataRate->setText(QString("%1").arg(dataRate));
    }
    mcco_online->setEnabled(false);
    mcco_offline->setEnabled(true);
    mcco_stop->setEnabled(true);
    mcco_base->setEnabled(false);
    mcco_replayUnit->setEnabled(false);
    mcco_dataRate->setEnabled(false);
    mcco_port->setEnabled(false);
    mcco_replaySpeed->setEnabled(false);
    mcco_dataPathChoice->setEnabled(false);
    mcco_offlineInterface->setVisible(false);
    mcco_stopInterface->setVisible(false);
    mcco_onlineInterface->setVisible(true);
    mcco_offlineInterface->DoFree();
    mcco_onlineInterface->ChartStart(mcco_replayUnit->text().toInt(),mcco_dataRate->text().toInt(),mcco_dataPathEdit->text());
    if(mcco_port->text().toInt() < 1000)
        mcco_port->setText("8888");
    if(mcco_collectionServer){
        mcco_collectionServer->close();
        mcco_collectionServer->waitForFinished();
        delete mcco_collectionServer;
        mcco_collectionServer = 0;
    }

    //设置FFT点数
    mcco_onlineInterface->FFTNum = mcco_base->text().toInt() / mcco_dataRate->text().toInt();
    qDebug()<< mcco_onlineInterface->FFTNum;
    mcco_onlineInterface->oni_fftw.inData = new double[mcco_onlineInterface->FFTNum];
    mcco_onlineInterface->oni_fftw.outData = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (2 * ((mcco_onlineInterface->FFTNum / 2) + 1)));
    mcco_onlineInterface->oni_fftw.plan = fftw_plan_dft_r2c_1d(mcco_onlineInterface->FFTNum, mcco_onlineInterface->oni_fftw.inData, mcco_onlineInterface->oni_fftw.outData,FFTW_EXHAUSTIVE);
    for(int i = 0;i < 8; i++)
            for(int dataCur = 0; dataCur <=mcco_onlineInterface->FFTNum ; dataCur ++)
                mcco_onlineInterface->oni_frequencyChart.data[i]->append(dataCur,0);

    mcco_onlineInterface->FrequencyChanged(0);

    //
    mcco_collectionServer = new QProcess(this);
    connect(mcco_collectionServer, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(ProcessFinished(int, QProcess::ExitStatus)));
    connect(mcco_collectionServer, SIGNAL(error(QProcess::ProcessError)), this, SLOT(ProcessError(QProcess::ProcessError)));
    connect(mcco_collectionServer, SIGNAL(readyReadStandardOutput()), this, SLOT(ReadFromCollectionServer()));
    #if HIGH_PERFORMANCE
    QString program = "CollectionServer_Extend.exe";
    #else
    QString program = "CollectionServer.exe";
    #endif


    //QString program = "D:/QT_Project\build-MultiChannelCollectionOnline-Desktop_Qt_5_9_1_MinGW_32bit-Debug/CollectionServer/debug/CollectionServer.exe";
    QStringList arguments;
    arguments << mcco_port->text();
    arguments << mcco_dataPathEdit->text();
    //qDebug()<<mcco_dataPathEdit->text();
    mcco_collectionServer->start(program,arguments);
    bool a;
    a = mcco_collectionServer->waitForStarted();
    //qDebug()<<a;
}

void MultiChannelCollectionOnline::DoOffLine(){
    if(mcco_collectionServer){
        mcco_collectionServer->close();
        mcco_collectionServer->waitForFinished();
        delete mcco_collectionServer;
        mcco_collectionServer = 0;
    }
    if(mcco_base->text().toInt() < 100)
        mcco_base->setText("100");
    mcco_offline->setEnabled(false);
    mcco_online->setEnabled(true);
    mcco_stop->setEnabled(true);
    mcco_base->setEnabled(false);
    mcco_replayUnit->setEnabled(false);
    mcco_dataRate->setEnabled(true);
    mcco_port->setEnabled(true);
    mcco_replaySpeed->setEnabled(false);
    mcco_dataPathChoice->setEnabled(false);
    mcco_onlineInterface->setVisible(false);
    mcco_stopInterface->setVisible(false);
    mcco_offlineInterface->setVisible(true);
    mcco_onlineInterface->ChartStop();
    if(mcco_replaySpeed->text().toInt() < 1)
        mcco_replaySpeed->setText("10");
    mcco_offlineInterface->DoInit(mcco_replayUnit->text().toInt(),mcco_replaySpeed->text().toInt(),mcco_dataPathEdit->text());
}

void MultiChannelCollectionOnline::DoStop(){
    if(mcco_collectionServer){
        mcco_collectionServer->close();
        mcco_collectionServer->waitForFinished();
        delete mcco_collectionServer;
        mcco_collectionServer = 0;
    }
    mcco_online->setEnabled(true);
    mcco_offline->setEnabled(true);
    mcco_stop->setEnabled(false);
    mcco_base->setEnabled(true);
    mcco_replayUnit->setEnabled(true);
    mcco_dataRate->setEnabled(true);
    mcco_port->setEnabled(true);
    mcco_dataPathChoice->setEnabled(true);
    mcco_replaySpeed->setEnabled(true);
    mcco_onlineInterface->setVisible(false);
    mcco_offlineInterface->setVisible(false);
    mcco_stopInterface->setVisible(true);
    mcco_onlineInterface->ChartStop();
    mcco_offlineInterface->DoFree();
}

void MultiChannelCollectionOnline::SendOrderToCollectionServer(QString content){
    if(!mcco_collectionServer)
        return;
    QByteArray msg = content.toLatin1() + "\n";
    mcco_collectionServer->write(msg);
}

void MultiChannelCollectionOnline::ReadFromCollectionServer(){
    if(!mcco_collectionServer)
        return;
    QString msg = mcco_collectionServer->readAllStandardOutput();
    qDebug()<<msg;
    QStringList msgList = msg.split(",,,",QString::SkipEmptyParts);
    QStringList::Iterator it = msgList.begin();
    for(;it != msgList.end();it++)
        mcco_onlineInterface->MessageFromCollectionServer(*it);
}

void  MultiChannelCollectionOnline::ProcessFinished(int, QProcess::ExitStatus){
//    qDebug()<<"------采集服务进程停止";
}

void  MultiChannelCollectionOnline::ProcessError(QProcess::ProcessError error __attribute__((unused))){
//    qDebug()<<"------采集服务进程出错"<<error;
}

void MultiChannelCollectionOnline::DoTextChanged(const QString text){
    if(text == QString(""))
        DATA_CACHE_MAX[0] = 100;
    else
        DATA_CACHE_MAX[0] = text.toInt();
   // qDebug()<< DATA_CACHE_MAX[0];
    DATA_CACHE_MAX[1] = DATA_CACHE_MAX[0] * 3;
    DATA_CACHE_MAX[2] = DATA_CACHE_MAX[0] * 5;
    DATA_CACHE_MAX[3] = DATA_CACHE_MAX[0] * 7;
}

void MultiChannelCollectionOnline::closeEvent(QCloseEvent *){
    if(mcco_collectionServer){
        mcco_collectionServer->close();
        mcco_collectionServer->waitForFinished();
        delete mcco_collectionServer;
        mcco_collectionServer = 0;
    }
}

void MultiChannelCollectionOnline::ChoiceDataPath(){
    QString path = QFileDialog::
            getExistingDirectory(this,tr("查找目录"),".", QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(path.size() > 0)
        mcco_dataPathEdit->setText(path);
}
