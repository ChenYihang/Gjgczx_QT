#include <SubInterface/onlineinterface.h>
#include <time.h>
#include <SubInterface/interfacecommon.h>
#include <QDebug>
OnlineInterface *OnlineInterface::oni_onlineI = 0;
OnlineInterface::OnlineInterface(QWidget *parent) : QWidget(parent){
    oni_tab.tabBar = new QTabBar();
    oni_tab.tabBar->setDrawBase(false);
    oni_tab.tabBar->setFixedHeight(30);
    oni_tab.tabBar->setContentsMargins(0,0,0,0);
    oni_tab.tabBar->setExpanding(false);
    oni_tab.tabBar->setTabsClosable(true);
    oni_tab.tabBar->setStyleSheet("QTabBar::tab{height:30px;font:12px;}");
    connect(oni_tab.tabBar,SIGNAL(currentChanged(int)),this,SLOT(TabChanged(int)));
    connect(oni_tab.tabBar,SIGNAL(tabCloseRequested(int)),this,SLOT(TabClosed(int)));
    oni_mainLayout = new QVBoxLayout();
    oni_mainLayout->addWidget(oni_tab.tabBar);
    oni_bottomLayout = new QHBoxLayout();
    oni_bottomLayout->setContentsMargins(0,0,0,0);
    InitLeftInterface();
    InitRightInterface();
    TimeSlotChanged(0);
    RangeChanged(0);
    frequencyMagChanged(4);
    FilterChanged(3);
    //FrequencyChanged(0);
    oni_mainLayout->addLayout(oni_bottomLayout);
    oni_mainLayout->setContentsMargins(0,0,0,0);
    setLayout(oni_mainLayout);
    setContentsMargins(0,0,0,0);

    oni_interfaceFlushTimer = new QTimer();
    connect(oni_interfaceFlushTimer,SIGNAL(timeout()),this,SLOT(UpdateChart()));
    oni_RMSAndColorCard.updateTimer = new QTimer();
    connect(oni_RMSAndColorCard.updateTimer,SIGNAL(timeout()),this,SLOT(UpdateRMSAndColorCard()));
    oni_RMSAndColorCard.hasNewData = false;
    pfile = 0;
    wfile = 0 ;
}

OnlineInterface::~OnlineInterface(){
    DestoryLeftInterface();
    DestoryRightInterface();
    delete oni_tab.tabBar;
    delete oni_bottomLayout;
    delete oni_mainLayout;
    delete oni_interfaceFlushTimer;
    delete oni_RMSAndColorCard.updateTimer;
}

OnlineInterface *OnlineInterface::GetInstance(){
    if(!oni_onlineI)
        oni_onlineI = new OnlineInterface();
    return oni_onlineI;
}

void OnlineInterface::InitLeftInterface(){
    oni_collectStart = new QPushButton("开始");
    oni_collectStart->setStyleSheet("QPushButton {border:1px groove gray;border-top-left-radius:15px;border-top-right-radius:15px;}QPushButton:pressed {background-color: #8a8c8e;border-style:inset;}");
    oni_collectStart->setFixedSize(65,35);
    connect(oni_collectStart,SIGNAL(clicked()),this,SLOT(CollectionStart()));
    oni_collectName = new QLabel("采集");
    oni_collectName->setAlignment(Qt::AlignCenter);
    oni_collectName->setFixedSize(65,20);
    oni_collectEnd = new QPushButton("停止");
    oni_collectEnd->setStyleSheet("QPushButton {border:1px groove gray;border-bottom-left-radius:15px;border-bottom-right-radius:15px;}QPushButton:pressed {background-color: #8a8c8e;border-style:inset;}");
    oni_collectEnd->setFixedSize(65,35);
    connect(oni_collectEnd,SIGNAL(clicked()),this,SLOT(CollectionStop()));
    oni_collectLayout = new QVBoxLayout();
    oni_collectLayout->addWidget(oni_collectStart);
    oni_collectLayout->addWidget(oni_collectName);
    oni_collectLayout->addWidget(oni_collectEnd);
    oni_collectLayout->setSpacing(0);
    oni_collectLayout->setContentsMargins(0,0,0,0);

    oni_reset = new QPushButton("复位");
    oni_reset->setStyleSheet("QPushButton {border:1px groove gray;border-radius:15px;}QPushButton:pressed {background-color: #8a8c8e;border-style:inset;}");
    oni_reset->setFixedSize(55,90);
    connect(oni_reset,SIGNAL(clicked()),this,SLOT(DoReset()));

    oni_stimulateStart = new QPushButton("开始");
    oni_stimulateStart->setStyleSheet("QPushButton {border:1px groove gray;border-top-left-radius:15px;border-top-right-radius:15px;}QPushButton:pressed {background-color: #8a8c8e;border-style:inset;}");
    oni_stimulateStart->setFixedSize(65,35);
    connect(oni_stimulateStart,SIGNAL(clicked()),this,SLOT(StimulateStart()));
    oni_stimulateName = new QLabel("刺激");
    oni_stimulateName->setAlignment(Qt::AlignCenter);
    oni_stimulateName->setFixedSize(65,20);
    oni_stimulateEnd = new QPushButton("停止");
    oni_stimulateEnd->setStyleSheet("QPushButton {border:1px groove gray;border-bottom-left-radius:15px;border-bottom-right-radius:15px;}QPushButton:pressed {background-color: #8a8c8e;border-style:inset;}");
    oni_stimulateEnd->setFixedSize(65,35);
    connect(oni_stimulateEnd,SIGNAL(clicked()),this,SLOT(StimulateStop()));
    oni_stimulateLayout = new QVBoxLayout();
    oni_stimulateLayout->addWidget(oni_stimulateStart);
    oni_stimulateLayout->addWidget(oni_stimulateName);
    oni_stimulateLayout->addWidget(oni_stimulateEnd);
    oni_stimulateLayout->setSpacing(0);
    oni_stimulateLayout->setContentsMargins(0,0,0,0);

    oni_downDataEidt = new QTextEdit();
    oni_downDataEidt->setFixedSize(177,50);
    oni_downDataFormat = new QCheckBox("HEX");
    oni_downDataFormat->setFixedSize(60,25);
    oni_downDataSend = new QPushButton("下发");
    oni_downDataSend->setStyleSheet("QPushButton {border:1px groove gray;border-radius:5px;}QPushButton:pressed {background-color: #8a8c8e;border-style:inset;}");
    oni_downDataSend->setFixedSize(110,25);
    connect(oni_downDataSend,SIGNAL(clicked()),this,SLOT(DownDataText()));
    oni_downDataControl = new QHBoxLayout();
    oni_downDataControl->addWidget(oni_downDataFormat);
    oni_downDataControl->addWidget(oni_downDataSend);
    oni_downDataLayout = new QVBoxLayout();
    oni_downDataLayout->addWidget(oni_downDataEidt);
    oni_downDataLayout->addLayout(oni_downDataControl);

    oni_controlLayout = new QHBoxLayout();
    oni_controlLayout->addLayout(oni_collectLayout);
    oni_controlLayout->addWidget(oni_reset);
    oni_controlLayout->addLayout(oni_stimulateLayout);
    oni_controlLayout->addLayout(oni_downDataLayout);
    oni_controlLayout->setContentsMargins(0,0,0,0);

    oni_downFilePathEdit = new QLineEdit();
    oni_downFilePathEdit->setFixedSize(275,25);
    oni_downFilePathEdit->setPlaceholderText("请选择下发文件");
    oni_downFilePathEdit->setReadOnly(true);
    oni_downFilePathChoice = new QPushButton("选择");
    oni_downFilePathChoice->setStyleSheet("border:1px groove gray;");
    oni_downFilePathChoice->setCursor(Qt::PointingHandCursor);
    oni_downFilePathChoice->setFixedSize(50, 25);
    connect(oni_downFilePathChoice,SIGNAL(clicked()),this,SLOT(ChoiceDownDataPath()));
    QMargins margins = oni_downFilePathEdit->textMargins();
    oni_downFilePathEdit->setTextMargins(margins.left(), margins.top(), oni_downFilePathChoice->width(), margins.bottom());
    oni_downFilePathLayout = new QHBoxLayout();
    oni_downFilePathLayout->addStretch();
    oni_downFilePathLayout->addWidget(oni_downFilePathChoice);
    oni_downFilePathLayout->setSpacing(0);
    oni_downFilePathLayout->setContentsMargins(0, 0, 0, 0);
    oni_downFilePathEdit->setLayout(oni_downFilePathLayout);
    oni_downFileSubmit = new QPushButton("下发");
    oni_downFileSubmit->setStyleSheet("QPushButton {border:1px groove gray;border-radius:5px;}QPushButton:pressed {background-color: #8a8c8e;border-style:inset;}");
    oni_downFileSubmit->setFixedSize(110,25);
    connect(oni_downFileSubmit,SIGNAL(clicked()),this,SLOT(DownDataFile()));
    oni_downFileLayout = new QHBoxLayout();
    oni_downFileLayout->addWidget(oni_downFilePathEdit);
    oni_downFileLayout->addWidget(oni_downFileSubmit);
    oni_downFileLayout->setContentsMargins(0,0,0,0);

    //频域图初始化
    for(int i = 0; i < 8; i++){
        oni_frequencyChart.data[i] = new QLineSeries();
        switch (i) {
            case 0:
                oni_frequencyChart.data[i]->setName("01");
                oni_frequencyChart.data[i]->setPen(QPen(Qt::gray,0.7));
                break;
            case 1:
                oni_frequencyChart.data[i]->setName("02");
                oni_frequencyChart.data[i]->setPen(QPen(Qt::red,0.7));
                break;
            case 2:
                oni_frequencyChart.data[i]->setName("17");
                oni_frequencyChart.data[i]->setPen(QPen(Qt::green,0.7));
                break;
            case 3:
                oni_frequencyChart.data[i]->setName("18");
                oni_frequencyChart.data[i]->setPen(QPen(Qt::blue,0.7));
                break;
            case 4:
                oni_frequencyChart.data[i]->setName("33");
                oni_frequencyChart.data[i]->setPen(QPen(Qt::cyan,0.7));
                break;
            case 5:
                oni_frequencyChart.data[i]->setName("34");
                oni_frequencyChart.data[i]->setPen(QPen(Qt::magenta,0.7));
                break;
            case 6:
                oni_frequencyChart.data[i]->setName("49");
                oni_frequencyChart.data[i]->setPen(QPen(Qt::yellow,0.7));
                break;
            case 7:
                oni_frequencyChart.data[i]->setName("50");
                oni_frequencyChart.data[i]->setPen(QPen(Qt::black,0.7));
                break;
            default:
                break;
        }
        oni_frequencyChart.data[i]->setUseOpenGL(true);
//        for(int dataCur = 0; dataCur <= 200; dataCur ++)
//            oni_frequencyChart.data[i]->append(dataCur,0);
    }
    oni_frequencyChart.chart = new QChart();
    oni_frequencyChart.chart->setContentsMargins(0,0,0,0);
    oni_frequencyChart.chart->legend()->setAlignment(Qt::AlignBottom);
    oni_frequencyChart.chart->legend()->setContentsMargins(0,0,0,0);
    for(int i = 0; i < 8; i++)
        oni_frequencyChart.chart->addSeries(oni_frequencyChart.data[i]);

    oni_frequencyChart.axisX = new QValueAxis();
    oni_frequencyChart.axisX->setGridLineVisible(false);
    oni_frequencyChart.axisX->setTitleVisible(false);
    oni_frequencyChart.axisY = new QValueAxis();
    oni_frequencyChart.axisY->setGridLineVisible(false);
    oni_frequencyChart.axisY->setTitleVisible(false);
    oni_frequencyChart.axisY->setTickCount(11);

    oni_frequencyChart.chart->addAxis(oni_frequencyChart.axisX,Qt::AlignBottom);
    oni_frequencyChart.chart->addAxis(oni_frequencyChart.axisY,Qt::AlignLeft);

    for(int i = 0; i < 8; i++){
        oni_frequencyChart.data[i]->attachAxis(oni_frequencyChart.axisX);
        oni_frequencyChart.data[i]->attachAxis(oni_frequencyChart.axisY);
    }

    oni_frequencyChart.view = new QChartView();
    oni_frequencyChart.view->setContentsMargins(0,0,0,0);
    oni_frequencyChart.view->setFixedSize(415,400);
    oni_frequencyChart.view->setChart(oni_frequencyChart.chart);
    oni_frequencyChart.view->setRenderHint(QPainter::Antialiasing);

//    oni_fftw.inData = new double[INTERFACE_FREQUENCY_FFTW[2]];
//    oni_fftw.outData = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (2 * ((INTERFACE_FREQUENCY_FFTW[2] / 2) + 1)));
//    oni_fftw.plan = fftw_plan_dft_r2c_1d(200,oni_fftw.inData,oni_fftw.outData,FFTW_EXHAUSTIVE);


    oni_log = new QTextEdit();
    oni_log->setFixedWidth(400);
    oni_log->setReadOnly(true);
    oni_colorCard = new ColorCard();

    oni_leftLayout = new QVBoxLayout();
    oni_leftLayout->setContentsMargins(0,0,0,0);
    oni_leftLayout->addLayout(oni_controlLayout);
    oni_leftLayout->addLayout(oni_downFileLayout);
    oni_leftLayout->addWidget(oni_frequencyChart.view);
    oni_leftLayout->addLayout(oni_colorCard);
    oni_leftLayout->addWidget(oni_log);
    oni_leftLayout->setAlignment(oni_log,Qt::AlignHCenter);
    oni_leftLayout->setAlignment(oni_frequencyChart.view,Qt::AlignHCenter);
    oni_leftLayoutW = new QWidget();
    oni_leftLayoutW->setContentsMargins(0,0,0,0);
    oni_leftLayoutW->setLayout(oni_leftLayout);
    oni_leftLayoutW->setFixedWidth(415);
    oni_bottomLayout->addWidget(oni_leftLayoutW);
}
void OnlineInterface::InitRightInterface(){
    oni_timeSlot = new ComboBoxT("时隙(S):");

   oni_timeSlot->AddItem("1    ");
    oni_timeSlot->AddItem("3    ");
    oni_timeSlot->AddItem("5    ");
    oni_timeSlot->AddItem("7    ");

    connect(oni_timeSlot->GetBox(),SIGNAL(currentIndexChanged(int)),this,SLOT(TimeSlotChanged(int)));

    oni_range = new ComboBoxT("时域幅度(UV):");
    oni_range->AddItem("50   ");
    oni_range->AddItem("100  ");
    oni_range->AddItem("200  ");
    oni_range->AddItem("400  ");
    oni_range->AddItem("1000 ");
    oni_range->AddItem("10000");
    connect(oni_range->GetBox(),SIGNAL(currentIndexChanged(int)),this,SLOT(RangeChanged(int)));
    oni_rangeBase = 0;

    oni_channel = new ComboBoxT("通道:");
    oni_channel->AddItem("all  ");
    oni_channel->AddItem("01~16");
    oni_channel->AddItem("17~32");
    oni_channel->AddItem("33~48");
    oni_channel->AddItem("49~64");
    connect(oni_channel->GetBox(),SIGNAL(currentIndexChanged(int)),this,SLOT(ChannelChanged(int)));

    oni_frequency = new ComboBoxT("频谱范围");
    oni_frequency->AddItem("0~1/2   ");
    oni_frequency->AddItem("1/2~1   ");
    oni_frequency->AddItem("1/3~2/3  ");
    oni_frequency->AddItem("0~1  ");

    connect(oni_frequency->GetBox(),SIGNAL(currentIndexChanged(int)),this,SLOT(FrequencyChanged(int)));


    frequencyMag = new ComboBoxT("频谱幅度");
    frequencyMag->AddItem("50   ");
    frequencyMag->AddItem("100  ");
    frequencyMag->AddItem("200  ");
    frequencyMag->AddItem("400  ");
    frequencyMag->AddItem("1000 ");
    frequencyMag->AddItem("5000");
    connect(frequencyMag->GetBox(),SIGNAL(currentIndexChanged(int)),this,SLOT(frequencyMagChanged(int)));


    oni_filter = new ComboBoxT("滤波:");
    oni_filter->AddItem("无 ");
    oni_filter->AddItem("陷波50HZ");
    oni_filter->AddItem("带通1-3HZ");
    oni_filter->AddItem("带通4-30HZ");
    oni_filter->AddItem("带通4-30HZ+陷波50HZ");
    oni_filter->AddItem("带通1-90HZ+陷波50HZ");
    oni_filter->AddItem("带通4-90HZ+陷波50HZ");

    connect(oni_filter->GetBox(),SIGNAL(currentIndexChanged(int)),this,SLOT(FilterChanged(int)));




    oni_temperature = new NameLineEditT("温度(℃):");
    oni_voltage = new NameLineEditT("电压(V):");

    oni_comboBoxS = new QSplitter(Qt::Horizontal);
    oni_comboBoxS->setContentsMargins(11,0,11,0);
    oni_comboBoxS->addWidget(oni_channel);
    oni_comboBoxS->addWidget(oni_timeSlot);
    oni_comboBoxS->addWidget(oni_range);
    oni_comboBoxS->addWidget(oni_frequency);
    oni_comboBoxS->addWidget(frequencyMag);//频谱幅度
    oni_comboBoxS->addWidget(oni_filter);//滤波选项
    oni_filter->setEnabled(false);
    oni_comboBoxS->addWidget(oni_temperature);
    oni_comboBoxS->addWidget(oni_voltage);
    oni_comboBoxS->setStretchFactor(0,2);
    oni_comboBoxS->setStretchFactor(1,2);
    oni_comboBoxS->setStretchFactor(2,2);
    oni_comboBoxS->setStretchFactor(3,2);
    oni_comboBoxS->setStretchFactor(4,2);
    oni_comboBoxS->setStretchFactor(5,2);
    oni_comboBoxS->setStretchFactor(6,2);
    oni_comboBoxS->setStretchFactor(7,2);

    oni_comboBoxS->setHandleWidth(20);

    for(int i = 0; i < DATA_CHANNEL_COUNT; i++){
        oni_realData.data[i] = new QLineSeries();
        oni_realData.data[i]->setName(QString("%1").arg(i+1,2,10,QChar('0')));
        switch(i%16){
            case 0:
                oni_realData.data[i]->setPen(QPen(QColor(0XFF,0x00,0x00,255),0.7));
                break;
            case 1:
                oni_realData.data[i]->setPen(QPen(QColor(0X00,0xFF,0x00,255),0.7));
                break;
            case 2:
                oni_realData.data[i]->setPen(QPen(QColor(0X00,0x00,0xFF,255),0.7));
                break;
            case 3:
                oni_realData.data[i]->setPen(QPen(QColor(0X1C,0x1C,0x1C,255),0.7));
                break;
            case 4:
                oni_realData.data[i]->setPen(QPen(QColor(0X8B,0x00,0x8B,255),0.7));
                break;
            case 5:
                oni_realData.data[i]->setPen(QPen(QColor(0X8B,0x00,0x00,255),0.7));
                break;
            case 6:
                oni_realData.data[i]->setPen(QPen(QColor(0X00,0x8B,0x8B,255),0.7));
                break;
            case 7:
                oni_realData.data[i]->setPen(QPen(QColor(0XA9,0xA9,0xA9,255),0.7));
                break;
            case 8:
                oni_realData.data[i]->setPen(QPen(QColor(0X8A,0x2B,0xE2,255),0.7));
                break;
            case 9:
                oni_realData.data[i]->setPen(QPen(QColor(0X00,0xFF,0xFF,255),0.7));
                break;
            case 10:
                oni_realData.data[i]->setPen(QPen(QColor(0XFF,0x83,0xFA,255),0.7));
                break;
            case 11:
                oni_realData.data[i]->setPen(QPen(QColor(0XFF,0xA5,0x00,255),0.7));
                break;
            case 12:
                oni_realData.data[i]->setPen(QPen(QColor(0X00,0x64,0x00,255),0.7));
                break;
            case 13:
                oni_realData.data[i]->setPen(QPen(QColor(0XFF,0xFF,0x00,255),0.7));
                break;
            case 14:
                oni_realData.data[i]->setPen(QPen(QColor(0X2F,0x4F,0x4F,255),0.7));
                break;
            case 15:
                oni_realData.data[i]->setPen(QPen(QColor(0X00,0xBF,0xFF,255),0.7));
                break;
            default:

                break;
        }
        oni_realData.data[i]->setUseOpenGL(true);
        oni_dataFile.newData[i] = new QList<QPointF>();
    }
    oni_realData.curEndX = 0;

    QFont ft;
    ft.setPointSize(9);

    for(int i = 0; i < 4; i++){
        oni_chart[i].chart = new QChart();
        oni_chart[i].chart->setContentsMargins(0,0,0,0);
        oni_chart[i].chart->setAcceptTouchEvents(true);

        oni_chart[i].chart->legend()->setAlignment(Qt::AlignBottom);
        oni_chart[i].chart->legend()->setContentsMargins(0,0,0,0);
        oni_chart[i].chart->legend()->setFont(ft);


        oni_chart[i].chart->addSeries(oni_realData.data[i*16+0]);
        oni_chart[i].chart->addSeries(oni_realData.data[i*16+1]);
        oni_chart[i].chart->addSeries(oni_realData.data[i*16+2]);
        oni_chart[i].chart->addSeries(oni_realData.data[i*16+3]);
        oni_chart[i].chart->addSeries(oni_realData.data[i*16+4]);
        oni_chart[i].chart->addSeries(oni_realData.data[i*16+5]);
        oni_chart[i].chart->addSeries(oni_realData.data[i*16+6]);
        oni_chart[i].chart->addSeries(oni_realData.data[i*16+7]);
        oni_chart[i].chart->addSeries(oni_realData.data[i*16+8]);
        oni_chart[i].chart->addSeries(oni_realData.data[i*16+9]);
        oni_chart[i].chart->addSeries(oni_realData.data[i*16+10]);
        oni_chart[i].chart->addSeries(oni_realData.data[i*16+11]);
        oni_chart[i].chart->addSeries(oni_realData.data[i*16+12]);
        oni_chart[i].chart->addSeries(oni_realData.data[i*16+13]);
        oni_chart[i].chart->addSeries(oni_realData.data[i*16+14]);
        oni_chart[i].chart->addSeries(oni_realData.data[i*16+15]);
        oni_chart[i].axisXShow = new QValueAxis();
        oni_chart[i].axisXShow->setTitleVisible(false);
        oni_chart[i].axisXShow->setGridLineVisible(false);

        oni_chart[i].axisXValue = new QValueAxis();
        oni_chart[i].axisXValue->setVisible(false);
        oni_chart[i].axisXValue->setTitleVisible(false);
        oni_chart[i].axisXValue->setGridLineVisible(false);
        oni_chart[i].axisXValue->setMinorGridLineVisible(false);
        oni_chart[i].axisXValue->setShadesVisible(false);
        oni_chart[i].axisXValue->setLineVisible(false);
        oni_chart[i].axisXValue->setLabelsVisible(false);
        oni_chart[i].axisXValue->setGridLineVisible(false);

        oni_chart[i].axisY = new QCategoryAxis();
        QFont ft;
        ft.setPointSize(8);
        oni_chart[i].axisY->setGridLineVisible(false);
         oni_chart[i].axisY->setLabelsFont(ft);
        //oni_chart[i].axisY->setLineVisible(false);
      //  oni_chart[i].axisY->setLabelsVisible(false);


        oni_chart[i].chart->addAxis(oni_chart[i].axisXShow,Qt::AlignBottom);
        oni_chart[i].chart->addAxis(oni_chart[i].axisXValue,Qt::AlignTop);
        oni_chart[i].chart->addAxis(oni_chart[i].axisY,Qt::AlignLeft);

        oni_realData.data[i*16+0]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+0]->attachAxis(oni_chart[i].axisY);
        oni_realData.data[i*16+1]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+1]->attachAxis(oni_chart[i].axisY);
        oni_realData.data[i*16+2]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+2]->attachAxis(oni_chart[i].axisY);
        oni_realData.data[i*16+3]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+3]->attachAxis(oni_chart[i].axisY);
        oni_realData.data[i*16+4]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+4]->attachAxis(oni_chart[i].axisY);
        oni_realData.data[i*16+5]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+5]->attachAxis(oni_chart[i].axisY);
        oni_realData.data[i*16+6]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+6]->attachAxis(oni_chart[i].axisY);
        oni_realData.data[i*16+7]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+7]->attachAxis(oni_chart[i].axisY);
        oni_realData.data[i*16+8]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+8]->attachAxis(oni_chart[i].axisY);
        oni_realData.data[i*16+9]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+9]->attachAxis(oni_chart[i].axisY);
        oni_realData.data[i*16+10]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+10]->attachAxis(oni_chart[i].axisY);
        oni_realData.data[i*16+11]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+11]->attachAxis(oni_chart[i].axisY);
        oni_realData.data[i*16+12]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+12]->attachAxis(oni_chart[i].axisY);
        oni_realData.data[i*16+13]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+13]->attachAxis(oni_chart[i].axisY);
        oni_realData.data[i*16+14]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+14]->attachAxis(oni_chart[i].axisY);
        oni_realData.data[i*16+15]->attachAxis(oni_chart[i].axisXValue);
        oni_realData.data[i*16+15]->attachAxis(oni_chart[i].axisY);

        oni_chart[i].view = new QChartView();
        oni_chart[i].view->setChart(oni_chart[i].chart);
        oni_chart[i].view->setRenderHint(QPainter::Antialiasing);
        oni_chart[i].view->setContentsMargins(0,0,0,0);
        oni_chart[i].rmsGroup = new RMSGroup(oni_chart[i].view);
    }

    oni_splitH01to32 = new QSplitter(Qt::Horizontal);
    oni_splitH01to32->setHandleWidth(1);
    oni_splitH01to32->addWidget(oni_chart[0].view);
    oni_splitH01to32->addWidget(oni_chart[1].view);
    oni_splitH01to32->setContentsMargins(0,0,0,0);
    oni_splitH33to64 = new QSplitter(Qt::Horizontal);
    oni_splitH33to64->setHandleWidth(1);
    oni_splitH33to64->addWidget(oni_chart[2].view);
    oni_splitH33to64->addWidget(oni_chart[3].view);
    oni_splitH33to64->setContentsMargins(0,0,0,0);
    oni_splitV = new QSplitter(Qt::Vertical);
    oni_splitV->setHandleWidth(1);
    oni_splitV->setStretchFactor(0,1);
    oni_splitV->setStretchFactor(1,1);
    oni_splitV->addWidget(oni_splitH01to32);
    oni_splitV->addWidget(oni_splitH33to64);
    oni_splitV->setContentsMargins(0,0,0,0);

    oni_rightLayout = new QVBoxLayout();
    oni_rightLayout->setContentsMargins(0,0,0,0);
    oni_rightLayout->addWidget(oni_comboBoxS);
    oni_rightLayout->addWidget(oni_splitV);
    oni_bottomLayout->addLayout(oni_rightLayout);
}

void OnlineInterface::DestoryLeftInterface(){
    delete oni_collectStart;
    delete oni_collectName;
    delete oni_collectEnd;
    delete oni_collectLayout;
    delete oni_reset;
    delete oni_stimulateStart;
    delete oni_stimulateName;
    delete oni_stimulateEnd;
    delete oni_stimulateLayout;
    delete oni_downDataEidt;
    delete oni_downDataFormat;
    delete oni_downDataSend;
    delete oni_downDataControl;
    delete oni_downDataLayout;
    delete oni_controlLayout;
    delete oni_downFilePathChoice;
    delete oni_downFilePathLayout;
    delete oni_downFilePathEdit;
    delete oni_downFileSubmit;
    delete oni_downFileLayout;

    delete oni_frequencyChart.chart;
    delete oni_frequencyChart.view;
    delete oni_frequencyChart.axisX;
    delete oni_frequencyChart.axisY;
    for(int i = 0; i < 8; i++)
        delete oni_frequencyChart.data[i];

    fftw_destroy_plan(oni_fftw.plan);
//    fftw_destroy_plan(oni_fftw.plan60);
//    fftw_destroy_plan(oni_fftw.plan120);
    delete [] oni_fftw.inData;
    fftw_free(oni_fftw.outData);

    delete oni_log;
    delete oni_colorCard;
    delete oni_leftLayout;
}
void OnlineInterface::DestoryRightInterface(){
    delete oni_timeSlot;
    delete oni_range;
    delete oni_channel;
    delete oni_frequency;
    delete oni_comboBoxS;
    for(int i = 0; i < 4; i++){
        delete oni_chart[i].rmsGroup;
        delete oni_chart[i].chart;
        delete oni_chart[i].axisXShow;
        delete oni_chart[i].axisY;
        delete oni_chart[i].axisXValue;
        delete oni_chart[i].view;
    }
    for(int i = 0; i < DATA_CHANNEL_COUNT; i++){
        delete oni_realData.data[i];
        delete oni_dataFile.newData[i];
    }
    delete oni_splitH01to32;
    delete oni_splitH33to64;
    delete oni_splitV;
    delete oni_rightLayout;
}

bool OnlineInterface::UpdateTimeChart(){
    if(oni_dataFile.guid.size() == 0){
        QMutexLocker(&(oni_tab.mutex));
        if(oni_tab.tabBar->currentIndex() < 0)
            return false;
        oni_dataFile.guid = oni_tab.tabBar->accessibleTabName(oni_tab.tabBar->currentIndex());
    }
    if(!oni_dataFile.file.isOpen()&&filterChangeFlag ==0){  //文件没打开，
        if(oni_filter->CurrentIndex() == 0){ //无滤波的首次打开文件
            oni_dataFile.file.setFileName(oni_dataFile.basePath+
                                oni_dataFile.guid+
                                "/"+
                                FILE_FINAL_PREFIX+
                                QString("%1").arg((oni_dataFile.fileSeq),10,10,QChar('0'))+
                                FILE_FINAL_SUFFIX);
           // qDebug()<<oni_dataFile.file.fileName();
            oni_dataFile.file.open(QFile::ReadOnly);
            return false;

        }
        else if(oni_filter->CurrentIndex() == 1){//有滤波的首次打开文件
            oni_dataFile.file.setFileName(oni_dataFile.basePath+
                                oni_dataFile.guid+
                                "/"+"Filter_50HZ/"+
                                FILE_FINAL_PREFIX+
                                QString("%1").arg((oni_dataFile.fileSeq),10,10,QChar('0'))+
                                FILE_FINAL_SUFFIX);
            oni_dataFile.file.open(QFile::ReadOnly|QFile::Text);
            oni_dataFile.ts.setDevice(&(oni_dataFile.file));
            return false;


        }
        else if(oni_filter->CurrentIndex() == 2){
            oni_dataFile.file.setFileName(oni_dataFile.basePath+
                                oni_dataFile.guid+
                                "/"+"Filter_1_3HZ/"+
                                FILE_FINAL_PREFIX+
                                QString("%1").arg((oni_dataFile.fileSeq),10,10,QChar('0'))+
                                FILE_FINAL_SUFFIX);
            oni_dataFile.file.open(QFile::ReadOnly|QFile::Text);
            oni_dataFile.ts.setDevice(&(oni_dataFile.file));
            return false;

        }
        else if(oni_filter->CurrentIndex() == 3){
            oni_dataFile.file.setFileName(oni_dataFile.basePath+
                                oni_dataFile.guid+
                                "/"+"Filter_4_30HZ/"+
                                FILE_FINAL_PREFIX+
                                QString("%1").arg((oni_dataFile.fileSeq),10,10,QChar('0'))+
                                FILE_FINAL_SUFFIX);
            oni_dataFile.file.open(QFile::ReadOnly|QFile::Text);
            oni_dataFile.ts.setDevice(&(oni_dataFile.file));
            return false;

        }
        else if(oni_filter->CurrentIndex() == 4){
            oni_dataFile.file.setFileName(oni_dataFile.basePath+
                                oni_dataFile.guid+
                                "/"+"Filter_4_30_S50HZ/"+
                                FILE_FINAL_PREFIX+
                                QString("%1").arg((oni_dataFile.fileSeq),10,10,QChar('0'))+
                                FILE_FINAL_SUFFIX);
            oni_dataFile.file.open(QFile::ReadOnly|QFile::Text);
            oni_dataFile.ts.setDevice(&(oni_dataFile.file));
            return false;

        }
        else if(oni_filter->CurrentIndex() == 5){
            oni_dataFile.file.setFileName(oni_dataFile.basePath+
                                oni_dataFile.guid+
                                "/"+"Filter_1_90_S50HZ/"+
                                FILE_FINAL_PREFIX+
                                QString("%1").arg((oni_dataFile.fileSeq),10,10,QChar('0'))+
                                FILE_FINAL_SUFFIX);
            oni_dataFile.file.open(QFile::ReadOnly|QFile::Text);
            oni_dataFile.ts.setDevice(&(oni_dataFile.file));
            return false;

        }
        else if(oni_filter->CurrentIndex() == 6){
            oni_dataFile.file.setFileName(oni_dataFile.basePath+
                                oni_dataFile.guid+
                                "/"+"Filter_4_90_S50HZ/"+
                                FILE_FINAL_PREFIX+
                                QString("%1").arg((oni_dataFile.fileSeq),10,10,QChar('0'))+
                                FILE_FINAL_SUFFIX);
            oni_dataFile.file.open(QFile::ReadOnly|QFile::Text);
            oni_dataFile.ts.setDevice(&(oni_dataFile.file));
            return false;

        }
    }


    /* 滤波模式切换*/
    if(filterChangeFlag == 1){
        if(filterChangeTemp == 0)  //状态未更新
             return false;
        if(oni_dataFile.file.isOpen())
           oni_dataFile.file.close();

            if(oni_filter->CurrentIndex() == 0){//切换到模式0
                  oni_dataFile.fileSeq = filterChangeTemp;  //更新文件序号
            oni_dataFile.file.setFileName(oni_dataFile.basePath+
                                    oni_dataFile.guid+
                                    "/"+
                                    FILE_FINAL_PREFIX+
                                    QString("%1").arg((filterChangeTemp),10,10,QChar('0'))+
                                    FILE_FINAL_SUFFIX);
            oni_dataFile.file.open(QFile::ReadOnly);
            if(oni_dataFile.file.isOpen())
                filterChangeFlag = 0;
            return false;
        }
        else if(oni_filter->CurrentIndex() == 1){//切换到模式1

            oni_dataFile.fileSeq = filterChangeTemp;  //更新文件序号
            oni_dataFile.file.setFileName(oni_dataFile.basePath+
                oni_dataFile.guid+
                "/"+"Filter_50HZ/"+
                FILE_FINAL_PREFIX+
                QString("%1").arg((filterChangeTemp),10,10,QChar('0'))+
                FILE_FINAL_SUFFIX);
            oni_dataFile.file.open(QFile::ReadOnly|QFile::Text);
            oni_dataFile.ts.setDevice(&(oni_dataFile.file));
            if(oni_dataFile.file.isOpen())
                filterChangeFlag = 0;
            return false;

        }
        else if(oni_filter->CurrentIndex() == 2){//切换到模式2
                oni_dataFile.fileSeq = filterChangeTemp;  //更新文件序号

            oni_dataFile.file.setFileName(oni_dataFile.basePath+
                                oni_dataFile.guid+
                                "/"+"Filter_1_3HZ/"+
                                FILE_FINAL_PREFIX+
                                QString("%1").arg((filterChangeTemp),10,10,QChar('0'))+
                                FILE_FINAL_SUFFIX);
            oni_dataFile.file.open(QFile::ReadOnly|QFile::Text);
            oni_dataFile.ts.setDevice(&(oni_dataFile.file));
           // qDebug()<<"opened fileter file";
            if(oni_dataFile.file.isOpen())
                filterChangeFlag = 0;

            return false;
        }
        else if(oni_filter->CurrentIndex() == 3){//切换到模式2
                    oni_dataFile.fileSeq = filterChangeTemp;  //更新文件序号

                oni_dataFile.file.setFileName(oni_dataFile.basePath+
                                    oni_dataFile.guid+
                                    "/"+"Filter_4_30HZ/"+
                                    FILE_FINAL_PREFIX+
                                    QString("%1").arg((filterChangeTemp),10,10,QChar('0'))+
                                    FILE_FINAL_SUFFIX);
                oni_dataFile.file.open(QFile::ReadOnly|QFile::Text);
                oni_dataFile.ts.setDevice(&(oni_dataFile.file));
               // qDebug()<<"opened fileter file";
                if(oni_dataFile.file.isOpen())
                    filterChangeFlag = 0;

                return false;
            }
            else if(oni_filter->CurrentIndex() == 4){//切换到模式2
                        oni_dataFile.fileSeq = filterChangeTemp;  //更新文件序号

                    oni_dataFile.file.setFileName(oni_dataFile.basePath+
                                        oni_dataFile.guid+
                                        "/"+"Filter_4_30_S50HZ/"+
                                        FILE_FINAL_PREFIX+
                                        QString("%1").arg((filterChangeTemp),10,10,QChar('0'))+
                                        FILE_FINAL_SUFFIX);
                    oni_dataFile.file.open(QFile::ReadOnly|QFile::Text);
                    oni_dataFile.ts.setDevice(&(oni_dataFile.file));
                   // qDebug()<<"opened fileter file";
                    if(oni_dataFile.file.isOpen())
                        filterChangeFlag = 0;

                    return false;
                }
            else if(oni_filter->CurrentIndex() == 5){//切换到模式2
                        oni_dataFile.fileSeq = filterChangeTemp;  //更新文件序号

                    oni_dataFile.file.setFileName(oni_dataFile.basePath+
                                        oni_dataFile.guid+
                                        "/"+"Filter_1_90_S50HZ/"+
                                        FILE_FINAL_PREFIX+
                                        QString("%1").arg((filterChangeTemp),10,10,QChar('0'))+
                                        FILE_FINAL_SUFFIX);
                    oni_dataFile.file.open(QFile::ReadOnly|QFile::Text);
                    oni_dataFile.ts.setDevice(&(oni_dataFile.file));
                   // qDebug()<<"opened fileter file";
                    if(oni_dataFile.file.isOpen())
                        filterChangeFlag = 0;

                    return false;
                }
            else if(oni_filter->CurrentIndex() == 6){//切换到模式2
                        oni_dataFile.fileSeq = filterChangeTemp;  //更新文件序号

                    oni_dataFile.file.setFileName(oni_dataFile.basePath+
                                        oni_dataFile.guid+
                                        "/"+"Filter_4_90_S50HZ/"+
                                        FILE_FINAL_PREFIX+
                                        QString("%1").arg((filterChangeTemp),10,10,QChar('0'))+
                                        FILE_FINAL_SUFFIX);
                    oni_dataFile.file.open(QFile::ReadOnly|QFile::Text);
                    oni_dataFile.ts.setDevice(&(oni_dataFile.file));
                   // qDebug()<<"opened fileter file";
                    if(oni_dataFile.file.isOpen())
                        filterChangeFlag = 0;

                    return false;
                }



    }
    QVector<double> filterTemp;
    if(oni_filter->CurrentIndex() == 0 ){  //无滤波的文件读取
       oni_dataFile.buff = oni_dataFile.file.read(MQ_MESSAGE_LENGHT*oni_dataFile.realUnitCount);
       if(oni_dataFile.buff.size() < MQ_MESSAGE_LENGHT){  //文件读完
            if(oni_dataFile.buff.size() == 0){
                if(oni_dataFile.file.isOpen())
                    oni_dataFile.file.close();
                oni_dataFile.fileSeq ++;  //文件递进读取
                pfile++;
                Inlog(tr("P/W : %1/%2").arg(pfile).arg(wfile));
                oni_dataFile.file.setFileName(oni_dataFile.basePath+
                                        oni_dataFile.guid+
                                        "/"+
                                        FILE_FINAL_PREFIX+
                                        QString("%1").arg((oni_dataFile.fileSeq),10,10,QChar('0'))+
                                        FILE_FINAL_SUFFIX);
                oni_dataFile.file.open(QFile::ReadOnly);
                return false;
            }
        }
   }
   else if(oni_filter->CurrentIndex() ==1 ||oni_filter->CurrentIndex() == 2||oni_filter->CurrentIndex() == 3||oni_filter->CurrentIndex() ==4||oni_filter->CurrentIndex() == 5||oni_filter->CurrentIndex() == 6){ //有滤波的文件读取  注意切换标志
       for(int i = 0;i<64 * oni_dataFile.realUnitCount;i++){
           filterTemp.append(oni_dataFile.ts.readLine().toDouble());   //realUnitCount 个 64通道数据  filterBuff[oni]
          // if(i==0)
           // qDebug()<<oni_dataFile.ts.readLine().toDouble();
            }
//       qDebug()<<"done";
    if(oni_dataFile.ts.atEnd())
      {
        if(oni_dataFile.file.isOpen())
            oni_dataFile.file.close();
      //  qDebug()<<oni_dataFile.filterBuff[64*200];
         oni_dataFile.fileSeq ++;

         if(oni_filter->CurrentIndex() ==1){
         oni_dataFile.file.setFileName(oni_dataFile.basePath+
                             oni_dataFile.guid+
                             "/"+"Filter_50HZ/"+
                             FILE_FINAL_PREFIX+
                             QString("%1").arg((oni_dataFile.fileSeq),10,10,QChar('0'))+
                             FILE_FINAL_SUFFIX);
         qDebug()<<"check";
         }
         else if(oni_filter->CurrentIndex() == 2){
             oni_dataFile.file.setFileName(oni_dataFile.basePath+
                     oni_dataFile.guid+
                     "/"+"Filter_1_3HZ/"+
                     FILE_FINAL_PREFIX+
                     QString("%1").arg((oni_dataFile.fileSeq),10,10,QChar('0'))+
                     FILE_FINAL_SUFFIX);

         }
         else if(oni_filter->CurrentIndex() == 3){
             oni_dataFile.file.setFileName(oni_dataFile.basePath+
                     oni_dataFile.guid+
                     "/"+"Filter_4_30HZ/"+
                     FILE_FINAL_PREFIX+
                     QString("%1").arg((oni_dataFile.fileSeq),10,10,QChar('0'))+
                     FILE_FINAL_SUFFIX);

         }
         else if(oni_filter->CurrentIndex() == 4){
             oni_dataFile.file.setFileName(oni_dataFile.basePath+
                     oni_dataFile.guid+
                     "/"+"Filter_4_30_S50HZ/"+
                     FILE_FINAL_PREFIX+
                     QString("%1").arg((oni_dataFile.fileSeq),10,10,QChar('0'))+
                     FILE_FINAL_SUFFIX);

         }
         else if(oni_filter->CurrentIndex() == 5){
             oni_dataFile.file.setFileName(oni_dataFile.basePath+
                     oni_dataFile.guid+
                     "/"+"Filter_1_90_S50HZ/"+
                     FILE_FINAL_PREFIX+
                     QString("%1").arg((oni_dataFile.fileSeq),10,10,QChar('0'))+
                     FILE_FINAL_SUFFIX);

         }
         else if(oni_filter->CurrentIndex() == 6){
             oni_dataFile.file.setFileName(oni_dataFile.basePath+
                     oni_dataFile.guid+
                     "/"+"Filter_4_90_S50HZ/"+
                     FILE_FINAL_PREFIX+
                     QString("%1").arg((oni_dataFile.fileSeq),10,10,QChar('0'))+
                     FILE_FINAL_SUFFIX);

         }
         oni_dataFile.file.open(QFile::ReadOnly|QFile::Text);
         oni_dataFile.ts.setDevice(&(oni_dataFile.file));
         pfile++;
         Inlog(tr("P/W : %1/%2").arg(pfile).arg(wfile));
         return false;//
   }
    }

    //开始绘图
    if(oni_filter->CurrentIndex()==0)  //无滤波的绘图
    {
        int readCount = oni_dataFile.buff.size() / MQ_MESSAGE_LENGHT;  //计算读到的数据块数目
        int startPos;
        //幅值
        for(int i = oni_dataFile.dataRate-1; i < readCount; i+=oni_dataFile.dataRate){   //循环次数（点数）为单次读取文件数据量 / 抽样间隔
            startPos = i * MQ_MESSAGE_LENGHT;
            oni_realData.curEndX += oni_dataFile.dataRate;  //横轴
          //  qDebug()<< *oni_dataFile.newData[0];
            *oni_dataFile.newData[0]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+3,startPos+4,startPos+5))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][0]);
            *oni_dataFile.newData[1]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+27,startPos+28,startPos+29))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][1]);
            *oni_dataFile.newData[2]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+51,startPos+52,startPos+53))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][2]);
            *oni_dataFile.newData[3]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+75,startPos+76,startPos+77))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][3]);
            *oni_dataFile.newData[4]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+99,startPos+100,startPos+101))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][4]);
            *oni_dataFile.newData[5]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+123,startPos+124,startPos+125))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][5]);
            *oni_dataFile.newData[6]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+147,startPos+148,startPos+149))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][6]);
            *oni_dataFile.newData[7]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+171,startPos+172,startPos+173))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][7]);
            *oni_dataFile.newData[8]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+6,startPos+7,startPos+8))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][8]);
            *oni_dataFile.newData[9]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+30,startPos+31,startPos+32))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][9]);
            *oni_dataFile.newData[10]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+54,startPos+55,startPos+56))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][10]);
            *oni_dataFile.newData[11]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+78,startPos+79,startPos+80))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][11]);
            *oni_dataFile.newData[12]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+102,startPos+103,startPos+104))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][12]);
            *oni_dataFile.newData[13]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+126,startPos+127,startPos+128))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][13]);
            *oni_dataFile.newData[14]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+150,startPos+151,startPos+152))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][14]);
            *oni_dataFile.newData[15]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+174,startPos+175,startPos+176))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][15]);
            *oni_dataFile.newData[16]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+9,startPos+10,startPos+11))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][0]);
            *oni_dataFile.newData[17]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+33,startPos+34,startPos+35))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][1]);
            *oni_dataFile.newData[18]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+57,startPos+58,startPos+59))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][2]);
            *oni_dataFile.newData[19]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+81,startPos+82,startPos+83))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][3]);
            *oni_dataFile.newData[20]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+105,startPos+106,startPos+107))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][4]);
            *oni_dataFile.newData[21]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+129,startPos+130,startPos+131))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][5]);
            *oni_dataFile.newData[22]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+153,startPos+154,startPos+155))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][6]);
            *oni_dataFile.newData[23]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+177,startPos+178,startPos+179))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][7]);
            *oni_dataFile.newData[24]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+12,startPos+13,startPos+14))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][8]);
            *oni_dataFile.newData[25]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+36,startPos+37,startPos+38))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][9]);
            *oni_dataFile.newData[26]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+60,startPos+61,startPos+62))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][10]);
            *oni_dataFile.newData[27]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+84,startPos+85,startPos+86))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][11]);
            *oni_dataFile.newData[28]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+108,startPos+109,startPos+110))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][12]);
            *oni_dataFile.newData[29]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+132,startPos+133,startPos+134))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][13]);
            *oni_dataFile.newData[30]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+156,startPos+157,startPos+158))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][14]);
            *oni_dataFile.newData[31]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+180,startPos+181,startPos+182))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][15]);
            *oni_dataFile.newData[32]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+15,startPos+16,startPos+17))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][0]);
            *oni_dataFile.newData[33]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+39,startPos+40,startPos+41))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][1]);
            *oni_dataFile.newData[34]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+63,startPos+64,startPos+65))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][2]);
            *oni_dataFile.newData[35]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+87,startPos+88,startPos+89))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][3]);
            *oni_dataFile.newData[36]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+111,startPos+112,startPos+113))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][4]);
            *oni_dataFile.newData[37]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+135,startPos+136,startPos+137))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][5]);
            *oni_dataFile.newData[38]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+159,startPos+160,startPos+161))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][6]);
            *oni_dataFile.newData[39]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+183,startPos+184,startPos+185))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][7]);
            *oni_dataFile.newData[40]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+18,startPos+19,startPos+20))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][8]);
            *oni_dataFile.newData[41]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+42,startPos+43,startPos+44))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][9]);
            *oni_dataFile.newData[42]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+66,startPos+67,startPos+68))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][10]);
            *oni_dataFile.newData[43]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+90,startPos+91,startPos+92))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][11]);
            *oni_dataFile.newData[44]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+114,startPos+115,startPos+116))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][12]);
            *oni_dataFile.newData[45]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+138,startPos+139,startPos+140))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][13]);
            *oni_dataFile.newData[46]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+162,startPos+163,startPos+164))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][14]);
            *oni_dataFile.newData[47]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+186,startPos+187,startPos+188))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][15]);
            *oni_dataFile.newData[48]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+21,startPos+22,startPos+23))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][0]);
            *oni_dataFile.newData[49]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+45,startPos+46,startPos+47))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][1]);
            *oni_dataFile.newData[50]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+69,startPos+70,startPos+71))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][2]);
            *oni_dataFile.newData[51]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+93,startPos+94,startPos+95))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][3]);
            *oni_dataFile.newData[52]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+117,startPos+118,startPos+119))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][4]);
            *oni_dataFile.newData[53]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+141,startPos+142,startPos+143))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][5]);
            *oni_dataFile.newData[54]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+165,startPos+166,startPos+167))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][6]);
            *oni_dataFile.newData[55]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+189,startPos+190,startPos+191))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][7]);
            *oni_dataFile.newData[56]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+24,startPos+25,startPos+26))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][8]);
            *oni_dataFile.newData[57]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+48,startPos+49,startPos+50))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][9]);
            *oni_dataFile.newData[58]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+72,startPos+73,startPos+74))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][10]);
            *oni_dataFile.newData[59]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+96,startPos+97,startPos+98))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][11]);
            *oni_dataFile.newData[60]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+120,startPos+121,startPos+122))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][12]);
            *oni_dataFile.newData[61]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+144,startPos+145,startPos+146))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][13]);
            *oni_dataFile.newData[62]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+168,startPos+169,startPos+170))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][14]);
            *oni_dataFile.newData[63]<<QPointF(oni_realData.curEndX,(GET_VALUE(oni_dataFile.buff,startPos+192,startPos+193,startPos+194))+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][15]);
        }
    }
    else {
       // qDebug()<<"plotstart";
        int  startFilterPos = 0;
         for(int i = oni_dataFile.dataRate-1; i < oni_dataFile.realUnitCount; i+=oni_dataFile.dataRate){
           // qDebug()<<"dataRate = "<<oni_dataFile.dataRate;
           // qDebug()<<"realUnitCount = "<<oni_dataFile.realUnitCount;
          //  qDebug()<<"vec size is = "<<filterTemp.size();


             startFilterPos = i * 64;
            // qDebug()<<"startFilterPos = "<<startFilterPos;
             oni_realData.curEndX += oni_dataFile.dataRate;  //横轴
            //  qDebug()<< *oni_dataFile.newData[0];
             *oni_dataFile.newData[0]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+0]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][0]);
             *oni_dataFile.newData[1]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+8]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][1]);
             *oni_dataFile.newData[2]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+16]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][2]);
             *oni_dataFile.newData[3]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+24]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][3]);
             *oni_dataFile.newData[4]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+32]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][4]);
             *oni_dataFile.newData[5]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+40]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][5]);
             *oni_dataFile.newData[6]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+48]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][6]);
             *oni_dataFile.newData[7]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+56]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][7]);
             *oni_dataFile.newData[8]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+1]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][8]);
             *oni_dataFile.newData[9]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+9]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][9]);
             *oni_dataFile.newData[10]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+17]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][10]);
             *oni_dataFile.newData[11]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+25]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][11]);
             *oni_dataFile.newData[12]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+33]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][12]);
             *oni_dataFile.newData[13]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+41]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][13]);
             *oni_dataFile.newData[14]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+49]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][14]);
             *oni_dataFile.newData[15]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+57]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][15]);
             *oni_dataFile.newData[16]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+2]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][0]);
             *oni_dataFile.newData[17]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+10]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][1]);
             *oni_dataFile.newData[18]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+18]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][2]);
             *oni_dataFile.newData[19]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+26]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][3]);
             *oni_dataFile.newData[20]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+34]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][4]);
             *oni_dataFile.newData[21]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+42]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][5]);
             *oni_dataFile.newData[22]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+50]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][6]);
             *oni_dataFile.newData[23]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+58]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][7]);
             *oni_dataFile.newData[24]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+3]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][8]);
             *oni_dataFile.newData[25]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+11]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][9]);
             *oni_dataFile.newData[26]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+19]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][10]);
             *oni_dataFile.newData[27]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+27]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][11]);
             *oni_dataFile.newData[28]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+35]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][12]);
             *oni_dataFile.newData[29]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+43]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][13]);
             *oni_dataFile.newData[30]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+51]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][14]);
             *oni_dataFile.newData[31]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+59]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][15]);
             *oni_dataFile.newData[32]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+4]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][0]);
             *oni_dataFile.newData[33]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+12]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][1]);
             *oni_dataFile.newData[34]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+20]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][2]);
             *oni_dataFile.newData[35]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+28]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][3]);
             *oni_dataFile.newData[36]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+36]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][4]);
             *oni_dataFile.newData[37]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+44]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][5]);
             *oni_dataFile.newData[38]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+52]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][6]);
             *oni_dataFile.newData[39]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+60]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][7]);
             *oni_dataFile.newData[40]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+5]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][8]);
             *oni_dataFile.newData[41]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+13]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][9]);
             *oni_dataFile.newData[42]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+21]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][10]);
             *oni_dataFile.newData[43]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+29]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][11]);
             *oni_dataFile.newData[44]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+37]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][12]);
             *oni_dataFile.newData[45]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+45]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][13]);
             *oni_dataFile.newData[46]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+53]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][14]);
             *oni_dataFile.newData[47]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+61]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][15]);
             *oni_dataFile.newData[48]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+6]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][0]);
             *oni_dataFile.newData[49]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+14]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][1]);
             *oni_dataFile.newData[50]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+22]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][2]);
             *oni_dataFile.newData[51]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+30]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][3]);
             *oni_dataFile.newData[52]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+38]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][4]);
             *oni_dataFile.newData[53]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+46]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][5]);
             *oni_dataFile.newData[54]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+54]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][6]);
             *oni_dataFile.newData[55]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+62]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][7]);
             *oni_dataFile.newData[56]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+7]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][8]);
             *oni_dataFile.newData[57]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+15]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][9]);
             *oni_dataFile.newData[58]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+23]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][10]);
             *oni_dataFile.newData[59]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+31]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][11]);
             *oni_dataFile.newData[60]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+39]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][12]);
             *oni_dataFile.newData[61]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+47]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][13]);
             *oni_dataFile.newData[62]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+55]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][14]);
             *oni_dataFile.newData[63]<<QPointF(oni_realData.curEndX,filterTemp[startFilterPos+63]+INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][15]);
//             for(int j = 0;j<64;j++)
//             oni_dataFile.filterBuff.pop_front();]
         }
       //  qDebug()<<"plotend";

        // filterTemp.resize(0); //销毁内存

    }
    //画新点
    *(oni_realData.data[0])<<*(oni_dataFile.newData[0]);
    *(oni_realData.data[1])<<*(oni_dataFile.newData[1]);
    *(oni_realData.data[2])<<*(oni_dataFile.newData[2]);
    *(oni_realData.data[3])<<*(oni_dataFile.newData[3]);
    *(oni_realData.data[4])<<*(oni_dataFile.newData[4]);
    *(oni_realData.data[5])<<*(oni_dataFile.newData[5]);
    *(oni_realData.data[6])<<*(oni_dataFile.newData[6]);
    *(oni_realData.data[7])<<*(oni_dataFile.newData[7]);
    *(oni_realData.data[8])<<*(oni_dataFile.newData[8]);
    *(oni_realData.data[9])<<*(oni_dataFile.newData[9]);
    *(oni_realData.data[10])<<*(oni_dataFile.newData[10]);
    *(oni_realData.data[11])<<*(oni_dataFile.newData[11]);
    *(oni_realData.data[12])<<*(oni_dataFile.newData[12]);
    *(oni_realData.data[13])<<*(oni_dataFile.newData[13]);
    *(oni_realData.data[14])<<*(oni_dataFile.newData[14]);
    *(oni_realData.data[15])<<*(oni_dataFile.newData[15]);
    *(oni_realData.data[16])<<*(oni_dataFile.newData[16]);
    *(oni_realData.data[17])<<*(oni_dataFile.newData[17]);
    *(oni_realData.data[18])<<*(oni_dataFile.newData[18]);
    *(oni_realData.data[19])<<*(oni_dataFile.newData[19]);
    *(oni_realData.data[20])<<*(oni_dataFile.newData[20]);
    *(oni_realData.data[21])<<*(oni_dataFile.newData[21]);
    *(oni_realData.data[22])<<*(oni_dataFile.newData[22]);
    *(oni_realData.data[23])<<*(oni_dataFile.newData[23]);
    *(oni_realData.data[24])<<*(oni_dataFile.newData[24]);
    *(oni_realData.data[25])<<*(oni_dataFile.newData[25]);
    *(oni_realData.data[26])<<*(oni_dataFile.newData[26]);
    *(oni_realData.data[27])<<*(oni_dataFile.newData[27]);
    *(oni_realData.data[28])<<*(oni_dataFile.newData[28]);
    *(oni_realData.data[29])<<*(oni_dataFile.newData[29]);
    *(oni_realData.data[30])<<*(oni_dataFile.newData[30]);
    *(oni_realData.data[31])<<*(oni_dataFile.newData[31]);
    *(oni_realData.data[32])<<*(oni_dataFile.newData[32]);
    *(oni_realData.data[33])<<*(oni_dataFile.newData[33]);
    *(oni_realData.data[34])<<*(oni_dataFile.newData[34]);
    *(oni_realData.data[35])<<*(oni_dataFile.newData[35]);
    *(oni_realData.data[36])<<*(oni_dataFile.newData[36]);
    *(oni_realData.data[37])<<*(oni_dataFile.newData[37]);
    *(oni_realData.data[38])<<*(oni_dataFile.newData[38]);
    *(oni_realData.data[39])<<*(oni_dataFile.newData[39]);
    *(oni_realData.data[40])<<*(oni_dataFile.newData[40]);
    *(oni_realData.data[41])<<*(oni_dataFile.newData[41]);
    *(oni_realData.data[42])<<*(oni_dataFile.newData[42]);
    *(oni_realData.data[43])<<*(oni_dataFile.newData[43]);
    *(oni_realData.data[44])<<*(oni_dataFile.newData[44]);
    *(oni_realData.data[45])<<*(oni_dataFile.newData[45]);
    *(oni_realData.data[46])<<*(oni_dataFile.newData[46]);
    *(oni_realData.data[47])<<*(oni_dataFile.newData[47]);
    *(oni_realData.data[48])<<*(oni_dataFile.newData[48]);
    *(oni_realData.data[49])<<*(oni_dataFile.newData[49]);
    *(oni_realData.data[50])<<*(oni_dataFile.newData[50]);
    *(oni_realData.data[51])<<*(oni_dataFile.newData[51]);
    *(oni_realData.data[52])<<*(oni_dataFile.newData[52]);
    *(oni_realData.data[53])<<*(oni_dataFile.newData[53]);
    *(oni_realData.data[54])<<*(oni_dataFile.newData[54]);
    *(oni_realData.data[55])<<*(oni_dataFile.newData[55]);
    *(oni_realData.data[56])<<*(oni_dataFile.newData[56]);
    *(oni_realData.data[57])<<*(oni_dataFile.newData[57]);
    *(oni_realData.data[58])<<*(oni_dataFile.newData[58]);
    *(oni_realData.data[59])<<*(oni_dataFile.newData[59]);
    *(oni_realData.data[60])<<*(oni_dataFile.newData[60]);
    *(oni_realData.data[61])<<*(oni_dataFile.newData[61]);
    *(oni_realData.data[62])<<*(oni_dataFile.newData[62]);
    *(oni_realData.data[63])<<*(oni_dataFile.newData[63]);
    oni_dataFile.newData[0]->clear();
    oni_dataFile.newData[1]->clear();
    oni_dataFile.newData[2]->clear();
    oni_dataFile.newData[3]->clear();
    oni_dataFile.newData[4]->clear();
    oni_dataFile.newData[5]->clear();
    oni_dataFile.newData[6]->clear();
    oni_dataFile.newData[7]->clear();
    oni_dataFile.newData[8]->clear();
    oni_dataFile.newData[9]->clear();
    oni_dataFile.newData[10]->clear();
    oni_dataFile.newData[11]->clear();
    oni_dataFile.newData[12]->clear();
    oni_dataFile.newData[13]->clear();
    oni_dataFile.newData[14]->clear();
    oni_dataFile.newData[15]->clear();
    oni_dataFile.newData[16]->clear();
    oni_dataFile.newData[17]->clear();
    oni_dataFile.newData[18]->clear();
    oni_dataFile.newData[19]->clear();
    oni_dataFile.newData[20]->clear();
    oni_dataFile.newData[21]->clear();
    oni_dataFile.newData[22]->clear();
    oni_dataFile.newData[23]->clear();
    oni_dataFile.newData[24]->clear();
    oni_dataFile.newData[25]->clear();
    oni_dataFile.newData[26]->clear();
    oni_dataFile.newData[27]->clear();
    oni_dataFile.newData[28]->clear();
    oni_dataFile.newData[29]->clear();
    oni_dataFile.newData[30]->clear();
    oni_dataFile.newData[31]->clear();
    oni_dataFile.newData[32]->clear();
    oni_dataFile.newData[33]->clear();
    oni_dataFile.newData[34]->clear();
    oni_dataFile.newData[35]->clear();
    oni_dataFile.newData[36]->clear();
    oni_dataFile.newData[37]->clear();
    oni_dataFile.newData[38]->clear();
    oni_dataFile.newData[39]->clear();
    oni_dataFile.newData[40]->clear();
    oni_dataFile.newData[41]->clear();
    oni_dataFile.newData[42]->clear();
    oni_dataFile.newData[43]->clear();
    oni_dataFile.newData[44]->clear();
    oni_dataFile.newData[45]->clear();
    oni_dataFile.newData[46]->clear();
    oni_dataFile.newData[47]->clear();
    oni_dataFile.newData[48]->clear();
    oni_dataFile.newData[49]->clear();
    oni_dataFile.newData[50]->clear();
    oni_dataFile.newData[51]->clear();
    oni_dataFile.newData[52]->clear();
    oni_dataFile.newData[53]->clear();
    oni_dataFile.newData[54]->clear();
    oni_dataFile.newData[55]->clear();
    oni_dataFile.newData[56]->clear();
    oni_dataFile.newData[57]->clear();
    oni_dataFile.newData[58]->clear();
    oni_dataFile.newData[59]->clear();
    oni_dataFile.newData[61]->clear();
    oni_dataFile.newData[62]->clear();
    oni_dataFile.newData[63]->clear();
    //qDebug()<<"list size is"<<oni_dataFile.newData[0]->size();
    qreal curStartX = oni_realData.curEndX-DATA_CACHE_MAX[oni_timeSlot->CurrentIndex()];  //计算下一次绘图的坐标起点  DATA_CACHE_MAX决定X轴范围最大值
    //qDebug()<<curStartX<<oni_realData.curEndX;

    oni_chart[0].axisXValue->setRange(curStartX,oni_realData.curEndX);
    oni_chart[1].axisXValue->setRange(curStartX,oni_realData.curEndX);
    oni_chart[2].axisXValue->setRange(curStartX,oni_realData.curEndX);
    oni_chart[3].axisXValue->setRange(curStartX,oni_realData.curEndX);

//    qDebug()<<"curStartX = "<<curStartX;
    oni_dataFile.newData[60]->clear();
//    qDebug()<<"oni_realData.curEndX" << oni_realData.curEndX;


//    qDebug()<<"line count * 10"<<oni_realData.data[0]->count() *oni_dataFile.dataRate;
    int removeCount = oni_realData.data[0]->count()*oni_dataFile.dataRate - DATA_CACHE_MAX[3];
   //int removeCount = oni_realData.data[0]->count() - DATA_CACHE_MAX[3];

   //qDebug()<<"remove count"<<removeCount;
    if(removeCount > 0)
    for(int j = 0; j < 64; j++)
      oni_realData.data[j]->removePoints(0,removeCount/oni_dataFile.dataRate);
//    qDebug()<<removeCount;
 //    qDebug()<<"af line count"<<oni_realData.data[0]->count();

    return true;
}


void OnlineInterface::FrequencyChanged(int index){
    oni_frequencyChart.axisX->setTickCount(5);
    int inumTemp = FFTNum /2;
    switch (index) {
    case 0:
        oni_frequencyChart.axisX->setRange(0,inumTemp/2);
        break;
    case 1:
        oni_frequencyChart.axisX->setRange(inumTemp/2,inumTemp);
        break;
    case 2:
        oni_frequencyChart.axisX->setRange(inumTemp/3,2*inumTemp/3);
        break;
    case 3:
        oni_frequencyChart.axisX->setRange(0,inumTemp);
        break;
    default:
        break;
    }
}


void OnlineInterface::FilterChanged(int index){
    qDebug()<<"change";
    QMutexLocker locker(&oni_tab.mutex);
    if(oni_tab.tabBar->currentIndex() < 0){
        Inlog("无有效下位机接入");
        return;
    }
    filterChangeFlag = 1;
    Inlog("滤波操作");
    emit OrderSend(QString::number(FILTER) +
                   " " +
                   oni_tab.tabBar->accessibleTabName(oni_tab.tabBar->currentIndex())+QString::number(index,10));
  //  qDebug()<<QString::number(FILTER)<<" "<< oni_tab.tabBar->accessibleTabName(oni_tab.tabBar->currentIndex())<<QString::number(index,10);
}


void OnlineInterface::frequencyMagChanged(int index){
    switch(index){

    case 0: oni_frequencyChart.axisY->setRange(0,50);
    break;
    case 1: oni_frequencyChart.axisY->setRange(0,100);
    break;
    case 2: oni_frequencyChart.axisY->setRange(0,200);
    break;
    case 3: oni_frequencyChart.axisY->setRange(0,400);
    break;
    case 4: oni_frequencyChart.axisY->setRange(0,1000);
    break;
    case 5: oni_frequencyChart.axisY->setRange(0,5000);
    break;
    default:
           break;
     }

}



void OnlineInterface::UpdateFrequencyDataCalculation(QLineSeries *source,QLineSeries *object){
    oni_fftw.cursorFrom = oni_fftw.start;
    oni_fftw.cursorTo = 0;
    for(; oni_fftw.cursorFrom <= oni_fftw.end; oni_fftw.cursorFrom++,oni_fftw.cursorTo++)
        oni_fftw.inData[oni_fftw.cursorTo] = source->at(oni_fftw.cursorFrom).y();
    for(;oni_fftw.cursorTo < FFTNum; oni_fftw.cursorTo++)
        oni_fftw.inData[oni_fftw.cursorTo] = 0;

//    switch (oni_fftw.index) {
//        case 0:
//            fftw_execute(oni_fftw.plan30);
//            break;
//        case 1:
//            fftw_execute(oni_fftw.plan60);
//            break;
//        case 2:
//            fftw_execute(oni_fftw.plan120);
//            break;
//        default:
//            break;
//    }
     fftw_execute(oni_fftw.plan);


    oni_fftw.cursorFrom = 1;
    oni_fftw.cursorTo = 1;
    for(;oni_fftw.cursorTo <= FFTNum; oni_fftw.cursorFrom++,oni_fftw.cursorTo++){
        double tempSq = sqrt(  //计算出幅值
                    oni_fftw.outData[oni_fftw.cursorFrom][0] * oni_fftw.outData[oni_fftw.cursorFrom][0] + oni_fftw.outData[oni_fftw.cursorFrom][1]*oni_fftw.outData[oni_fftw.cursorFrom][1]
       )/FFTNum/2;
    object->replace(oni_fftw.cursorTo,  oni_fftw.cursorTo, tempSq );//考虑这里横坐标不做修改，在显示的时候再改名即可。更新2018年5月8日00:38:00果然横轴还是要改,sqrt的结果应该还要除以N/2，这里N是FFT操作的点数即120，直流量是除以N，暂时不管

    }
    //qDebug()<<oni_frequencyChart.data[0]->count();
   // qDebug()<<oni_fftw.cursorTo<<" "<<oni_fftw.cursorTo * oni_FS / 500<<" "<<tempSq;
    //qDebug()<<oni_frequencyChart.data[0]->count();
}

void OnlineInterface::UpdateFrequencyChart(){
    oni_fftw.index = oni_frequency->CurrentIndex();
    if(oni_realData.data[0]->count() > FFTNum){
        oni_fftw.end = oni_realData.data[0]->count() - 1;
        oni_fftw.start = oni_fftw.end - FFTNum + 1;
    }else{
        oni_fftw.end = oni_realData.data[0]->count() -1;
        oni_fftw.start = 0;
    }
   // qDebug()<<oni_realData.data[0]->count();
    UpdateFrequencyDataCalculation(oni_realData.data[0],oni_frequencyChart.data[0]);
    UpdateFrequencyDataCalculation(oni_realData.data[1],oni_frequencyChart.data[1]);
    UpdateFrequencyDataCalculation(oni_realData.data[16],oni_frequencyChart.data[2]);
    UpdateFrequencyDataCalculation(oni_realData.data[17],oni_frequencyChart.data[3]);
    UpdateFrequencyDataCalculation(oni_realData.data[32],oni_frequencyChart.data[4]);
    UpdateFrequencyDataCalculation(oni_realData.data[33],oni_frequencyChart.data[5]);
    UpdateFrequencyDataCalculation(oni_realData.data[48],oni_frequencyChart.data[6]);
    UpdateFrequencyDataCalculation(oni_realData.data[49],oni_frequencyChart.data[7]);

}


void OnlineInterface::UpdateChart(){
    QMutexLocker locker(&(oni_realData.mutex));

    if(!UpdateTimeChart())
        return;
    UpdateFrequencyChart();
    oni_RMSAndColorCard.hasNewData = true;
}

void OnlineInterface::TimeSlotChanged(int index){
    switch(index){
        case 0:
            oni_chart[0].axisXShow->setRange(-1,0);
            oni_chart[0].axisXShow->setTickCount(11);
            oni_chart[0].axisXShow->setLabelsVisible(true);

            oni_chart[1].axisXShow->setRange(-1,0);
            oni_chart[1].axisXShow->setTickCount(11);
            oni_chart[2].axisXShow->setRange(-1,0);
            oni_chart[2].axisXShow->setTickCount(11);
            oni_chart[3].axisXShow->setRange(-1,0);
            oni_chart[3].axisXShow->setTickCount(11);

            break;
        case 1:
        oni_chart[0].axisXShow->setRange(-3,0);
        oni_chart[0].axisXShow->setTickCount(11);
        oni_chart[1].axisXShow->setRange(-3,0);
        oni_chart[1].axisXShow->setTickCount(11);
        oni_chart[2].axisXShow->setRange(-3,0);
        oni_chart[2].axisXShow->setTickCount(11);
        oni_chart[3].axisXShow->setRange(-3,0);
        oni_chart[3].axisXShow->setTickCount(11);
        break;
    case 2:
            oni_chart[0].axisXShow->setRange(-5,0);
            oni_chart[0].axisXShow->setTickCount(11);
            oni_chart[1].axisXShow->setRange(-5,0);
            oni_chart[1].axisXShow->setTickCount(11);
            oni_chart[2].axisXShow->setRange(-5,0);
            oni_chart[2].axisXShow->setTickCount(11);
            oni_chart[3].axisXShow->setRange(-5,0);
            oni_chart[3].axisXShow->setTickCount(11);
            break;
        case 3:
            oni_chart[0].axisXShow->setRange(-7,0);
            oni_chart[0].axisXShow->setTickCount(11);
            oni_chart[1].axisXShow->setRange(-7,0);
            oni_chart[1].axisXShow->setTickCount(11);
            oni_chart[2].axisXShow->setRange(-7,0);
            oni_chart[2].axisXShow->setTickCount(11);
            oni_chart[3].axisXShow->setRange(-7,0);
            oni_chart[3].axisXShow->setTickCount(11);

            break;
        default:
            return;
            break;
    }
    qreal curStartX = oni_realData.curEndX - DATA_CACHE_MAX[oni_timeSlot->CurrentIndex()];
    oni_chart[0].axisXValue->setRange(curStartX,oni_realData.curEndX);
    oni_chart[1].axisXValue->setRange(curStartX,oni_realData.curEndX);
    oni_chart[2].axisXValue->setRange(curStartX,oni_realData.curEndX);
    oni_chart[3].axisXValue->setRange(curStartX,oni_realData.curEndX);
    // qDebug()<<curStartX<<oni_realData.curEndX;


}
void OnlineInterface::RangeChanged(int index){
    oni_chart[0].axisY->remove("01");
    oni_chart[0].axisY->remove("02");
    oni_chart[0].axisY->remove("03");
    oni_chart[0].axisY->remove("04");
    oni_chart[0].axisY->remove("05");
    oni_chart[0].axisY->remove("06");
    oni_chart[0].axisY->remove("07");
    oni_chart[0].axisY->remove("08");
    oni_chart[0].axisY->remove("09");
    oni_chart[0].axisY->remove("10");
    oni_chart[0].axisY->remove("11");
    oni_chart[0].axisY->remove("12");
    oni_chart[0].axisY->remove("13");
    oni_chart[0].axisY->remove("14");
    oni_chart[0].axisY->remove("15");
    oni_chart[0].axisY->remove("16");
    oni_chart[1].axisY->remove("17");
    oni_chart[1].axisY->remove("18");
    oni_chart[1].axisY->remove("19");
    oni_chart[1].axisY->remove("20");
    oni_chart[1].axisY->remove("21");
    oni_chart[1].axisY->remove("22");
    oni_chart[1].axisY->remove("23");
    oni_chart[1].axisY->remove("24");
    oni_chart[1].axisY->remove("25");
    oni_chart[1].axisY->remove("26");
    oni_chart[1].axisY->remove("27");
    oni_chart[1].axisY->remove("28");
    oni_chart[1].axisY->remove("29");
    oni_chart[1].axisY->remove("30");
    oni_chart[1].axisY->remove("31");
    oni_chart[1].axisY->remove("32");
    oni_chart[2].axisY->remove("33");
    oni_chart[2].axisY->remove("34");
    oni_chart[2].axisY->remove("35");
    oni_chart[2].axisY->remove("36");
    oni_chart[2].axisY->remove("37");
    oni_chart[2].axisY->remove("38");
    oni_chart[2].axisY->remove("39");
    oni_chart[2].axisY->remove("40");
    oni_chart[2].axisY->remove("41");
    oni_chart[2].axisY->remove("42");
    oni_chart[2].axisY->remove("43");
    oni_chart[2].axisY->remove("44");
    oni_chart[2].axisY->remove("45");
    oni_chart[2].axisY->remove("46");
    oni_chart[2].axisY->remove("47");
    oni_chart[2].axisY->remove("48");
    oni_chart[3].axisY->remove("49");
    oni_chart[3].axisY->remove("50");
    oni_chart[3].axisY->remove("51");
    oni_chart[3].axisY->remove("52");
    oni_chart[3].axisY->remove("53");
    oni_chart[3].axisY->remove("54");
    oni_chart[3].axisY->remove("55");
    oni_chart[3].axisY->remove("56");
    oni_chart[3].axisY->remove("57");
    oni_chart[3].axisY->remove("58");
    oni_chart[3].axisY->remove("59");
    oni_chart[3].axisY->remove("60");
    oni_chart[3].axisY->remove("61");
    oni_chart[3].axisY->remove("62");
    oni_chart[3].axisY->remove("63");
    oni_chart[3].axisY->remove("64");
    switch(index){
        case 0:
            oni_chart[0].axisY->append("01",100);
            oni_chart[0].axisY->append("02",200);
            oni_chart[0].axisY->append("03",300);
            oni_chart[0].axisY->append("04",400);
            oni_chart[0].axisY->append("05",500);
            oni_chart[0].axisY->append("06",600);
            oni_chart[0].axisY->append("07",700);
            oni_chart[0].axisY->append("08",800);
            oni_chart[0].axisY->append("09",900);
            oni_chart[0].axisY->append("10",1000);
            oni_chart[0].axisY->append("11",1100);
            oni_chart[0].axisY->append("12",1200);
            oni_chart[0].axisY->append("13",1300);
            oni_chart[0].axisY->append("14",1400);
            oni_chart[0].axisY->append("15",1500);
            oni_chart[0].axisY->append("16",1600);
            oni_chart[0].axisY->setRange(0,1600);
            oni_chart[1].axisY->append("17",100);
            oni_chart[1].axisY->append("18",200);
            oni_chart[1].axisY->append("19",300);
            oni_chart[1].axisY->append("20",400);
            oni_chart[1].axisY->append("21",500);
            oni_chart[1].axisY->append("22",600);
            oni_chart[1].axisY->append("23",700);
            oni_chart[1].axisY->append("24",800);
            oni_chart[1].axisY->append("25",900);
            oni_chart[1].axisY->append("26",1000);
            oni_chart[1].axisY->append("27",1100);
            oni_chart[1].axisY->append("28",1200);
            oni_chart[1].axisY->append("29",1300);
            oni_chart[1].axisY->append("30",1400);
            oni_chart[1].axisY->append("31",1500);
            oni_chart[1].axisY->append("32",1600);
            oni_chart[1].axisY->setRange(0,1600);
            oni_chart[2].axisY->append("33",100);
            oni_chart[2].axisY->append("34",200);
            oni_chart[2].axisY->append("35",300);
            oni_chart[2].axisY->append("36",400);
            oni_chart[2].axisY->append("37",500);
            oni_chart[2].axisY->append("38",600);
            oni_chart[2].axisY->append("39",700);
            oni_chart[2].axisY->append("40",800);
            oni_chart[2].axisY->append("41",900);
            oni_chart[2].axisY->append("42",1000);
            oni_chart[2].axisY->append("43",1100);
            oni_chart[2].axisY->append("44",1200);
            oni_chart[2].axisY->append("45",1300);
            oni_chart[2].axisY->append("46",1400);
            oni_chart[2].axisY->append("47",1500);
            oni_chart[2].axisY->append("48",1600);
            oni_chart[2].axisY->setRange(0,1600);
            oni_chart[3].axisY->append("49",100);
            oni_chart[3].axisY->append("50",200);
            oni_chart[3].axisY->append("51",300);
            oni_chart[3].axisY->append("52",400);
            oni_chart[3].axisY->append("53",500);
            oni_chart[3].axisY->append("54",600);
            oni_chart[3].axisY->append("55",700);
            oni_chart[3].axisY->append("56",800);
            oni_chart[3].axisY->append("57",900);
            oni_chart[3].axisY->append("58",1000);
            oni_chart[3].axisY->append("59",1100);
            oni_chart[3].axisY->append("60",1200);
            oni_chart[3].axisY->append("61",1300);
            oni_chart[3].axisY->append("62",1400);
            oni_chart[3].axisY->append("63",1500);
            oni_chart[3].axisY->append("64",1600);
            oni_chart[3].axisY->setRange(0,1600);
           // oni_frequencyChart.axisY->setRange(-50,50);
            break;
        case 1:
            oni_chart[0].axisY->append("01",200);
            oni_chart[0].axisY->append("02",400);
            oni_chart[0].axisY->append("03",600);
            oni_chart[0].axisY->append("04",800);
            oni_chart[0].axisY->append("05",1000);
            oni_chart[0].axisY->append("06",1200);
            oni_chart[0].axisY->append("07",1400);
            oni_chart[0].axisY->append("08",1600);
            oni_chart[0].axisY->append("09",1800);
            oni_chart[0].axisY->append("10",2000);
            oni_chart[0].axisY->append("11",2200);
            oni_chart[0].axisY->append("12",2400);
            oni_chart[0].axisY->append("13",2600);
            oni_chart[0].axisY->append("14",2800);
            oni_chart[0].axisY->append("15",3000);
            oni_chart[0].axisY->append("16",3200);
            oni_chart[0].axisY->setRange(0,3200);
            oni_chart[1].axisY->append("17",200);
            oni_chart[1].axisY->append("18",400);
            oni_chart[1].axisY->append("19",600);
            oni_chart[1].axisY->append("20",800);
            oni_chart[1].axisY->append("21",1000);
            oni_chart[1].axisY->append("22",1200);
            oni_chart[1].axisY->append("23",1400);
            oni_chart[1].axisY->append("24",1600);
            oni_chart[1].axisY->append("25",1800);
            oni_chart[1].axisY->append("26",2000);
            oni_chart[1].axisY->append("27",2200);
            oni_chart[1].axisY->append("28",2400);
            oni_chart[1].axisY->append("29",2600);
            oni_chart[1].axisY->append("30",2800);
            oni_chart[1].axisY->append("31",3000);
            oni_chart[1].axisY->append("32",3200);
            oni_chart[1].axisY->setRange(0,3200);
            oni_chart[2].axisY->append("33",200);
            oni_chart[2].axisY->append("34",400);
            oni_chart[2].axisY->append("35",600);
            oni_chart[2].axisY->append("36",800);
            oni_chart[2].axisY->append("37",1000);
            oni_chart[2].axisY->append("38",1200);
            oni_chart[2].axisY->append("39",1400);
            oni_chart[2].axisY->append("40",1600);
            oni_chart[2].axisY->append("41",1800);
            oni_chart[2].axisY->append("42",2000);
            oni_chart[2].axisY->append("43",2200);
            oni_chart[2].axisY->append("44",2400);
            oni_chart[2].axisY->append("45",2600);
            oni_chart[2].axisY->append("46",2800);
            oni_chart[2].axisY->append("47",3000);
            oni_chart[2].axisY->append("48",3200);
            oni_chart[2].axisY->setRange(0,3200);
            oni_chart[3].axisY->append("49",200);
            oni_chart[3].axisY->append("50",400);
            oni_chart[3].axisY->append("51",600);
            oni_chart[3].axisY->append("52",800);
            oni_chart[3].axisY->append("53",1000);
            oni_chart[3].axisY->append("54",1200);
            oni_chart[3].axisY->append("55",1400);
            oni_chart[3].axisY->append("56",1600);
            oni_chart[3].axisY->append("57",1800);
            oni_chart[3].axisY->append("58",2000);
            oni_chart[3].axisY->append("59",2200);
            oni_chart[3].axisY->append("60",2400);
            oni_chart[3].axisY->append("61",2600);
            oni_chart[3].axisY->append("62",2800);
            oni_chart[3].axisY->append("63",3000);
            oni_chart[3].axisY->append("64",3200);
            oni_chart[3].axisY->setRange(0,3200);
            //oni_frequencyChart.axisY->setRange(-100,100);
            break;
        case 2:
            oni_chart[0].axisY->append("01",400);
            oni_chart[0].axisY->append("02",800);
            oni_chart[0].axisY->append("03",1200);
            oni_chart[0].axisY->append("04",1600);
            oni_chart[0].axisY->append("05",2000);
            oni_chart[0].axisY->append("06",2400);
            oni_chart[0].axisY->append("07",2800);
            oni_chart[0].axisY->append("08",3200);
            oni_chart[0].axisY->append("09",3600);
            oni_chart[0].axisY->append("10",4000);
            oni_chart[0].axisY->append("11",4400);
            oni_chart[0].axisY->append("12",4800);
            oni_chart[0].axisY->append("13",5200);
            oni_chart[0].axisY->append("14",5600);
            oni_chart[0].axisY->append("15",6000);
            oni_chart[0].axisY->append("16",6400);
            oni_chart[0].axisY->setRange(0,6400);
            oni_chart[1].axisY->append("17",400);
            oni_chart[1].axisY->append("18",800);
            oni_chart[1].axisY->append("19",1200);
            oni_chart[1].axisY->append("20",1600);
            oni_chart[1].axisY->append("21",2000);
            oni_chart[1].axisY->append("22",2400);
            oni_chart[1].axisY->append("23",2800);
            oni_chart[1].axisY->append("24",3200);
            oni_chart[1].axisY->append("25",3600);
            oni_chart[1].axisY->append("26",4000);
            oni_chart[1].axisY->append("27",4400);
            oni_chart[1].axisY->append("28",4800);
            oni_chart[1].axisY->append("29",5200);
            oni_chart[1].axisY->append("30",5600);
            oni_chart[1].axisY->append("31",6000);
            oni_chart[1].axisY->append("32",6400);
            oni_chart[1].axisY->setRange(0,6400);
            oni_chart[2].axisY->append("33",400);
            oni_chart[2].axisY->append("34",800);
            oni_chart[2].axisY->append("35",1200);
            oni_chart[2].axisY->append("36",1600);
            oni_chart[2].axisY->append("37",2000);
            oni_chart[2].axisY->append("38",2400);
            oni_chart[2].axisY->append("39",2800);
            oni_chart[2].axisY->append("40",3200);
            oni_chart[2].axisY->append("41",3600);
            oni_chart[2].axisY->append("42",4000);
            oni_chart[2].axisY->append("43",4400);
            oni_chart[2].axisY->append("44",4800);
            oni_chart[2].axisY->append("45",5200);
            oni_chart[2].axisY->append("46",5600);
            oni_chart[2].axisY->append("47",6000);
            oni_chart[2].axisY->append("48",6400);
            oni_chart[2].axisY->setRange(0,6400);
            oni_chart[3].axisY->append("49",400);
            oni_chart[3].axisY->append("50",800);
            oni_chart[3].axisY->append("51",1200);
            oni_chart[3].axisY->append("52",1600);
            oni_chart[3].axisY->append("53",2000);
            oni_chart[3].axisY->append("54",2400);
            oni_chart[3].axisY->append("55",2800);
            oni_chart[3].axisY->append("56",3200);
            oni_chart[3].axisY->append("57",3600);
            oni_chart[3].axisY->append("58",4000);
            oni_chart[3].axisY->append("59",4400);
            oni_chart[3].axisY->append("60",4800);
            oni_chart[3].axisY->append("61",5200);
            oni_chart[3].axisY->append("62",5600);
            oni_chart[3].axisY->append("63",6000);
            oni_chart[3].axisY->append("64",6400);
            oni_chart[3].axisY->setRange(0,6400);
           // oni_frequencyChart.axisY->setRange(-200,200);
            break;
        case 3:
            oni_chart[0].axisY->append("01",800);
            oni_chart[0].axisY->append("02",1600);
            oni_chart[0].axisY->append("03",2400);
            oni_chart[0].axisY->append("04",3200);
            oni_chart[0].axisY->append("05",4000);
            oni_chart[0].axisY->append("06",4800);
            oni_chart[0].axisY->append("07",5600);
            oni_chart[0].axisY->append("08",6400);
            oni_chart[0].axisY->append("09",7200);
            oni_chart[0].axisY->append("10",8000);
            oni_chart[0].axisY->append("11",8800);
            oni_chart[0].axisY->append("12",9600);
            oni_chart[0].axisY->append("13",10400);
            oni_chart[0].axisY->append("14",11200);
            oni_chart[0].axisY->append("15",12000);
            oni_chart[0].axisY->append("16",12800);
            oni_chart[0].axisY->setRange(0,12800);
            oni_chart[1].axisY->append("17",800);
            oni_chart[1].axisY->append("18",1600);
            oni_chart[1].axisY->append("19",2400);
            oni_chart[1].axisY->append("20",3200);
            oni_chart[1].axisY->append("21",4000);
            oni_chart[1].axisY->append("22",4800);
            oni_chart[1].axisY->append("23",5600);
            oni_chart[1].axisY->append("24",6400);
            oni_chart[1].axisY->append("25",7200);
            oni_chart[1].axisY->append("26",8000);
            oni_chart[1].axisY->append("27",8800);
            oni_chart[1].axisY->append("28",9600);
            oni_chart[1].axisY->append("29",10400);
            oni_chart[1].axisY->append("30",11200);
            oni_chart[1].axisY->append("31",12000);
            oni_chart[1].axisY->append("32",12800);
            oni_chart[1].axisY->setRange(0,12800);
            oni_chart[2].axisY->append("33",800);
            oni_chart[2].axisY->append("34",1600);
            oni_chart[2].axisY->append("35",2400);
            oni_chart[2].axisY->append("36",3200);
            oni_chart[2].axisY->append("37",4000);
            oni_chart[2].axisY->append("38",4800);
            oni_chart[2].axisY->append("39",5600);
            oni_chart[2].axisY->append("40",6400);
            oni_chart[2].axisY->append("41",7200);
            oni_chart[2].axisY->append("42",8000);
            oni_chart[2].axisY->append("43",8800);
            oni_chart[2].axisY->append("44",9600);
            oni_chart[2].axisY->append("45",10400);
            oni_chart[2].axisY->append("46",11200);
            oni_chart[2].axisY->append("47",12000);
            oni_chart[2].axisY->append("48",12800);
            oni_chart[2].axisY->setRange(0,12800);
            oni_chart[3].axisY->append("49",800);
            oni_chart[3].axisY->append("50",1600);
            oni_chart[3].axisY->append("51",2400);
            oni_chart[3].axisY->append("52",3200);
            oni_chart[3].axisY->append("53",4000);
            oni_chart[3].axisY->append("54",4800);
            oni_chart[3].axisY->append("55",5600);
            oni_chart[3].axisY->append("56",6400);
            oni_chart[3].axisY->append("57",7200);
            oni_chart[3].axisY->append("58",8000);
            oni_chart[3].axisY->append("59",8800);
            oni_chart[3].axisY->append("60",9600);
            oni_chart[3].axisY->append("61",10400);
            oni_chart[3].axisY->append("62",11200);
            oni_chart[3].axisY->append("63",12000);
            oni_chart[3].axisY->append("64",12800);
            oni_chart[3].axisY->setRange(0,12800);
            //oni_frequencyChart.axisY->setRange(-400,400);
            break;
        case 4:
            oni_chart[0].axisY->append("01",2000);
            oni_chart[0].axisY->append("02",4000);
            oni_chart[0].axisY->append("03",6000);
            oni_chart[0].axisY->append("04",8000);
            oni_chart[0].axisY->append("05",10000);
            oni_chart[0].axisY->append("06",12000);
            oni_chart[0].axisY->append("07",14000);
            oni_chart[0].axisY->append("08",16000);
            oni_chart[0].axisY->append("09",18000);
            oni_chart[0].axisY->append("10",20000);
            oni_chart[0].axisY->append("11",22000);
            oni_chart[0].axisY->append("12",24000);
            oni_chart[0].axisY->append("13",26000);
            oni_chart[0].axisY->append("14",28000);
            oni_chart[0].axisY->append("15",30000);
            oni_chart[0].axisY->append("16",32000);
            oni_chart[0].axisY->setRange(0,32000);
            oni_chart[1].axisY->append("17",2000);
            oni_chart[1].axisY->append("18",4000);
            oni_chart[1].axisY->append("19",6000);
            oni_chart[1].axisY->append("20",8000);
            oni_chart[1].axisY->append("21",10000);
            oni_chart[1].axisY->append("22",12000);
            oni_chart[1].axisY->append("23",14000);
            oni_chart[1].axisY->append("24",16000);
            oni_chart[1].axisY->append("25",18000);
            oni_chart[1].axisY->append("26",20000);
            oni_chart[1].axisY->append("27",22000);
            oni_chart[1].axisY->append("28",24000);
            oni_chart[1].axisY->append("29",26000);
            oni_chart[1].axisY->append("30",28000);
            oni_chart[1].axisY->append("31",30000);
            oni_chart[1].axisY->append("32",32000);
            oni_chart[1].axisY->setRange(0,32000);
            oni_chart[2].axisY->append("33",2000);
            oni_chart[2].axisY->append("34",4000);
            oni_chart[2].axisY->append("35",6000);
            oni_chart[2].axisY->append("36",8000);
            oni_chart[2].axisY->append("37",10000);
            oni_chart[2].axisY->append("38",12000);
            oni_chart[2].axisY->append("39",14000);
            oni_chart[2].axisY->append("40",16000);
            oni_chart[2].axisY->append("41",18000);
            oni_chart[2].axisY->append("42",20000);
            oni_chart[2].axisY->append("43",22000);
            oni_chart[2].axisY->append("44",24000);
            oni_chart[2].axisY->append("45",26000);
            oni_chart[2].axisY->append("46",28000);
            oni_chart[2].axisY->append("47",30000);
            oni_chart[2].axisY->append("48",32000);
            oni_chart[2].axisY->setRange(0,32000);
            oni_chart[3].axisY->append("49",2000);
            oni_chart[3].axisY->append("50",4000);
            oni_chart[3].axisY->append("51",6000);
            oni_chart[3].axisY->append("52",8000);
            oni_chart[3].axisY->append("53",10000);
            oni_chart[3].axisY->append("54",12000);
            oni_chart[3].axisY->append("55",14000);
            oni_chart[3].axisY->append("56",16000);
            oni_chart[3].axisY->append("57",18000);
            oni_chart[3].axisY->append("58",20000);
            oni_chart[3].axisY->append("59",22000);
            oni_chart[3].axisY->append("60",24000);
            oni_chart[3].axisY->append("61",26000);
            oni_chart[3].axisY->append("62",28000);
            oni_chart[3].axisY->append("63",30000);
            oni_chart[3].axisY->append("64",32000);
            oni_chart[3].axisY->setRange(0,32000);
          //  oni_frequencyChart.axisY->setRange(-1000,1000);
            break;
        case 5:
            oni_chart[0].axisY->append("01",20000);
            oni_chart[0].axisY->append("02",40000);
            oni_chart[0].axisY->append("03",60000);
            oni_chart[0].axisY->append("04",80000);
            oni_chart[0].axisY->append("05",100000);
            oni_chart[0].axisY->append("06",120000);
            oni_chart[0].axisY->append("07",140000);
            oni_chart[0].axisY->append("08",160000);
            oni_chart[0].axisY->append("09",180000);
            oni_chart[0].axisY->append("10",200000);
            oni_chart[0].axisY->append("11",220000);
            oni_chart[0].axisY->append("12",240000);
            oni_chart[0].axisY->append("13",260000);
            oni_chart[0].axisY->append("14",280000);
            oni_chart[0].axisY->append("15",300000);
            oni_chart[0].axisY->append("16",320000);
            oni_chart[0].axisY->setRange(0,320000);
            oni_chart[1].axisY->append("17",20000);
            oni_chart[1].axisY->append("18",40000);
            oni_chart[1].axisY->append("19",60000);
            oni_chart[1].axisY->append("20",80000);
            oni_chart[1].axisY->append("21",100000);
            oni_chart[1].axisY->append("22",120000);
            oni_chart[1].axisY->append("23",140000);
            oni_chart[1].axisY->append("24",160000);
            oni_chart[1].axisY->append("25",180000);
            oni_chart[1].axisY->append("26",200000);
            oni_chart[1].axisY->append("27",220000);
            oni_chart[1].axisY->append("28",240000);
            oni_chart[1].axisY->append("29",260000);
            oni_chart[1].axisY->append("30",280000);
            oni_chart[1].axisY->append("31",300000);
            oni_chart[1].axisY->append("32",320000);
            oni_chart[1].axisY->setRange(0,320000);
            oni_chart[2].axisY->append("33",20000);
            oni_chart[2].axisY->append("34",40000);
            oni_chart[2].axisY->append("35",60000);
            oni_chart[2].axisY->append("36",80000);
            oni_chart[2].axisY->append("37",100000);
            oni_chart[2].axisY->append("38",120000);
            oni_chart[2].axisY->append("39",140000);
            oni_chart[2].axisY->append("40",160000);
            oni_chart[2].axisY->append("41",180000);
            oni_chart[2].axisY->append("42",200000);
            oni_chart[2].axisY->append("43",220000);
            oni_chart[2].axisY->append("44",240000);
            oni_chart[2].axisY->append("45",260000);
            oni_chart[2].axisY->append("46",280000);
            oni_chart[2].axisY->append("47",300000);
            oni_chart[2].axisY->append("48",320000);
            oni_chart[2].axisY->setRange(0,320000);
            oni_chart[3].axisY->append("49",20000);
            oni_chart[3].axisY->append("50",40000);
            oni_chart[3].axisY->append("51",60000);
            oni_chart[3].axisY->append("52",80000);
            oni_chart[3].axisY->append("53",100000);
            oni_chart[3].axisY->append("54",120000);
            oni_chart[3].axisY->append("55",140000);
            oni_chart[3].axisY->append("56",160000);
            oni_chart[3].axisY->append("57",180000);
            oni_chart[3].axisY->append("58",200000);
            oni_chart[3].axisY->append("59",220000);
            oni_chart[3].axisY->append("60",240000);
            oni_chart[3].axisY->append("61",260000);
            oni_chart[3].axisY->append("62",280000);
            oni_chart[3].axisY->append("63",300000);
            oni_chart[3].axisY->append("64",320000);
            oni_chart[3].axisY->setRange(0,320000);
           // oni_frequencyChart.axisY->setRange(-10000,10000);
            break;
    default:

        break;
    }

    QMutexLocker locker(&(oni_realData.mutex));
    QVector<QPointF> tempV;
    int channel = 0;
    int tempI;
    int secondRangeBase;
    for(; channel < DATA_CHANNEL_COUNT; channel++){
        secondRangeBase = channel%16;
        tempV = oni_realData.data[channel]->pointsVector();
        for(tempI = 0; tempI < tempV.count(); tempI++)
            tempV[tempI].setY(tempV[tempI].y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][secondRangeBase] + INTERFACE_RANGE_BASE_VALUE[index][secondRangeBase]);
        oni_realData.data[channel]->replace(tempV);
    }
    oni_rangeBase = index;
}
void OnlineInterface::ChannelChanged(int index){
    switch(index){
        case 0:
            oni_chart[0].view->setVisible(true);
            oni_chart[1].view->setVisible(true);
            oni_chart[2].view->setVisible(true);
            oni_chart[3].view->setVisible(true);
            break;
        case 1:
            oni_chart[0].view->setVisible(true);
            oni_chart[1].view->setVisible(false);
            oni_chart[2].view->setVisible(false);
            oni_chart[3].view->setVisible(false);
            break;
        case 2:
            oni_chart[0].view->setVisible(false);
            oni_chart[1].view->setVisible(true);
            oni_chart[2].view->setVisible(false);
            oni_chart[3].view->setVisible(false);
            break;
        case 3:
            oni_chart[0].view->setVisible(false);
            oni_chart[1].view->setVisible(false);
            oni_chart[2].view->setVisible(true);
            oni_chart[3].view->setVisible(false);
            break;
        case 4:
            oni_chart[0].view->setVisible(false);
            oni_chart[1].view->setVisible(false);
            oni_chart[2].view->setVisible(false);
            oni_chart[3].view->setVisible(true);
            break;
        default:

            break;
    }
}

void OnlineInterface::ResetData(){
    QMutexLocker locker(&(oni_realData.mutex));
    QFile file;
    //电压
    file.setFileName(oni_dataFile.basePath+oni_dataFile.guid+"/"+FILE_VOLTAGE_NAME);
    file.open(QFile::ReadOnly|QFile::Text);
    if(file.isOpen()){
        oni_voltage->SetValue(file.readAll());
        file.close();
    }

    //温度
    file.setFileName(oni_dataFile.basePath+oni_dataFile.guid+"/"+FILE_TEMPERATURE_NAME);
    file.open(QFile::ReadOnly|QFile::Text);
    if(file.isOpen()){
        oni_temperature->SetValue(file.readAll());
        file.close();
    }

    //日志
    oni_log->clear();
    file.setFileName(oni_dataFile.basePath+oni_dataFile.guid+"/"+FILE_LOG_NAME);
    file.open(QFile::ReadOnly|QFile::Text);
    if(file.isOpen()){
        oni_log->append(QString::fromLocal8Bit(file.readAll()));
        file.close();
    }
    oni_downFilePathEdit->clear();

    oni_realData.data[0]->clear();
    oni_realData.data[1]->clear();
    oni_realData.data[2]->clear();
    oni_realData.data[3]->clear();
    oni_realData.data[4]->clear();
    oni_realData.data[5]->clear();
    oni_realData.data[6]->clear();
    oni_realData.data[7]->clear();
    oni_realData.data[8]->clear();
    oni_realData.data[9]->clear();
    oni_realData.data[10]->clear();
    oni_realData.data[11]->clear();
    oni_realData.data[12]->clear();
    oni_realData.data[13]->clear();
    oni_realData.data[14]->clear();
    oni_realData.data[15]->clear();
    oni_realData.data[16]->clear();
    oni_realData.data[17]->clear();
    oni_realData.data[18]->clear();
    oni_realData.data[19]->clear();
    oni_realData.data[20]->clear();
    oni_realData.data[21]->clear();
    oni_realData.data[22]->clear();
    oni_realData.data[23]->clear();
    oni_realData.data[24]->clear();
    oni_realData.data[25]->clear();
    oni_realData.data[26]->clear();
    oni_realData.data[27]->clear();
    oni_realData.data[28]->clear();
    oni_realData.data[29]->clear();
    oni_realData.data[30]->clear();
    oni_realData.data[31]->clear();
    oni_realData.data[32]->clear();
    oni_realData.data[33]->clear();
    oni_realData.data[34]->clear();
    oni_realData.data[35]->clear();
    oni_realData.data[36]->clear();
    oni_realData.data[37]->clear();
    oni_realData.data[38]->clear();
    oni_realData.data[39]->clear();
    oni_realData.data[40]->clear();
    oni_realData.data[41]->clear();
    oni_realData.data[42]->clear();
    oni_realData.data[43]->clear();
    oni_realData.data[44]->clear();
    oni_realData.data[45]->clear();
    oni_realData.data[46]->clear();
    oni_realData.data[47]->clear();
    oni_realData.data[48]->clear();
    oni_realData.data[49]->clear();
    oni_realData.data[50]->clear();
    oni_realData.data[51]->clear();
    oni_realData.data[52]->clear();
    oni_realData.data[53]->clear();
    oni_realData.data[54]->clear();
    oni_realData.data[55]->clear();
    oni_realData.data[56]->clear();
    oni_realData.data[57]->clear();
    oni_realData.data[58]->clear();
    oni_realData.data[59]->clear();
    oni_realData.data[60]->clear();
    oni_realData.data[61]->clear();
    oni_realData.data[62]->clear();
    oni_realData.data[63]->clear();

    oni_realData.curEndX = 0;

    if(oni_dataFile.file.isOpen())
        oni_dataFile.file.close();

    oni_dataFile.fileSeq = 0;
    unsigned int tempSeq = 0;
    QDir dir(oni_dataFile.basePath+oni_dataFile.guid);
    if(dir.exists()){
        QFileInfoList fileList = dir.entryInfoList();
        for(QFileInfoList::iterator i = fileList.begin(); i != fileList.end(); i++){
            if(i->isDir())
                continue;
            if(i->fileName().left(FILE_FINAL_PREFIX_LEN) != FILE_FINAL_PREFIX)
                continue;
            if((i->fileName().right(FILE_TEMP_SUFFIX_LEN) == FILE_TEMP_SUFFIX))
                continue;
            tempSeq = i->fileName().mid(FILE_FINAL_PREFIX_LEN,10).toUInt();
            if(tempSeq > oni_dataFile.fileSeq)
                oni_dataFile.fileSeq = tempSeq;
        }
    }
//    TimeSlotChanged(oni_timeSlot->CurrentIndex());
//    UpdateFrequencyChart();
}


void OnlineInterface::ChartStart(int unit,int rate,QString dataDir){
   oni_dataFile.realUnitCount = unit;
   oni_dataFile.dataRate = rate;
   oni_dataFile.basePath = (dataDir.right(1) == "/" ? dataDir : (dataDir+"/"));
   oni_interfaceFlushTimer->start();
   oni_RMSAndColorCard.updateTimer->start(500);
}

void OnlineInterface::ChartStop(){
    while(oni_tab.tabBar->count())
        oni_tab.tabBar->removeTab(0);
    QMutexLocker locker(&(oni_realData.mutex));
    oni_realData.data[0]->clear();
    oni_realData.data[1]->clear();
    oni_realData.data[2]->clear();
    oni_realData.data[3]->clear();
    oni_realData.data[4]->clear();
    oni_realData.data[5]->clear();
    oni_realData.data[6]->clear();
    oni_realData.data[7]->clear();
    oni_realData.data[8]->clear();
    oni_realData.data[9]->clear();
    oni_realData.data[10]->clear();
    oni_realData.data[11]->clear();
    oni_realData.data[12]->clear();
    oni_realData.data[13]->clear();
    oni_realData.data[14]->clear();
    oni_realData.data[15]->clear();
    oni_realData.data[16]->clear();
    oni_realData.data[17]->clear();
    oni_realData.data[18]->clear();
    oni_realData.data[19]->clear();
    oni_realData.data[20]->clear();
    oni_realData.data[21]->clear();
    oni_realData.data[22]->clear();
    oni_realData.data[23]->clear();
    oni_realData.data[24]->clear();
    oni_realData.data[25]->clear();
    oni_realData.data[26]->clear();
    oni_realData.data[27]->clear();
    oni_realData.data[28]->clear();
    oni_realData.data[29]->clear();
    oni_realData.data[30]->clear();
    oni_realData.data[31]->clear();
    oni_realData.data[32]->clear();
    oni_realData.data[33]->clear();
    oni_realData.data[34]->clear();
    oni_realData.data[35]->clear();
    oni_realData.data[36]->clear();
    oni_realData.data[37]->clear();
    oni_realData.data[38]->clear();
    oni_realData.data[39]->clear();
    oni_realData.data[40]->clear();
    oni_realData.data[41]->clear();
    oni_realData.data[42]->clear();
    oni_realData.data[43]->clear();
    oni_realData.data[44]->clear();
    oni_realData.data[45]->clear();
    oni_realData.data[46]->clear();
    oni_realData.data[47]->clear();
    oni_realData.data[48]->clear();
    oni_realData.data[49]->clear();
    oni_realData.data[50]->clear();
    oni_realData.data[51]->clear();
    oni_realData.data[52]->clear();
    oni_realData.data[53]->clear();
    oni_realData.data[54]->clear();
    oni_realData.data[55]->clear();
    oni_realData.data[56]->clear();
    oni_realData.data[57]->clear();
    oni_realData.data[58]->clear();
    oni_realData.data[59]->clear();
    oni_realData.data[60]->clear();
    oni_realData.data[61]->clear();
    oni_realData.data[62]->clear();
    oni_realData.data[63]->clear();

    oni_realData.curEndX = 0;

    oni_RMSAndColorCard.updateTimer->stop();
    oni_interfaceFlushTimer->stop();
    oni_colorCard->DoReset();
    oni_chart[0].rmsGroup->DoReset();
    oni_chart[1].rmsGroup->DoReset();
    oni_chart[2].rmsGroup->DoReset();
    oni_chart[3].rmsGroup->DoReset();
}

void OnlineInterface::TabChanged(int index){
    QMutexLocker(&(oni_tab.mutex));
    if(index < 0)
        oni_dataFile.guid = "";
    else
        oni_dataFile.guid = oni_tab.tabBar->accessibleTabName(index);

    oni_colorCard->DoReset();
    oni_chart[0].rmsGroup->DoReset();
    oni_chart[1].rmsGroup->DoReset();
    oni_chart[2].rmsGroup->DoReset();
    oni_chart[3].rmsGroup->DoReset();

    ResetData();
//    oni_RMSAndColorCard.hasNewData = true;
//    UpdateRMSAndColorCard();
}


void OnlineInterface::TabClosed(int index){
    QMutexLocker(&(oni_tab.mutex));
    if(index < 0)
        return;
//    oni_tab.tabBar->removeTab(index);
    emit OrderSend(QString::number(TAB_CLOSED) +
                   " " +
                   oni_tab.tabBar->accessibleTabName(index));
   // qDebug()<< oni_tab.tabBar->accessibleTabName(oni_tab.tabBar->currentIndex());

}

//指令判决
void OnlineInterface::MessageFromCollectionServer(QString msg){
    QString operate = msg.mid(0,3);
    if(operate == QString(SLAVE_UPDATE))
        UpdateInterface(msg.mid(3));
    else if(operate == QString(SLAVE_CONNECT))
        AddInterface(msg.mid(3));
    else if(operate == QString(SLAVE_DISCONNECT))
        DeleteInterface(msg.mid(3));
    else if(operate ==QString(FILE_WRITED))
        wfile++;
    else if(operate == QString(FILTER_CHANGE))
    {
        filterChangeTemp = msg.mid(3).toInt();
        //qDebug()<<msg;
        //qDebug()<<filterChangeTemp;
    }

    else
        return;
}

void OnlineInterface::AddInterface(QString msg){
    int index = oni_tab.tabBar->addTab(QString("\n") + msg.mid(36));
    oni_tab.tabBar->setAccessibleTabName(index,msg.mid(0,36));
    oni_filter->setEnabled(true);

}
void OnlineInterface::DeleteInterface(QString msg){
    for(int index = 0; index < oni_tab.tabBar->count(); index++)
        if(oni_tab.tabBar->accessibleTabName(index) == msg){
            oni_tab.tabBar->removeTab(index);
            oni_filter->setEnabled(false);
            oni_filter->setIndex(0);
            return;
        }
}
void OnlineInterface::UpdateInterface(QString msg){
    QString guid = msg.mid(3,36);
    QString operate = msg.mid(0,3);
    if(operate == QString(SLAVE_UPD_TEP)){
        QMutexLocker locker(&oni_tab.mutex);
        if(oni_tab.tabBar->currentIndex() < 0 ||
                guid != oni_tab.tabBar->accessibleTabName(oni_tab.tabBar->currentIndex()))
            return;
        oni_temperature->SetValue(msg.mid(39));
    }else if(operate == QString(SLAVE_UPD_VTG)){
        QMutexLocker locker(&oni_tab.mutex);
        if(oni_tab.tabBar->currentIndex() < 0 ||
                guid != oni_tab.tabBar->accessibleTabName(oni_tab.tabBar->currentIndex()))
            return;
        oni_voltage->SetValue(msg.mid(39));
    }else if(operate == QString(SLAVE_UPD_LOG)){
        QMutexLocker locker(&oni_tab.mutex);
        if(oni_tab.tabBar->currentIndex() < 0 ||
                guid != oni_tab.tabBar->accessibleTabName(oni_tab.tabBar->currentIndex()))
            return;
        oni_log->append(msg.mid(39));
    }else if(operate == QString(SLAVE_UPD_IDF)){
        QMutexLocker locker(&oni_tab.mutex);
        for(int index = 0; index < oni_tab.tabBar->count(); index++)
            if(oni_tab.tabBar->accessibleTabName(index) == guid){
                QString curText = oni_tab.tabBar->tabText(index);
                curText = msg.mid(39) + curText.mid(curText.indexOf('\n'));
                oni_tab.tabBar->setTabText(index,curText);
                return;
            }
    }else
        return;
}

void OnlineInterface::CollectionStart(){
    QMutexLocker locker(&oni_tab.mutex);
    if(oni_tab.tabBar->currentIndex() < 0){
        Inlog("无有效下位机接入");
        return;
    }
    Inlog("开始采集");
    emit OrderSend(QString::number(COLLECTION_START) +
                   " " +
                   oni_tab.tabBar->accessibleTabName(oni_tab.tabBar->currentIndex()));
}
void OnlineInterface::CollectionStop(){
    QMutexLocker locker(&oni_tab.mutex);
    if(oni_tab.tabBar->currentIndex() < 0){
        Inlog("无有效下位机接入");
        return;
    }
    Inlog("停止采集");
    emit OrderSend(QString::number(COLLECTION_STOP) +
                   " " +
                   oni_tab.tabBar->accessibleTabName(oni_tab.tabBar->currentIndex()));
}
void OnlineInterface::DoReset(){
    QMutexLocker locker(&oni_tab.mutex);
    if(oni_tab.tabBar->currentIndex() < 0){
        Inlog("无有效下位机接入");
        return;
    }
    Inlog("执行复位操作");
    emit OrderSend(QString::number(RESET_OPERATE) +
                   " " +
                   oni_tab.tabBar->accessibleTabName(oni_tab.tabBar->currentIndex()));
}
void OnlineInterface::StimulateStart(){
    QMutexLocker locker(&oni_tab.mutex);
    if(oni_tab.tabBar->currentIndex() < 0){
        Inlog("无有效下位机接入");
        return;
    }
    Inlog("开始刺激");
    emit OrderSend(QString::number(STIMULATE_STAET) +
                   " " +
                   oni_tab.tabBar->accessibleTabName(oni_tab.tabBar->currentIndex()));
}
void OnlineInterface::StimulateStop(){
    QMutexLocker locker(&oni_tab.mutex);
    if(oni_tab.tabBar->currentIndex() < 0){
        Inlog("无有效下位机接入");
        return;
    }
    Inlog("停止刺激");
    emit OrderSend(QString::number(STIMULATE_STOP) +
                   " " +
                   oni_tab.tabBar->accessibleTabName(oni_tab.tabBar->currentIndex()));
}
void OnlineInterface::DownDataText(){
    QMutexLocker locker(&oni_tab.mutex);
    if(oni_tab.tabBar->currentIndex() < 0){
        Inlog("无有效下位机接入");
        return;
    }
    if(oni_downDataEidt->toPlainText().size() == 0){
        Inlog("下发内容为空");
        return;
    }
    Inlog("下发文本数据");
    if(oni_downDataFormat->isChecked())
        emit OrderSend(QString::number(DOWN_DATA_TEXT) +
                       " " +
                       oni_tab.tabBar->accessibleTabName(oni_tab.tabBar->currentIndex()) +
                       oni_downDataEidt->toPlainText().toLatin1());
    else
        emit OrderSend(QString::number(DOWN_DATA_TEXT) +
                       " " +
                       oni_tab.tabBar->accessibleTabName(oni_tab.tabBar->currentIndex()) +
                       oni_downDataEidt->toPlainText().toLatin1().toHex());
}
void OnlineInterface::DownDataFile(){
    QMutexLocker locker(&oni_tab.mutex);
    if(oni_tab.tabBar->currentIndex() < 0){
        Inlog("无有效下位机接入");
        return;
    }
    if(oni_downFilePathEdit->text().size() == 0){
        Inlog("未指定下发文件");
        return;
    }
    Inlog("下发文件数据");
    emit OrderSend(QString::number(DOWN_DATA_FILE) +
                    " " +
                   oni_tab.tabBar->accessibleTabName(oni_tab.tabBar->currentIndex()) +
                   QTextCodec::codecForName("utf8")->fromUnicode(oni_downFilePathEdit->text()));
}

void OnlineInterface::ChoiceDownDataPath(){
    QString path = QFileDialog::
            getOpenFileName(this, tr("查找文件"),".",tr("*.*"));
    if(path.size() > 0)
        oni_downFilePathEdit->setText(path);
}

void OnlineInterface::UpdateRMSAndColorCard(){
    QMutexLocker locker(&(oni_realData.mutex));
    if(!oni_RMSAndColorCard.hasNewData)
        return;
    oni_RMSAndColorCard.hasNewData = false;
    oni_RMSAndColorCard.pos = oni_realData.data[0]->count() - 1;
    if(oni_RMSAndColorCard.pos < 0)
        return;
    oni_RMSAndColorCard.valueList[0] = oni_realData.data[0]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][0];
    oni_RMSAndColorCard.valueList[1] = oni_realData.data[1]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][1];
    oni_RMSAndColorCard.valueList[2] = oni_realData.data[2]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][2];
    oni_RMSAndColorCard.valueList[3] = oni_realData.data[3]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][3];
    oni_RMSAndColorCard.valueList[4] = oni_realData.data[4]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][4];
    oni_RMSAndColorCard.valueList[5] = oni_realData.data[5]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][5];
    oni_RMSAndColorCard.valueList[6] = oni_realData.data[6]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][6];
    oni_RMSAndColorCard.valueList[7] = oni_realData.data[7]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][7];
    oni_RMSAndColorCard.valueList[8] = oni_realData.data[8]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][8];
    oni_RMSAndColorCard.valueList[9] = oni_realData.data[9]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][9];
    oni_RMSAndColorCard.valueList[10] = oni_realData.data[10]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][10];
    oni_RMSAndColorCard.valueList[11] = oni_realData.data[11]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][11];
    oni_RMSAndColorCard.valueList[12] = oni_realData.data[12]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][12];
    oni_RMSAndColorCard.valueList[13] = oni_realData.data[13]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][13];
    oni_RMSAndColorCard.valueList[14] = oni_realData.data[14]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][14];
    oni_RMSAndColorCard.valueList[15] = oni_realData.data[15]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][15];
    oni_RMSAndColorCard.valueList[16] = oni_realData.data[16]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][0];
    oni_RMSAndColorCard.valueList[17] = oni_realData.data[17]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][1];
    oni_RMSAndColorCard.valueList[18] = oni_realData.data[18]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][2];
    oni_RMSAndColorCard.valueList[19] = oni_realData.data[19]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][3];
    oni_RMSAndColorCard.valueList[20] = oni_realData.data[20]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][4];
    oni_RMSAndColorCard.valueList[21] = oni_realData.data[21]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][5];
    oni_RMSAndColorCard.valueList[22] = oni_realData.data[22]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][6];
    oni_RMSAndColorCard.valueList[23] = oni_realData.data[23]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][7];
    oni_RMSAndColorCard.valueList[24] = oni_realData.data[24]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][8];
    oni_RMSAndColorCard.valueList[25] = oni_realData.data[25]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][9];
    oni_RMSAndColorCard.valueList[26] = oni_realData.data[26]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][10];
    oni_RMSAndColorCard.valueList[27] = oni_realData.data[27]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][11];
    oni_RMSAndColorCard.valueList[28] = oni_realData.data[28]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][12];
    oni_RMSAndColorCard.valueList[29] = oni_realData.data[29]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][13];
    oni_RMSAndColorCard.valueList[30] = oni_realData.data[30]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][14];
    oni_RMSAndColorCard.valueList[31] = oni_realData.data[31]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][15];
    oni_RMSAndColorCard.valueList[32] = oni_realData.data[32]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][0];
    oni_RMSAndColorCard.valueList[33] = oni_realData.data[33]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][1];
    oni_RMSAndColorCard.valueList[34] = oni_realData.data[34]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][2];
    oni_RMSAndColorCard.valueList[35] = oni_realData.data[35]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][3];
    oni_RMSAndColorCard.valueList[36] = oni_realData.data[36]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][4];
    oni_RMSAndColorCard.valueList[37] = oni_realData.data[37]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][5];
    oni_RMSAndColorCard.valueList[38] = oni_realData.data[38]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][6];
    oni_RMSAndColorCard.valueList[39] = oni_realData.data[39]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][7];
    oni_RMSAndColorCard.valueList[40] = oni_realData.data[40]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][8];
    oni_RMSAndColorCard.valueList[41] = oni_realData.data[41]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][9];
    oni_RMSAndColorCard.valueList[42] = oni_realData.data[42]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][10];
    oni_RMSAndColorCard.valueList[43] = oni_realData.data[43]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][11];
    oni_RMSAndColorCard.valueList[44] = oni_realData.data[44]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][12];
    oni_RMSAndColorCard.valueList[45] = oni_realData.data[45]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][13];
    oni_RMSAndColorCard.valueList[46] = oni_realData.data[46]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][14];
    oni_RMSAndColorCard.valueList[47] = oni_realData.data[47]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][15];
    oni_RMSAndColorCard.valueList[48] = oni_realData.data[48]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][0];
    oni_RMSAndColorCard.valueList[49] = oni_realData.data[49]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][1];
    oni_RMSAndColorCard.valueList[50] = oni_realData.data[50]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][2];
    oni_RMSAndColorCard.valueList[51] = oni_realData.data[51]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][3];
    oni_RMSAndColorCard.valueList[52] = oni_realData.data[52]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][4];
    oni_RMSAndColorCard.valueList[53] = oni_realData.data[53]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][5];
    oni_RMSAndColorCard.valueList[54] = oni_realData.data[54]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][6];
    oni_RMSAndColorCard.valueList[55] = oni_realData.data[55]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][7];
    oni_RMSAndColorCard.valueList[56] = oni_realData.data[56]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][8];
    oni_RMSAndColorCard.valueList[57] = oni_realData.data[57]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][9];
    oni_RMSAndColorCard.valueList[58] = oni_realData.data[58]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][10];
    oni_RMSAndColorCard.valueList[59] = oni_realData.data[59]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][11];
    oni_RMSAndColorCard.valueList[60] = oni_realData.data[60]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][12];
    oni_RMSAndColorCard.valueList[61] = oni_realData.data[61]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][13];
    oni_RMSAndColorCard.valueList[62] = oni_realData.data[62]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][14];
    oni_RMSAndColorCard.valueList[63] = oni_realData.data[63]->at(oni_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[oni_rangeBase][15];

    oni_colorCard->DoUpdate(oni_RMSAndColorCard.valueList);
    oni_chart[0].rmsGroup->DoUpdate(oni_RMSAndColorCard.valueList);
    oni_chart[1].rmsGroup->DoUpdate(oni_RMSAndColorCard.valueList+16);
    oni_chart[2].rmsGroup->DoUpdate(oni_RMSAndColorCard.valueList+32);
    oni_chart[3].rmsGroup->DoUpdate(oni_RMSAndColorCard.valueList+48);
}

void OnlineInterface::Inlog(QString msg){
    QString content = QDateTime::currentDateTime().toString("[yyyy.MM.dd-hh:mm:ss] ") + msg;
    oni_log->append(content);
    QFile file(oni_dataFile.basePath+oni_dataFile.guid+"/"+FILE_LOG_NAME);
    file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
    file.write((content+"\n").toLocal8Bit());
    file.close();
}
