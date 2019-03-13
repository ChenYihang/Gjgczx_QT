#include "offlineinterface.h"
#include "SubInterface/interfacecommon.h"
OfflineInterface *OfflineInterface::offi_offlineI = 0;
OfflineInterface::OfflineInterface(QWidget *parent) : QWidget(parent){
    offi_mainLayout = new QHBoxLayout();
    InitInterface();
    TimeSlotChanged(0);
    RangeChanged(0);
    FrequencyChanged(0);
    offi_mainLayout->setContentsMargins(0,0,0,0);
    setLayout(offi_mainLayout);
    setContentsMargins(0,0,0,0);
}
OfflineInterface::~OfflineInterface(){
    DestoryRightInterface();
    delete offi_mainLayout;
}

OfflineInterface *OfflineInterface::GetInstance(){
    if(!offi_offlineI)
        offi_offlineI = new OfflineInterface();
    return offi_offlineI;
}

void OfflineInterface::InitInterface(){
    offi_start = new QPushButton("开始");
    offi_start->setFixedSize(60,25);
    offi_start->setStyleSheet("QPushButton {border:1px groove gray;border-radius:5px;}QPushButton:pressed {background-color: #8a8c8e;border-style:inset;}");
    connect(offi_start,SIGNAL(clicked()),this,SLOT(StartUpdateData()));
    offi_suspend = new QPushButton("暂停");;
    offi_suspend->setFixedSize(60,25);
    offi_suspend->setStyleSheet("QPushButton {border:1px groove gray;border-radius:5px;}QPushButton:pressed {background-color: #8a8c8e;border-style:inset;}");
    connect(offi_suspend,SIGNAL(clicked()),this,SLOT(SuspendUpdateData()));
    offi_stop = new QPushButton("停止");;
    offi_stop->setFixedSize(60,25);
    offi_stop->setStyleSheet("QPushButton {border:1px groove gray;border-radius:5px;}QPushButton:pressed {background-color: #8a8c8e;border-style:inset;}");
    connect(offi_stop,SIGNAL(clicked()),this,SLOT(StopUpdateData()));
    offi_slave = new ComboBoxT("下位机:");
    offi_slave->setFixedSize(185,25);
    connect(offi_slave->GetBox(),SIGNAL(currentIndexChanged(int)),this,SLOT(SlaveChanged(int)));
    offi_slaveList = new QList<SLAVE_LIST>();
    offi_leftTop = new QHBoxLayout();
    offi_leftTop->setSpacing(0);
    offi_leftTop->setContentsMargins(0,0,0,0);
    offi_leftTop->addWidget(offi_start);
    offi_leftTop->addWidget(offi_suspend);
    offi_leftTop->addWidget(offi_stop);
    offi_leftTop->addSpacing(15);
    offi_leftTop->addWidget(offi_slave);

    offi_offlinePathEdit = new QLineEdit();
    offi_offlinePathEdit->setPlaceholderText("请选择离线播放目录");
    offi_offlinePathEdit->setReadOnly(true);
    offi_offlinePathChoice = new QPushButton("选择");
    offi_offlinePathChoice->setStyleSheet("border:1px groove gray;");
    offi_offlinePathChoice->setCursor(Qt::PointingHandCursor);
    offi_offlinePathChoice->setFixedSize(80, 25);
    connect(offi_offlinePathChoice,SIGNAL(clicked()),this,SLOT(ChoiceOfflinePath()));
    QMargins margins = offi_offlinePathEdit->textMargins();
    offi_offlinePathEdit->setTextMargins(margins.left(), margins.top(), offi_offlinePathChoice->width(), margins.bottom());
    offi_offlinePathLayout = new QHBoxLayout();
    offi_offlinePathLayout->addStretch();
    offi_offlinePathLayout->addWidget(offi_offlinePathChoice);
    offi_offlinePathLayout->setSpacing(0);
    offi_offlinePathLayout->setContentsMargins(0, 0, 0, 0);
    offi_offlinePathEdit->setLayout(offi_offlinePathLayout);
    offi_offlinePathEdit->setFixedSize(400,25);

    offi_offlineFilePathEdit = new QLineEdit();
    offi_offlineFilePathEdit->setPlaceholderText("请选择离线播放文件");
    offi_offlineFilePathEdit->setReadOnly(true);
    offi_offlineFilePathChoice = new QPushButton("选择");
    offi_offlineFilePathChoice->setStyleSheet("border:1px groove gray;");
    offi_offlineFilePathChoice->setCursor(Qt::PointingHandCursor);
    offi_offlineFilePathChoice->setFixedSize(80, 25);
    connect(offi_offlineFilePathChoice,SIGNAL(clicked()),this,SLOT(ChoiceOfflineFilePath()));
    QMargins marginsFile = offi_offlineFilePathEdit->textMargins();
    offi_offlineFilePathEdit->setTextMargins(marginsFile.left(), marginsFile.top(), offi_offlineFilePathChoice->width(), margins.bottom());
    offi_offlineFilePathLayout = new QHBoxLayout();
    offi_offlineFilePathLayout->addStretch();
    offi_offlineFilePathLayout->addWidget(offi_offlineFilePathChoice);
    offi_offlineFilePathLayout->setSpacing(0);
    offi_offlineFilePathLayout->setContentsMargins(0, 0, 0, 0);
    offi_offlineFilePathEdit->setLayout(offi_offlineFilePathLayout);
    offi_offlineFilePathEdit->setFixedSize(400,25);



    for(int i = 0; i < 8; i++){
        offi_frequencyChart.data[i] = new QLineSeries();
        switch (i) {
            case 0:
                offi_frequencyChart.data[i]->setName("01");
                offi_frequencyChart.data[i]->setPen(QPen(Qt::gray,0.7));
                break;
            case 1:
                offi_frequencyChart.data[i]->setName("02");
                offi_frequencyChart.data[i]->setPen(QPen(Qt::red,0.7));
                break;
            case 2:
                offi_frequencyChart.data[i]->setName("17");
                offi_frequencyChart.data[i]->setPen(QPen(Qt::green,0.7));
                break;
            case 3:
                offi_frequencyChart.data[i]->setName("18");
                offi_frequencyChart.data[i]->setPen(QPen(Qt::blue,0.7));
                break;
            case 4:
                offi_frequencyChart.data[i]->setName("33");
                offi_frequencyChart.data[i]->setPen(QPen(Qt::cyan,0.7));
                break;
            case 5:
                offi_frequencyChart.data[i]->setName("34");
                offi_frequencyChart.data[i]->setPen(QPen(Qt::magenta,0.7));
                break;
            case 6:
                offi_frequencyChart.data[i]->setName("49");
                offi_frequencyChart.data[i]->setPen(QPen(Qt::yellow,0.7));
                break;
            case 7:
                offi_frequencyChart.data[i]->setName("50");
                offi_frequencyChart.data[i]->setPen(QPen(Qt::black,0.7));
                break;
            default:
                break;
        }
        offi_frequencyChart.data[i]->setUseOpenGL(true);
        for(int dataCur = 0; dataCur <= INTERFACE_FREQUENCY_FFTW[2]; dataCur ++)
            offi_frequencyChart.data[i]->append(dataCur,0);
    }
    offi_frequencyChart.chart = new QChart();
    offi_frequencyChart.chart->setContentsMargins(0,0,0,0);
    offi_frequencyChart.chart->legend()->setAlignment(Qt::AlignBottom);
    offi_frequencyChart.chart->legend()->setContentsMargins(0,0,0,0);
    for(int i = 0; i < 8; i++)
        offi_frequencyChart.chart->addSeries(offi_frequencyChart.data[i]);

    offi_frequencyChart.axisX = new QValueAxis();
    offi_frequencyChart.axisX->setGridLineVisible(false);
    offi_frequencyChart.axisX->setTitleVisible(false);
    offi_frequencyChart.axisY = new QValueAxis();
    offi_frequencyChart.axisY->setGridLineVisible(false);
    offi_frequencyChart.axisY->setTitleVisible(false);
    offi_frequencyChart.axisY->setTickCount(11);

    offi_frequencyChart.chart->addAxis(offi_frequencyChart.axisX,Qt::AlignBottom);
    offi_frequencyChart.chart->addAxis(offi_frequencyChart.axisY,Qt::AlignLeft);

    for(int i = 0; i < 8; i++){
        offi_frequencyChart.data[i]->attachAxis(offi_frequencyChart.axisX);
        offi_frequencyChart.data[i]->attachAxis(offi_frequencyChart.axisY);
    }

    offi_frequencyChart.view = new QChartView();
    offi_frequencyChart.view->setContentsMargins(0,0,0,0);
    offi_frequencyChart.view->setFixedSize(415,400);
    offi_frequencyChart.view->setChart(offi_frequencyChart.chart);
    offi_frequencyChart.view->setRenderHint(QPainter::Antialiasing);

    offi_fftw.inData = new double[INTERFACE_FREQUENCY_FFTW[2]];
    offi_fftw.outData = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (2 * ((INTERFACE_FREQUENCY_FFTW[2] / 2) + 1)));
    offi_fftw.plan30 = fftw_plan_dft_r2c_1d(INTERFACE_FREQUENCY_FFTW[0],offi_fftw.inData,offi_fftw.outData,FFTW_EXHAUSTIVE);
    offi_fftw.plan60 = fftw_plan_dft_r2c_1d(INTERFACE_FREQUENCY_FFTW[1],offi_fftw.inData,offi_fftw.outData,FFTW_EXHAUSTIVE);
    offi_fftw.plan120 = fftw_plan_dft_r2c_1d(INTERFACE_FREQUENCY_FFTW[2],offi_fftw.inData,offi_fftw.outData,FFTW_EXHAUSTIVE);

    offi_log = new QTextEdit();
    offi_log->setFixedWidth(400);
    offi_log->setReadOnly(true);
    offi_colorCard = new ColorCard();

    offi_left = new QVBoxLayout();
    offi_left->setContentsMargins(0,0,0,0);
    offi_left->addLayout(offi_leftTop);
    offi_left->addWidget(offi_offlinePathEdit);
    offi_left->addWidget(offi_offlineFilePathEdit);
    offi_left->addWidget(offi_frequencyChart.view);
    offi_left->addLayout(offi_colorCard);
    offi_left->addWidget(offi_log);
    offi_left->setAlignment(offi_offlinePathEdit,Qt::AlignHCenter);
    offi_left->setAlignment(offi_offlineFilePathEdit,Qt::AlignHCenter);
    offi_left->setAlignment(offi_frequencyChart.view,Qt::AlignHCenter);
    offi_left->setAlignment(offi_log,Qt::AlignHCenter);
    offi_leftW = new QWidget();
    offi_leftW->setLayout(offi_left);
    offi_leftW->setContentsMargins(0,0,0,0);
    offi_leftW->setFixedWidth(415);

    offi_timeSlot = new ComboBoxT("时隙(S):");
    offi_timeSlot->AddItem("1    ");
    offi_timeSlot->AddItem("3    ");
    offi_timeSlot->AddItem("5    ");
    offi_timeSlot->AddItem("7    ");
    connect(offi_timeSlot->GetBox(),SIGNAL(currentIndexChanged(int)),this,SLOT(TimeSlotChanged(int)));
    offi_range = new ComboBoxT("幅度(UV):");
    offi_range->AddItem("50   ");
    offi_range->AddItem("100  ");
    offi_range->AddItem("200  ");
    offi_range->AddItem("400  ");
    offi_range->AddItem("1000 ");
    offi_range->AddItem("10000");
    connect(offi_range->GetBox(),SIGNAL(currentIndexChanged(int)),this,SLOT(RangeChanged(int)));
    offi_rangeBase = 0;
    offi_channel = new ComboBoxT("通道:");
    offi_channel->AddItem("all  ");
    offi_channel->AddItem("01~16");
    offi_channel->AddItem("17~32");
    offi_channel->AddItem("33~48");
    offi_channel->AddItem("49~64");
    connect(offi_channel->GetBox(),SIGNAL(currentIndexChanged(int)),this,SLOT(ChannelChanged(int)));

    offi_frequency = new ComboBoxT("频率(HZ):");
    offi_frequency->AddItem("30   ");
    offi_frequency->AddItem("60   ");
    offi_frequency->AddItem("120  ");
    connect(offi_frequency->GetBox(),SIGNAL(currentIndexChanged(int)),this,SLOT(FrequencyChanged(int)));

    offi_rightTop = new QHBoxLayout();
    offi_rightTop->setContentsMargins(0,0,0,0);
    offi_rightTop->addWidget(offi_channel);
    offi_rightTop->addWidget(offi_timeSlot);
    offi_rightTop->addWidget(offi_range);
    offi_rightTop->addWidget(offi_frequency);

    for(int i = 0; i < DATA_CHANNEL_COUNT; i++){
        offi_realData.data[i] = new QLineSeries();
        offi_realData.data[i]->setName(QString("%1").arg(i+1,2,10,QChar('0')));
        switch(i%16){
            case 0:
                offi_realData.data[i]->setPen(QPen(QColor(0XFF,0x00,0x00,255),0.7));
                break;
            case 1:
                offi_realData.data[i]->setPen(QPen(QColor(0X00,0xFF,0x00,255),0.7));
                break;
            case 2:
                offi_realData.data[i]->setPen(QPen(QColor(0X00,0x00,0xFF,255),0.7));
                break;
            case 3:
                offi_realData.data[i]->setPen(QPen(QColor(0X1C,0x1C,0x1C,255),0.7));
                break;
            case 4:
                offi_realData.data[i]->setPen(QPen(QColor(0X8B,0x00,0x8B,255),0.7));
                break;
            case 5:
                offi_realData.data[i]->setPen(QPen(QColor(0X8B,0x00,0x00,255),0.7));
                break;
            case 6:
                offi_realData.data[i]->setPen(QPen(QColor(0X00,0x8B,0x8B,255),0.7));
                break;
            case 7:
                offi_realData.data[i]->setPen(QPen(QColor(0XA9,0xA9,0xA9,255),0.7));
                break;
            case 8:
                offi_realData.data[i]->setPen(QPen(QColor(0X8A,0x2B,0xE2,255),0.7));
                break;
            case 9:
                offi_realData.data[i]->setPen(QPen(QColor(0X00,0xFF,0xFF,255),0.7));
                break;
            case 10:
                offi_realData.data[i]->setPen(QPen(QColor(0XFF,0x83,0xFA,255),0.7));
                break;
            case 11:
                offi_realData.data[i]->setPen(QPen(QColor(0XFF,0xA5,0x00,255),0.7));
                break;
            case 12:
                offi_realData.data[i]->setPen(QPen(QColor(0X00,0x64,0x00,255),0.7));
                break;
            case 13:
                offi_realData.data[i]->setPen(QPen(QColor(0XFF,0xFF,0x00,255),0.7));
                break;
            case 14:
                offi_realData.data[i]->setPen(QPen(QColor(0X2F,0x4F,0x4F,255),0.7));
                break;
            case 15:
                offi_realData.data[i]->setPen(QPen(QColor(0X00,0xBF,0xFF,255),0.7));
                break;
            default:

                break;
        }
        offi_realData.data[i]->setUseOpenGL(true);
        offi_dataFile.newData[i] = new QList<QPointF>();
    }
    offi_realData.curEndX = 0;
    offi_dataUpdateTimer = new QTimer();
    connect(offi_dataUpdateTimer,SIGNAL(timeout()),this,SLOT(UpdateData()));
    offi_RMSAndColorCard.updateTimer = new QTimer();
    connect(offi_RMSAndColorCard.updateTimer,SIGNAL(timeout()),this,SLOT(UpdateRMSAndColorCard()));

    for(int i = 0; i < 4; i++){
        offi_chart[i].chart = new QChart();
        offi_chart[i].chart->setContentsMargins(0,0,0,0);
        offi_chart[i].chart->legend()->setAlignment(Qt::AlignBottom);
        offi_chart[i].chart->legend()->setContentsMargins(0,0,0,0);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+0]);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+1]);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+2]);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+3]);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+4]);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+5]);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+6]);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+7]);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+8]);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+9]);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+10]);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+11]);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+12]);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+13]);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+14]);
        offi_chart[i].chart->addSeries(offi_realData.data[i*16+15]);

        offi_chart[i].axisXShow = new QValueAxis();
        offi_chart[i].axisXShow->setTitleVisible(false);
        offi_chart[i].axisXShow->setGridLineVisible(false);

        offi_chart[i].axisXValue = new QValueAxis();
        offi_chart[i].axisXValue->setVisible(false);
        offi_chart[i].axisXValue->setTitleVisible(false);
        offi_chart[i].axisXValue->setGridLineVisible(false);
        offi_chart[i].axisXValue->setMinorGridLineVisible(false);
        offi_chart[i].axisXValue->setShadesVisible(false);
        offi_chart[i].axisXValue->setLineVisible(false);
        offi_chart[i].axisXValue->setLabelsVisible(false);
        offi_chart[i].axisXValue->setGridLineVisible(false);

        offi_chart[i].axisY = new QCategoryAxis();
        offi_chart[i].axisY->setTitleVisible(false);

        offi_chart[i].chart->addAxis(offi_chart[i].axisXShow,Qt::AlignBottom);
        offi_chart[i].chart->addAxis(offi_chart[i].axisXValue,Qt::AlignTop);
        offi_chart[i].chart->addAxis(offi_chart[i].axisY,Qt::AlignLeft);

        offi_realData.data[i*16+0]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+0]->attachAxis(offi_chart[i].axisY);
        offi_realData.data[i*16+1]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+1]->attachAxis(offi_chart[i].axisY);
        offi_realData.data[i*16+2]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+2]->attachAxis(offi_chart[i].axisY);
        offi_realData.data[i*16+3]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+3]->attachAxis(offi_chart[i].axisY);
        offi_realData.data[i*16+4]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+4]->attachAxis(offi_chart[i].axisY);
        offi_realData.data[i*16+5]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+5]->attachAxis(offi_chart[i].axisY);
        offi_realData.data[i*16+6]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+6]->attachAxis(offi_chart[i].axisY);
        offi_realData.data[i*16+7]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+7]->attachAxis(offi_chart[i].axisY);
        offi_realData.data[i*16+8]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+8]->attachAxis(offi_chart[i].axisY);
        offi_realData.data[i*16+9]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+9]->attachAxis(offi_chart[i].axisY);
        offi_realData.data[i*16+10]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+10]->attachAxis(offi_chart[i].axisY);
        offi_realData.data[i*16+11]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+11]->attachAxis(offi_chart[i].axisY);
        offi_realData.data[i*16+12]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+12]->attachAxis(offi_chart[i].axisY);
        offi_realData.data[i*16+13]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+13]->attachAxis(offi_chart[i].axisY);
        offi_realData.data[i*16+14]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+14]->attachAxis(offi_chart[i].axisY);
        offi_realData.data[i*16+15]->attachAxis(offi_chart[i].axisXValue);
        offi_realData.data[i*16+15]->attachAxis(offi_chart[i].axisY);

        offi_chart[i].view = new QChartView();
        offi_chart[i].view->setChart(offi_chart[i].chart);
        offi_chart[i].view->setRenderHint(QPainter::Antialiasing);
        offi_chart[i].view->setContentsMargins(0,0,0,0);
        offi_chart[i].rmsGroup = new RMSGroup(offi_chart[i].view);
    }

    offi_splitH01to32 = new QSplitter(Qt::Horizontal);
    offi_splitH01to32->setHandleWidth(1);
    offi_splitH01to32->addWidget(offi_chart[0].view);
    offi_splitH01to32->addWidget(offi_chart[1].view);
    offi_splitH01to32->setContentsMargins(0,0,0,0);
    offi_splitH33to64 = new QSplitter(Qt::Horizontal);
    offi_splitH33to64->setHandleWidth(1);
    offi_splitH33to64->addWidget(offi_chart[2].view);
    offi_splitH33to64->addWidget(offi_chart[3].view);
    offi_splitH33to64->setContentsMargins(0,0,0,0);
    offi_splitV = new QSplitter(Qt::Vertical);
    offi_splitV->setHandleWidth(1);
    offi_splitV->setStretchFactor(0,1);
    offi_splitV->setStretchFactor(1,1);
    offi_splitV->addWidget(offi_splitH01to32);
    offi_splitV->addWidget(offi_splitH33to64);
    offi_splitV->setContentsMargins(0,0,0,0);

    offi_right = new QVBoxLayout();
    offi_right->setContentsMargins(0,0,0,0);
    offi_right->addLayout(offi_rightTop);
    offi_right->addWidget(offi_splitV);

    offi_mainLayout->addWidget(offi_leftW);
    offi_mainLayout->addLayout(offi_right);
}

void OfflineInterface::DestoryRightInterface(){
    StopUpdateData();
    delete offi_dataUpdateTimer;
    delete offi_RMSAndColorCard.updateTimer;
    delete offi_start;
    delete offi_stop;
    delete offi_suspend;
    delete offi_slave;
    delete offi_offlinePathChoice;
    delete offi_offlinePathLayout;
    delete offi_offlinePathEdit;
    delete offi_offlineFilePathChoice;
    delete offi_offlineFilePathLayout;
    delete offi_offlineFilePathEdit;
    delete offi_frequencyChart.chart;
    delete offi_frequencyChart.view;
    delete offi_frequencyChart.axisX;
    delete offi_frequencyChart.axisY;
    for(int i = 0; i < 8; i++)
        delete offi_frequencyChart.data[i];

    fftw_destroy_plan(offi_fftw.plan30);
    fftw_destroy_plan(offi_fftw.plan60);
    fftw_destroy_plan(offi_fftw.plan120);
    delete [] offi_fftw.inData;
    fftw_free(offi_fftw.outData);


    delete offi_leftTop;
    delete offi_left;
    delete offi_leftW;

    delete offi_timeSlot;
    delete offi_range;
    delete offi_channel;
    delete offi_frequency;

    for(int i = 0; i < 4; i++){
        delete offi_chart[i].rmsGroup;
        delete offi_chart[i].chart;
        delete offi_chart[i].axisY;
        delete offi_chart[i].axisXValue;
        delete offi_chart[i].axisXShow;
        delete offi_chart[i].view;
    }
    for(int i = 0; i < DATA_CHANNEL_COUNT; i++){
        delete offi_realData.data[i];
        delete offi_dataFile.newData[i];
    }
    delete offi_splitH01to32;
    delete offi_splitH33to64;
    delete offi_splitV;
    delete offi_rightTop;
    delete offi_right;
}

void OfflineInterface::TimeSlotChanged(int index){
    RecordLog("更新时域图数据量为"+QString::number(DATA_CACHE_MAX[index]));
    switch(index){
        case 0:
            offi_chart[0].axisXShow->setRange(-1,0);
            offi_chart[0].axisXShow->setTickCount(11);
            offi_chart[1].axisXShow->setRange(-1,0);
            offi_chart[1].axisXShow->setTickCount(11);
            offi_chart[2].axisXShow->setRange(-1,0);
            offi_chart[2].axisXShow->setTickCount(11);
            offi_chart[3].axisXShow->setRange(-1,0);
            offi_chart[3].axisXShow->setTickCount(11);
            break;
        case 1:
            offi_chart[0].axisXShow->setRange(-3,0);
            offi_chart[0].axisXShow->setTickCount(4);
            offi_chart[1].axisXShow->setRange(-3,0);
            offi_chart[1].axisXShow->setTickCount(4);
            offi_chart[2].axisXShow->setRange(-3,0);
            offi_chart[2].axisXShow->setTickCount(4);
            offi_chart[3].axisXShow->setRange(-3,0);
            offi_chart[3].axisXShow->setTickCount(4);
            break;
        case 2:
            offi_chart[0].axisXShow->setRange(-5,0);
            offi_chart[0].axisXShow->setTickCount(6);
            offi_chart[1].axisXShow->setRange(-5,0);
            offi_chart[1].axisXShow->setTickCount(6);
            offi_chart[2].axisXShow->setRange(-5,0);
            offi_chart[2].axisXShow->setTickCount(6);
            offi_chart[3].axisXShow->setRange(-5,0);
            offi_chart[3].axisXShow->setTickCount(6);
            break;
        case 3:
            offi_chart[0].axisXShow->setRange(-7,0);
            offi_chart[0].axisXShow->setTickCount(8);
            offi_chart[1].axisXShow->setRange(-7,0);
            offi_chart[1].axisXShow->setTickCount(8);
            offi_chart[2].axisXShow->setRange(-7,0);
            offi_chart[2].axisXShow->setTickCount(8);
            offi_chart[3].axisXShow->setRange(-7,0);
            offi_chart[3].axisXShow->setTickCount(8);
            break;
        default:
            return;
            break;
    }
    qreal curStartX = offi_realData.curEndX - DATA_CACHE_MAX[offi_timeSlot->CurrentIndex()];
    offi_chart[0].axisXValue->setRange(curStartX,offi_realData.curEndX);
    offi_chart[1].axisXValue->setRange(curStartX,offi_realData.curEndX);
    offi_chart[2].axisXValue->setRange(curStartX,offi_realData.curEndX);
    offi_chart[3].axisXValue->setRange(curStartX,offi_realData.curEndX);
}
void OfflineInterface::RangeChanged(int index){
    offi_chart[0].axisY->remove("01");
    offi_chart[0].axisY->remove("02");
    offi_chart[0].axisY->remove("03");
    offi_chart[0].axisY->remove("04");
    offi_chart[0].axisY->remove("05");
    offi_chart[0].axisY->remove("06");
    offi_chart[0].axisY->remove("07");
    offi_chart[0].axisY->remove("08");
    offi_chart[0].axisY->remove("09");
    offi_chart[0].axisY->remove("10");
    offi_chart[0].axisY->remove("11");
    offi_chart[0].axisY->remove("12");
    offi_chart[0].axisY->remove("13");
    offi_chart[0].axisY->remove("14");
    offi_chart[0].axisY->remove("15");
    offi_chart[0].axisY->remove("16");
    offi_chart[1].axisY->remove("17");
    offi_chart[1].axisY->remove("18");
    offi_chart[1].axisY->remove("19");
    offi_chart[1].axisY->remove("20");
    offi_chart[1].axisY->remove("21");
    offi_chart[1].axisY->remove("22");
    offi_chart[1].axisY->remove("23");
    offi_chart[1].axisY->remove("24");
    offi_chart[1].axisY->remove("25");
    offi_chart[1].axisY->remove("26");
    offi_chart[1].axisY->remove("27");
    offi_chart[1].axisY->remove("28");
    offi_chart[1].axisY->remove("29");
    offi_chart[1].axisY->remove("30");
    offi_chart[1].axisY->remove("31");
    offi_chart[1].axisY->remove("32");
    offi_chart[2].axisY->remove("33");
    offi_chart[2].axisY->remove("34");
    offi_chart[2].axisY->remove("35");
    offi_chart[2].axisY->remove("36");
    offi_chart[2].axisY->remove("37");
    offi_chart[2].axisY->remove("38");
    offi_chart[2].axisY->remove("39");
    offi_chart[2].axisY->remove("40");
    offi_chart[2].axisY->remove("41");
    offi_chart[2].axisY->remove("42");
    offi_chart[2].axisY->remove("43");
    offi_chart[2].axisY->remove("44");
    offi_chart[2].axisY->remove("45");
    offi_chart[2].axisY->remove("46");
    offi_chart[2].axisY->remove("47");
    offi_chart[2].axisY->remove("48");
    offi_chart[3].axisY->remove("49");
    offi_chart[3].axisY->remove("50");
    offi_chart[3].axisY->remove("51");
    offi_chart[3].axisY->remove("52");
    offi_chart[3].axisY->remove("53");
    offi_chart[3].axisY->remove("54");
    offi_chart[3].axisY->remove("55");
    offi_chart[3].axisY->remove("56");
    offi_chart[3].axisY->remove("57");
    offi_chart[3].axisY->remove("58");
    offi_chart[3].axisY->remove("59");
    offi_chart[3].axisY->remove("60");
    offi_chart[3].axisY->remove("61");
    offi_chart[3].axisY->remove("62");
    offi_chart[3].axisY->remove("63");
    offi_chart[3].axisY->remove("64");
    switch(index){
        case 0:
            offi_chart[0].axisY->append("01",100);
            offi_chart[0].axisY->append("02",200);
            offi_chart[0].axisY->append("03",300);
            offi_chart[0].axisY->append("04",400);
            offi_chart[0].axisY->append("05",500);
            offi_chart[0].axisY->append("06",600);
            offi_chart[0].axisY->append("07",700);
            offi_chart[0].axisY->append("08",800);
            offi_chart[0].axisY->append("09",900);
            offi_chart[0].axisY->append("10",1000);
            offi_chart[0].axisY->append("11",1100);
            offi_chart[0].axisY->append("12",1200);
            offi_chart[0].axisY->append("13",1300);
            offi_chart[0].axisY->append("14",1400);
            offi_chart[0].axisY->append("15",1500);
            offi_chart[0].axisY->append("16",1600);
            offi_chart[0].axisY->setRange(0,1600);
            offi_chart[1].axisY->append("17",100);
            offi_chart[1].axisY->append("18",200);
            offi_chart[1].axisY->append("19",300);
            offi_chart[1].axisY->append("20",400);
            offi_chart[1].axisY->append("21",500);
            offi_chart[1].axisY->append("22",600);
            offi_chart[1].axisY->append("23",700);
            offi_chart[1].axisY->append("24",800);
            offi_chart[1].axisY->append("25",900);
            offi_chart[1].axisY->append("26",1000);
            offi_chart[1].axisY->append("27",1100);
            offi_chart[1].axisY->append("28",1200);
            offi_chart[1].axisY->append("29",1300);
            offi_chart[1].axisY->append("30",1400);
            offi_chart[1].axisY->append("31",1500);
            offi_chart[1].axisY->append("32",1600);
            offi_chart[1].axisY->setRange(0,1600);
            offi_chart[2].axisY->append("33",100);
            offi_chart[2].axisY->append("34",200);
            offi_chart[2].axisY->append("35",300);
            offi_chart[2].axisY->append("36",400);
            offi_chart[2].axisY->append("37",500);
            offi_chart[2].axisY->append("38",600);
            offi_chart[2].axisY->append("39",700);
            offi_chart[2].axisY->append("40",800);
            offi_chart[2].axisY->append("41",900);
            offi_chart[2].axisY->append("42",1000);
            offi_chart[2].axisY->append("43",1100);
            offi_chart[2].axisY->append("44",1200);
            offi_chart[2].axisY->append("45",1300);
            offi_chart[2].axisY->append("46",1400);
            offi_chart[2].axisY->append("47",1500);
            offi_chart[2].axisY->append("48",1600);
            offi_chart[2].axisY->setRange(0,1600);
            offi_chart[3].axisY->append("49",100);
            offi_chart[3].axisY->append("50",200);
            offi_chart[3].axisY->append("51",300);
            offi_chart[3].axisY->append("52",400);
            offi_chart[3].axisY->append("53",500);
            offi_chart[3].axisY->append("54",600);
            offi_chart[3].axisY->append("55",700);
            offi_chart[3].axisY->append("56",800);
            offi_chart[3].axisY->append("57",900);
            offi_chart[3].axisY->append("58",1000);
            offi_chart[3].axisY->append("59",1100);
            offi_chart[3].axisY->append("60",1200);
            offi_chart[3].axisY->append("61",1300);
            offi_chart[3].axisY->append("62",1400);
            offi_chart[3].axisY->append("63",1500);
            offi_chart[3].axisY->append("64",1600);
            offi_chart[3].axisY->setRange(0,1600);
            offi_frequencyChart.axisY->setRange(-50,50);
            RecordLog("更新数据幅度为50UV");
            break;
        case 1:
            offi_chart[0].axisY->append("01",200);
            offi_chart[0].axisY->append("02",400);
            offi_chart[0].axisY->append("03",600);
            offi_chart[0].axisY->append("04",800);
            offi_chart[0].axisY->append("05",1000);
            offi_chart[0].axisY->append("06",1200);
            offi_chart[0].axisY->append("07",1400);
            offi_chart[0].axisY->append("08",1600);
            offi_chart[0].axisY->append("09",1800);
            offi_chart[0].axisY->append("10",2000);
            offi_chart[0].axisY->append("11",2200);
            offi_chart[0].axisY->append("12",2400);
            offi_chart[0].axisY->append("13",2600);
            offi_chart[0].axisY->append("14",2800);
            offi_chart[0].axisY->append("15",3000);
            offi_chart[0].axisY->append("16",3200);
            offi_chart[0].axisY->setRange(0,3200);
            offi_chart[1].axisY->append("17",200);
            offi_chart[1].axisY->append("18",400);
            offi_chart[1].axisY->append("19",600);
            offi_chart[1].axisY->append("20",800);
            offi_chart[1].axisY->append("21",1000);
            offi_chart[1].axisY->append("22",1200);
            offi_chart[1].axisY->append("23",1400);
            offi_chart[1].axisY->append("24",1600);
            offi_chart[1].axisY->append("25",1800);
            offi_chart[1].axisY->append("26",2000);
            offi_chart[1].axisY->append("27",2200);
            offi_chart[1].axisY->append("28",2400);
            offi_chart[1].axisY->append("29",2600);
            offi_chart[1].axisY->append("30",2800);
            offi_chart[1].axisY->append("31",3000);
            offi_chart[1].axisY->append("32",3200);
            offi_chart[1].axisY->setRange(0,3200);
            offi_chart[2].axisY->append("33",200);
            offi_chart[2].axisY->append("34",400);
            offi_chart[2].axisY->append("35",600);
            offi_chart[2].axisY->append("36",800);
            offi_chart[2].axisY->append("37",1000);
            offi_chart[2].axisY->append("38",1200);
            offi_chart[2].axisY->append("39",1400);
            offi_chart[2].axisY->append("40",1600);
            offi_chart[2].axisY->append("41",1800);
            offi_chart[2].axisY->append("42",2000);
            offi_chart[2].axisY->append("43",2200);
            offi_chart[2].axisY->append("44",2400);
            offi_chart[2].axisY->append("45",2600);
            offi_chart[2].axisY->append("46",2800);
            offi_chart[2].axisY->append("47",3000);
            offi_chart[2].axisY->append("48",3200);
            offi_chart[2].axisY->setRange(0,3200);
            offi_chart[3].axisY->append("49",200);
            offi_chart[3].axisY->append("50",400);
            offi_chart[3].axisY->append("51",600);
            offi_chart[3].axisY->append("52",800);
            offi_chart[3].axisY->append("53",1000);
            offi_chart[3].axisY->append("54",1200);
            offi_chart[3].axisY->append("55",1400);
            offi_chart[3].axisY->append("56",1600);
            offi_chart[3].axisY->append("57",1800);
            offi_chart[3].axisY->append("58",2000);
            offi_chart[3].axisY->append("59",2200);
            offi_chart[3].axisY->append("60",2400);
            offi_chart[3].axisY->append("61",2600);
            offi_chart[3].axisY->append("62",2800);
            offi_chart[3].axisY->append("63",3000);
            offi_chart[3].axisY->append("64",3200);
            offi_chart[3].axisY->setRange(0,3200);
            offi_frequencyChart.axisY->setRange(-100,100);
            RecordLog("更新数据幅度为100UV");
            break;
        case 2:
            offi_chart[0].axisY->append("01",400);
            offi_chart[0].axisY->append("02",800);
            offi_chart[0].axisY->append("03",1200);
            offi_chart[0].axisY->append("04",1600);
            offi_chart[0].axisY->append("05",2000);
            offi_chart[0].axisY->append("06",2400);
            offi_chart[0].axisY->append("07",2800);
            offi_chart[0].axisY->append("08",3200);
            offi_chart[0].axisY->append("09",3600);
            offi_chart[0].axisY->append("10",4000);
            offi_chart[0].axisY->append("11",4400);
            offi_chart[0].axisY->append("12",4800);
            offi_chart[0].axisY->append("13",5200);
            offi_chart[0].axisY->append("14",5600);
            offi_chart[0].axisY->append("15",6000);
            offi_chart[0].axisY->append("16",6400);
            offi_chart[0].axisY->setRange(0,6400);
            offi_chart[1].axisY->append("17",400);
            offi_chart[1].axisY->append("18",800);
            offi_chart[1].axisY->append("19",1200);
            offi_chart[1].axisY->append("20",1600);
            offi_chart[1].axisY->append("21",2000);
            offi_chart[1].axisY->append("22",2400);
            offi_chart[1].axisY->append("23",2800);
            offi_chart[1].axisY->append("24",3200);
            offi_chart[1].axisY->append("25",3600);
            offi_chart[1].axisY->append("26",4000);
            offi_chart[1].axisY->append("27",4400);
            offi_chart[1].axisY->append("28",4800);
            offi_chart[1].axisY->append("29",5200);
            offi_chart[1].axisY->append("30",5600);
            offi_chart[1].axisY->append("31",6000);
            offi_chart[1].axisY->append("32",6400);
            offi_chart[1].axisY->setRange(0,6400);
            offi_chart[2].axisY->append("33",400);
            offi_chart[2].axisY->append("34",800);
            offi_chart[2].axisY->append("35",1200);
            offi_chart[2].axisY->append("36",1600);
            offi_chart[2].axisY->append("37",2000);
            offi_chart[2].axisY->append("38",2400);
            offi_chart[2].axisY->append("39",2800);
            offi_chart[2].axisY->append("40",3200);
            offi_chart[2].axisY->append("41",3600);
            offi_chart[2].axisY->append("42",4000);
            offi_chart[2].axisY->append("43",4400);
            offi_chart[2].axisY->append("44",4800);
            offi_chart[2].axisY->append("45",5200);
            offi_chart[2].axisY->append("46",5600);
            offi_chart[2].axisY->append("47",6000);
            offi_chart[2].axisY->append("48",6400);
            offi_chart[2].axisY->setRange(0,6400);
            offi_chart[3].axisY->append("49",400);
            offi_chart[3].axisY->append("50",800);
            offi_chart[3].axisY->append("51",1200);
            offi_chart[3].axisY->append("52",1600);
            offi_chart[3].axisY->append("53",2000);
            offi_chart[3].axisY->append("54",2400);
            offi_chart[3].axisY->append("55",2800);
            offi_chart[3].axisY->append("56",3200);
            offi_chart[3].axisY->append("57",3600);
            offi_chart[3].axisY->append("58",4000);
            offi_chart[3].axisY->append("59",4400);
            offi_chart[3].axisY->append("60",4800);
            offi_chart[3].axisY->append("61",5200);
            offi_chart[3].axisY->append("62",5600);
            offi_chart[3].axisY->append("63",6000);
            offi_chart[3].axisY->append("64",6400);
            offi_chart[3].axisY->setRange(0,6400);
            offi_frequencyChart.axisY->setRange(-200,200);
            RecordLog("更新数据幅度为200UV");
            break;
        case 3:
            offi_chart[0].axisY->append("01",800);
            offi_chart[0].axisY->append("02",1600);
            offi_chart[0].axisY->append("03",2400);
            offi_chart[0].axisY->append("04",3200);
            offi_chart[0].axisY->append("05",4000);
            offi_chart[0].axisY->append("06",4800);
            offi_chart[0].axisY->append("07",5600);
            offi_chart[0].axisY->append("08",6400);
            offi_chart[0].axisY->append("09",7200);
            offi_chart[0].axisY->append("10",8000);
            offi_chart[0].axisY->append("11",8800);
            offi_chart[0].axisY->append("12",9600);
            offi_chart[0].axisY->append("13",10400);
            offi_chart[0].axisY->append("14",11200);
            offi_chart[0].axisY->append("15",12000);
            offi_chart[0].axisY->append("16",12800);
            offi_chart[0].axisY->setRange(0,12800);
            offi_chart[1].axisY->append("17",800);
            offi_chart[1].axisY->append("18",1600);
            offi_chart[1].axisY->append("19",2400);
            offi_chart[1].axisY->append("20",3200);
            offi_chart[1].axisY->append("21",4000);
            offi_chart[1].axisY->append("22",4800);
            offi_chart[1].axisY->append("23",5600);
            offi_chart[1].axisY->append("24",6400);
            offi_chart[1].axisY->append("25",7200);
            offi_chart[1].axisY->append("26",8000);
            offi_chart[1].axisY->append("27",8800);
            offi_chart[1].axisY->append("28",9600);
            offi_chart[1].axisY->append("29",10400);
            offi_chart[1].axisY->append("30",11200);
            offi_chart[1].axisY->append("31",12000);
            offi_chart[1].axisY->append("32",12800);
            offi_chart[1].axisY->setRange(0,12800);
            offi_chart[2].axisY->append("33",800);
            offi_chart[2].axisY->append("34",1600);
            offi_chart[2].axisY->append("35",2400);
            offi_chart[2].axisY->append("36",3200);
            offi_chart[2].axisY->append("37",4000);
            offi_chart[2].axisY->append("38",4800);
            offi_chart[2].axisY->append("39",5600);
            offi_chart[2].axisY->append("40",6400);
            offi_chart[2].axisY->append("41",7200);
            offi_chart[2].axisY->append("42",8000);
            offi_chart[2].axisY->append("43",8800);
            offi_chart[2].axisY->append("44",9600);
            offi_chart[2].axisY->append("45",10400);
            offi_chart[2].axisY->append("46",11200);
            offi_chart[2].axisY->append("47",12000);
            offi_chart[2].axisY->append("48",12800);
            offi_chart[2].axisY->setRange(0,12800);
            offi_chart[3].axisY->append("49",800);
            offi_chart[3].axisY->append("50",1600);
            offi_chart[3].axisY->append("51",2400);
            offi_chart[3].axisY->append("52",3200);
            offi_chart[3].axisY->append("53",4000);
            offi_chart[3].axisY->append("54",4800);
            offi_chart[3].axisY->append("55",5600);
            offi_chart[3].axisY->append("56",6400);
            offi_chart[3].axisY->append("57",7200);
            offi_chart[3].axisY->append("58",8000);
            offi_chart[3].axisY->append("59",8800);
            offi_chart[3].axisY->append("60",9600);
            offi_chart[3].axisY->append("61",10400);
            offi_chart[3].axisY->append("62",11200);
            offi_chart[3].axisY->append("63",12000);
            offi_chart[3].axisY->append("64",12800);
            offi_chart[3].axisY->setRange(0,12800);
            offi_frequencyChart.axisY->setRange(-400,400);
            RecordLog("更新数据幅度为400UV");
            break;
        case 4:
            offi_chart[0].axisY->append("01",2000);
            offi_chart[0].axisY->append("02",4000);
            offi_chart[0].axisY->append("03",6000);
            offi_chart[0].axisY->append("04",8000);
            offi_chart[0].axisY->append("05",10000);
            offi_chart[0].axisY->append("06",12000);
            offi_chart[0].axisY->append("07",14000);
            offi_chart[0].axisY->append("08",16000);
            offi_chart[0].axisY->append("09",18000);
            offi_chart[0].axisY->append("10",20000);
            offi_chart[0].axisY->append("11",22000);
            offi_chart[0].axisY->append("12",24000);
            offi_chart[0].axisY->append("13",26000);
            offi_chart[0].axisY->append("14",28000);
            offi_chart[0].axisY->append("15",30000);
            offi_chart[0].axisY->append("16",32000);
            offi_chart[0].axisY->setRange(0,32000);
            offi_chart[1].axisY->append("17",2000);
            offi_chart[1].axisY->append("18",4000);
            offi_chart[1].axisY->append("19",6000);
            offi_chart[1].axisY->append("20",8000);
            offi_chart[1].axisY->append("21",10000);
            offi_chart[1].axisY->append("22",12000);
            offi_chart[1].axisY->append("23",14000);
            offi_chart[1].axisY->append("24",16000);
            offi_chart[1].axisY->append("25",18000);
            offi_chart[1].axisY->append("26",20000);
            offi_chart[1].axisY->append("27",22000);
            offi_chart[1].axisY->append("28",24000);
            offi_chart[1].axisY->append("29",26000);
            offi_chart[1].axisY->append("30",28000);
            offi_chart[1].axisY->append("31",30000);
            offi_chart[1].axisY->append("32",32000);
            offi_chart[1].axisY->setRange(0,32000);
            offi_chart[2].axisY->append("33",2000);
            offi_chart[2].axisY->append("34",4000);
            offi_chart[2].axisY->append("35",6000);
            offi_chart[2].axisY->append("36",8000);
            offi_chart[2].axisY->append("37",10000);
            offi_chart[2].axisY->append("38",12000);
            offi_chart[2].axisY->append("39",14000);
            offi_chart[2].axisY->append("40",16000);
            offi_chart[2].axisY->append("41",18000);
            offi_chart[2].axisY->append("42",20000);
            offi_chart[2].axisY->append("43",22000);
            offi_chart[2].axisY->append("44",24000);
            offi_chart[2].axisY->append("45",26000);
            offi_chart[2].axisY->append("46",28000);
            offi_chart[2].axisY->append("47",30000);
            offi_chart[2].axisY->append("48",32000);
            offi_chart[2].axisY->setRange(0,32000);
            offi_chart[3].axisY->append("49",2000);
            offi_chart[3].axisY->append("50",4000);
            offi_chart[3].axisY->append("51",6000);
            offi_chart[3].axisY->append("52",8000);
            offi_chart[3].axisY->append("53",10000);
            offi_chart[3].axisY->append("54",12000);
            offi_chart[3].axisY->append("55",14000);
            offi_chart[3].axisY->append("56",16000);
            offi_chart[3].axisY->append("57",18000);
            offi_chart[3].axisY->append("58",20000);
            offi_chart[3].axisY->append("59",22000);
            offi_chart[3].axisY->append("60",24000);
            offi_chart[3].axisY->append("61",26000);
            offi_chart[3].axisY->append("62",28000);
            offi_chart[3].axisY->append("63",30000);
            offi_chart[3].axisY->append("64",32000);
            offi_chart[3].axisY->setRange(0,32000);
            offi_frequencyChart.axisY->setRange(-1000,1000);
            RecordLog("更新数据幅度为1000UV");
            break;
        case 5:
            offi_chart[0].axisY->append("01",20000);
            offi_chart[0].axisY->append("02",40000);
            offi_chart[0].axisY->append("03",60000);
            offi_chart[0].axisY->append("04",80000);
            offi_chart[0].axisY->append("05",100000);
            offi_chart[0].axisY->append("06",120000);
            offi_chart[0].axisY->append("07",140000);
            offi_chart[0].axisY->append("08",160000);
            offi_chart[0].axisY->append("09",180000);
            offi_chart[0].axisY->append("10",200000);
            offi_chart[0].axisY->append("11",220000);
            offi_chart[0].axisY->append("12",240000);
            offi_chart[0].axisY->append("13",260000);
            offi_chart[0].axisY->append("14",280000);
            offi_chart[0].axisY->append("15",300000);
            offi_chart[0].axisY->append("16",320000);
            offi_chart[0].axisY->setRange(0,320000);
            offi_chart[1].axisY->append("17",20000);
            offi_chart[1].axisY->append("18",40000);
            offi_chart[1].axisY->append("19",60000);
            offi_chart[1].axisY->append("20",80000);
            offi_chart[1].axisY->append("21",100000);
            offi_chart[1].axisY->append("22",120000);
            offi_chart[1].axisY->append("23",140000);
            offi_chart[1].axisY->append("24",160000);
            offi_chart[1].axisY->append("25",180000);
            offi_chart[1].axisY->append("26",200000);
            offi_chart[1].axisY->append("27",220000);
            offi_chart[1].axisY->append("28",240000);
            offi_chart[1].axisY->append("29",260000);
            offi_chart[1].axisY->append("30",280000);
            offi_chart[1].axisY->append("31",300000);
            offi_chart[1].axisY->append("32",320000);
            offi_chart[1].axisY->setRange(0,320000);
            offi_chart[2].axisY->append("33",20000);
            offi_chart[2].axisY->append("34",40000);
            offi_chart[2].axisY->append("35",60000);
            offi_chart[2].axisY->append("36",80000);
            offi_chart[2].axisY->append("37",100000);
            offi_chart[2].axisY->append("38",120000);
            offi_chart[2].axisY->append("39",140000);
            offi_chart[2].axisY->append("40",160000);
            offi_chart[2].axisY->append("41",180000);
            offi_chart[2].axisY->append("42",200000);
            offi_chart[2].axisY->append("43",220000);
            offi_chart[2].axisY->append("44",240000);
            offi_chart[2].axisY->append("45",260000);
            offi_chart[2].axisY->append("46",280000);
            offi_chart[2].axisY->append("47",300000);
            offi_chart[2].axisY->append("48",320000);
            offi_chart[2].axisY->setRange(0,320000);
            offi_chart[3].axisY->append("49",20000);
            offi_chart[3].axisY->append("50",40000);
            offi_chart[3].axisY->append("51",60000);
            offi_chart[3].axisY->append("52",80000);
            offi_chart[3].axisY->append("53",100000);
            offi_chart[3].axisY->append("54",120000);
            offi_chart[3].axisY->append("55",140000);
            offi_chart[3].axisY->append("56",160000);
            offi_chart[3].axisY->append("57",180000);
            offi_chart[3].axisY->append("58",200000);
            offi_chart[3].axisY->append("59",220000);
            offi_chart[3].axisY->append("60",240000);
            offi_chart[3].axisY->append("61",260000);
            offi_chart[3].axisY->append("62",280000);
            offi_chart[3].axisY->append("63",300000);
            offi_chart[3].axisY->append("64",320000);
            offi_chart[3].axisY->setRange(0,320000);
            offi_frequencyChart.axisY->setRange(-10000,10000);
            RecordLog("更新数据幅度为10000UV");
            break;
    default:
        break;
    }

    QMutexLocker locker(&(offi_realData.mutex));
    QVector<QPointF> tempV;
    int channel = 0;
    int tempI;
    int secondRangeBase;
    for(; channel < DATA_CHANNEL_COUNT; channel++){
        secondRangeBase = channel%16;
        tempV = offi_realData.data[channel]->pointsVector();
        for(tempI = 0; tempI < tempV.count(); tempI++)
            tempV[tempI].setY(tempV[tempI].y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][secondRangeBase] + INTERFACE_RANGE_BASE_VALUE[index][secondRangeBase]);
        offi_realData.data[channel]->replace(tempV);
    }
    offi_rangeBase = index;
}
void OfflineInterface::ChannelChanged(int index){
    switch(index){
        case 0:
            offi_chart[0].view->setVisible(true);
            offi_chart[1].view->setVisible(true);
            offi_chart[2].view->setVisible(true);
            offi_chart[3].view->setVisible(true);
            RecordLog("显示所有通道时域图");
            break;
        case 1:
            offi_chart[0].view->setVisible(true);
            offi_chart[1].view->setVisible(false);
            offi_chart[2].view->setVisible(false);
            offi_chart[3].view->setVisible(false);
            RecordLog("显示01到16通道时域图");
            break;
        case 2:
            offi_chart[0].view->setVisible(false);
            offi_chart[1].view->setVisible(true);
            offi_chart[2].view->setVisible(false);
            offi_chart[3].view->setVisible(false);
            RecordLog("显示17到32通道时域图");
            break;
        case 3:
            offi_chart[0].view->setVisible(false);
            offi_chart[1].view->setVisible(false);
            offi_chart[2].view->setVisible(true);
            offi_chart[3].view->setVisible(false);
            RecordLog("显示33到48通道时域图");
            break;
        case 4:
            offi_chart[0].view->setVisible(false);
            offi_chart[1].view->setVisible(false);
            offi_chart[2].view->setVisible(false);
            offi_chart[3].view->setVisible(true);
            RecordLog("显示49到64通道时域图");
            break;
        default:

            break;
    }
}

void OfflineInterface::FrequencyChanged(int index){
    offi_frequencyChart.axisX->setTickCount(7);
    offi_frequencyChart.axisX->setRange(0,INTERFACE_FREQUENCY_FFTW[index]);
    RecordLog("更新频域图频率为"+QString::number(INTERFACE_FREQUENCY_FFTW[index]));
}

void OfflineInterface::ChoiceOfflinePath(){
    QString path = QFileDialog::
            getExistingDirectory(this,tr("查找目录"),".", QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(path.size() > 0){
        offi_offlinePathEdit->setText(path);
        offi_offlineFilePathEdit->setText("");
        RecordLog("选择离线播放目录为"+path);
    }
}

void OfflineInterface::ChoiceOfflineFilePath(){
    QString path = QFileDialog::
            getOpenFileName(this, tr("查找文件"),".",tr("*.*"));
    if(path.size() > 0){
        offi_offlineFilePathEdit->setText(path);
        offi_offlinePathEdit->setText("");
        RecordLog("选择离线播放文件"+path);
    }
}

void OfflineInterface::SlaveChanged(int index){
    offi_offlineFilePathEdit->setText("");
    if(index == -1){
        offi_offlinePathEdit->setText("");
        return;
    }
    offi_offlinePathEdit->setText(offi_slaveList->at(index).path);
    if(index == 0){
        offi_offlinePathChoice->setEnabled(true);
        offi_offlineFilePathChoice->setEnabled(true);
        RecordLog("切换为自选离线播放数据，请选择数据目录或数据文件");
    }else{
        offi_offlinePathChoice->setEnabled(false);
        offi_offlineFilePathChoice->setEnabled(false);
        RecordLog("下位机ID/IP："+offi_slaveList->at(index).id+"，数据目录："+offi_slaveList->at(index).path);
    }
}

void OfflineInterface::DoInit(int unit,int speed,QString dataDir){
    offi_realUnitCount = unit;
    offi_speed = speed;
    SLAVE_LIST curSlave = {"自选目录或文件",""};
    offi_slaveList->clear();
    offi_slaveList->append(curSlave);
    QDir dir(dataDir);
    if(dir.exists()){
        QFileInfoList fileList = dir.entryInfoList();
        for(QFileInfoList::iterator i = fileList.begin(); i != fileList.end(); i++){
            if(!(i->isDir()) || i->fileName() == "." || i->fileName() == "..")
                continue;
            curSlave.path = i->filePath() + "/";
            curSlave.id = "";
            QFile fileId(curSlave.path+FILE_ID_NAME);
            if(fileId.open(QIODevice::ReadOnly|QIODevice::Text)){
                curSlave.id = fileId.readAll();
                fileId.close();
            }
            if(curSlave.id == ""){
                QFile fileIp(curSlave.path+FILE_IP_NAME);
                if(fileIp.open(QIODevice::ReadOnly|QIODevice::Text)){
                    curSlave.id = fileIp.readAll();
                    fileIp.close();
                }
            }
            offi_slaveList->append(curSlave);
        }
    }

    offi_slave->Clear();
    for(QList<SLAVE_LIST>::iterator i = offi_slaveList->begin(); i != offi_slaveList->end(); i++)
        offi_slave->AddItem(i->id);
    StopUpdateData();
}

void OfflineInterface::DoFree(){
    offi_slaveList->clear();
    offi_slave->Clear();
    StopUpdateData();
}

void OfflineInterface::UpdateData(){
    if(offi_updateStatus != UPDATA_STATUS_START)
        return;
    offi_dataFile.buff = offi_dataFile.file.read(MQ_MESSAGE_LENGHT*offi_realUnitCount);
    if(offi_dataFile.buff.size() < MQ_MESSAGE_LENGHT){
        if(offi_dataFile.buff.size() == 0){
            if(offi_dataFile.file.isOpen())
                offi_dataFile.file.close();
            if(offi_dataFile.curReadFilePosInList >= offi_dataFile.fileList.size()){
                StopUpdateData();
                RecordLog("下位机"+offi_slaveList->at(offi_slave->CurrentIndex()).id+"的离线数据播放完成");
                return;
            }
            offi_dataFile.file.setFileName(offi_dataFile.fileList.at(offi_dataFile.curReadFilePosInList));
            offi_dataFile.curReadFilePosInList ++;
            offi_dataFile.file.open(QFile::ReadOnly);
        }
        return;
    }
    QMutexLocker locker(&(offi_realData.mutex));
    UpdateTimeData();
    UpdateFrequencyData();
}

void OfflineInterface::UpdateTimeData(){
    int readCount = offi_dataFile.buff.size() / MQ_MESSAGE_LENGHT;
    int startPos;
    for(int i = 0; i < readCount; i++){
        startPos = i * MQ_MESSAGE_LENGHT;
        *offi_dataFile.newData[0]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+3,startPos+4,startPos+5))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][0]);
        *offi_dataFile.newData[1]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+6,startPos+7,startPos+8))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][1]);
        *offi_dataFile.newData[2]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+9,startPos+10,startPos+11))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][2]);
        *offi_dataFile.newData[3]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+12,startPos+13,startPos+14))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][3]);
        *offi_dataFile.newData[4]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+15,startPos+16,startPos+17))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][4]);
        *offi_dataFile.newData[5]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+18,startPos+19,startPos+20))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][5]);
        *offi_dataFile.newData[6]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+21,startPos+22,startPos+23))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][6]);
        *offi_dataFile.newData[7]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+24,startPos+25,startPos+26))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][7]);
        *offi_dataFile.newData[8]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+27,startPos+28,startPos+29))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][8]);
        *offi_dataFile.newData[9]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+30,startPos+31,startPos+32))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][9]);
        *offi_dataFile.newData[10]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+33,startPos+34,startPos+35))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][10]);
        *offi_dataFile.newData[11]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+36,startPos+37,startPos+38))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][11]);
        *offi_dataFile.newData[12]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+39,startPos+40,startPos+41))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][12]);
        *offi_dataFile.newData[13]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+42,startPos+43,startPos+44))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][13]);
        *offi_dataFile.newData[14]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+45,startPos+46,startPos+47))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][14]);
        *offi_dataFile.newData[15]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+48,startPos+49,startPos+50))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][15]);
        *offi_dataFile.newData[16]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+51,startPos+52,startPos+53))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][0]);
        *offi_dataFile.newData[17]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+54,startPos+55,startPos+56))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][1]);
        *offi_dataFile.newData[18]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+57,startPos+58,startPos+59))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][2]);
        *offi_dataFile.newData[19]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+60,startPos+61,startPos+62))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][3]);
        *offi_dataFile.newData[20]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+63,startPos+64,startPos+65))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][4]);
        *offi_dataFile.newData[21]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+66,startPos+67,startPos+68))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][5]);
        *offi_dataFile.newData[22]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+69,startPos+70,startPos+71))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][6]);
        *offi_dataFile.newData[23]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+72,startPos+73,startPos+74))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][7]);
        *offi_dataFile.newData[24]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+75,startPos+76,startPos+77))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][8]);
        *offi_dataFile.newData[25]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+78,startPos+79,startPos+80))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][9]);
        *offi_dataFile.newData[26]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+81,startPos+82,startPos+83))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][10]);
        *offi_dataFile.newData[27]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+84,startPos+85,startPos+86))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][11]);
        *offi_dataFile.newData[28]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+87,startPos+88,startPos+89))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][12]);
        *offi_dataFile.newData[29]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+90,startPos+91,startPos+92))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][13]);
        *offi_dataFile.newData[30]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+93,startPos+94,startPos+95))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][14]);
        *offi_dataFile.newData[31]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+96,startPos+97,startPos+98))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][15]);
        *offi_dataFile.newData[32]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+99,startPos+100,startPos+101))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][0]);
        *offi_dataFile.newData[33]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+102,startPos+103,startPos+104))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][1]);
        *offi_dataFile.newData[34]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+105,startPos+106,startPos+107))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][2]);
        *offi_dataFile.newData[35]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+108,startPos+109,startPos+110))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][3]);
        *offi_dataFile.newData[36]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+111,startPos+112,startPos+113))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][4]);
        *offi_dataFile.newData[37]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+114,startPos+115,startPos+116))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][5]);
        *offi_dataFile.newData[38]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+117,startPos+118,startPos+119))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][6]);
        *offi_dataFile.newData[39]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+120,startPos+121,startPos+122))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][7]);
        *offi_dataFile.newData[40]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+123,startPos+124,startPos+125))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][8]);
        *offi_dataFile.newData[41]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+126,startPos+127,startPos+128))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][9]);
        *offi_dataFile.newData[42]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+129,startPos+130,startPos+131))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][10]);
        *offi_dataFile.newData[43]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+132,startPos+133,startPos+134))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][11]);
        *offi_dataFile.newData[44]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+135,startPos+136,startPos+137))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][12]);
        *offi_dataFile.newData[45]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+138,startPos+139,startPos+140))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][13]);
        *offi_dataFile.newData[46]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+141,startPos+142,startPos+143))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][14]);
        *offi_dataFile.newData[47]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+144,startPos+145,startPos+146))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][15]);
        *offi_dataFile.newData[48]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+147,startPos+148,startPos+149))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][0]);
        *offi_dataFile.newData[49]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+150,startPos+151,startPos+152))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][1]);
        *offi_dataFile.newData[50]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+153,startPos+154,startPos+155))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][2]);
        *offi_dataFile.newData[51]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+156,startPos+157,startPos+158))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][3]);
        *offi_dataFile.newData[52]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+159,startPos+160,startPos+161))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][4]);
        *offi_dataFile.newData[53]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+162,startPos+163,startPos+164))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][5]);
        *offi_dataFile.newData[54]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+165,startPos+166,startPos+167))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][6]);
        *offi_dataFile.newData[55]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+168,startPos+169,startPos+170))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][7]);
        *offi_dataFile.newData[56]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+171,startPos+172,startPos+173))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][8]);
        *offi_dataFile.newData[57]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+174,startPos+175,startPos+176))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][9]);
        *offi_dataFile.newData[58]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+177,startPos+178,startPos+179))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][10]);
        *offi_dataFile.newData[59]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+180,startPos+181,startPos+182))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][11]);
        *offi_dataFile.newData[60]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+183,startPos+184,startPos+185))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][12]);
        *offi_dataFile.newData[61]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+186,startPos+187,startPos+188))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][13]);
        *offi_dataFile.newData[62]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+189,startPos+190,startPos+191))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][14]);
        *offi_dataFile.newData[63]<<QPointF(offi_realData.curEndX,(GET_VALUE(offi_dataFile.buff,startPos+192,startPos+193,startPos+194))+INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][15]);
        offi_realData.curEndX ++;
    }

    *(offi_realData.data[0])<<*(offi_dataFile.newData[0]);
    *(offi_realData.data[1])<<*(offi_dataFile.newData[1]);
    *(offi_realData.data[2])<<*(offi_dataFile.newData[2]);
    *(offi_realData.data[3])<<*(offi_dataFile.newData[3]);
    *(offi_realData.data[4])<<*(offi_dataFile.newData[4]);
    *(offi_realData.data[5])<<*(offi_dataFile.newData[5]);
    *(offi_realData.data[6])<<*(offi_dataFile.newData[6]);
    *(offi_realData.data[7])<<*(offi_dataFile.newData[7]);
    *(offi_realData.data[8])<<*(offi_dataFile.newData[8]);
    *(offi_realData.data[9])<<*(offi_dataFile.newData[9]);
    *(offi_realData.data[10])<<*(offi_dataFile.newData[10]);
    *(offi_realData.data[11])<<*(offi_dataFile.newData[11]);
    *(offi_realData.data[12])<<*(offi_dataFile.newData[12]);
    *(offi_realData.data[13])<<*(offi_dataFile.newData[13]);
    *(offi_realData.data[14])<<*(offi_dataFile.newData[14]);
    *(offi_realData.data[15])<<*(offi_dataFile.newData[15]);
    *(offi_realData.data[16])<<*(offi_dataFile.newData[16]);
    *(offi_realData.data[17])<<*(offi_dataFile.newData[17]);
    *(offi_realData.data[18])<<*(offi_dataFile.newData[18]);
    *(offi_realData.data[19])<<*(offi_dataFile.newData[19]);
    *(offi_realData.data[20])<<*(offi_dataFile.newData[20]);
    *(offi_realData.data[21])<<*(offi_dataFile.newData[21]);
    *(offi_realData.data[22])<<*(offi_dataFile.newData[22]);
    *(offi_realData.data[23])<<*(offi_dataFile.newData[23]);
    *(offi_realData.data[24])<<*(offi_dataFile.newData[24]);
    *(offi_realData.data[25])<<*(offi_dataFile.newData[25]);
    *(offi_realData.data[26])<<*(offi_dataFile.newData[26]);
    *(offi_realData.data[27])<<*(offi_dataFile.newData[27]);
    *(offi_realData.data[28])<<*(offi_dataFile.newData[28]);
    *(offi_realData.data[29])<<*(offi_dataFile.newData[29]);
    *(offi_realData.data[30])<<*(offi_dataFile.newData[30]);
    *(offi_realData.data[31])<<*(offi_dataFile.newData[31]);
    *(offi_realData.data[32])<<*(offi_dataFile.newData[32]);
    *(offi_realData.data[33])<<*(offi_dataFile.newData[33]);
    *(offi_realData.data[34])<<*(offi_dataFile.newData[34]);
    *(offi_realData.data[35])<<*(offi_dataFile.newData[35]);
    *(offi_realData.data[36])<<*(offi_dataFile.newData[36]);
    *(offi_realData.data[37])<<*(offi_dataFile.newData[37]);
    *(offi_realData.data[38])<<*(offi_dataFile.newData[38]);
    *(offi_realData.data[39])<<*(offi_dataFile.newData[39]);
    *(offi_realData.data[40])<<*(offi_dataFile.newData[40]);
    *(offi_realData.data[41])<<*(offi_dataFile.newData[41]);
    *(offi_realData.data[42])<<*(offi_dataFile.newData[42]);
    *(offi_realData.data[43])<<*(offi_dataFile.newData[43]);
    *(offi_realData.data[44])<<*(offi_dataFile.newData[44]);
    *(offi_realData.data[45])<<*(offi_dataFile.newData[45]);
    *(offi_realData.data[46])<<*(offi_dataFile.newData[46]);
    *(offi_realData.data[47])<<*(offi_dataFile.newData[47]);
    *(offi_realData.data[48])<<*(offi_dataFile.newData[48]);
    *(offi_realData.data[49])<<*(offi_dataFile.newData[49]);
    *(offi_realData.data[50])<<*(offi_dataFile.newData[50]);
    *(offi_realData.data[51])<<*(offi_dataFile.newData[51]);
    *(offi_realData.data[52])<<*(offi_dataFile.newData[52]);
    *(offi_realData.data[53])<<*(offi_dataFile.newData[53]);
    *(offi_realData.data[54])<<*(offi_dataFile.newData[54]);
    *(offi_realData.data[55])<<*(offi_dataFile.newData[55]);
    *(offi_realData.data[56])<<*(offi_dataFile.newData[56]);
    *(offi_realData.data[57])<<*(offi_dataFile.newData[57]);
    *(offi_realData.data[58])<<*(offi_dataFile.newData[58]);
    *(offi_realData.data[59])<<*(offi_dataFile.newData[59]);
    *(offi_realData.data[60])<<*(offi_dataFile.newData[60]);
    *(offi_realData.data[61])<<*(offi_dataFile.newData[61]);
    *(offi_realData.data[62])<<*(offi_dataFile.newData[62]);
    *(offi_realData.data[63])<<*(offi_dataFile.newData[63]);
    offi_dataFile.newData[0]->clear();
    offi_dataFile.newData[1]->clear();
    offi_dataFile.newData[2]->clear();
    offi_dataFile.newData[3]->clear();
    offi_dataFile.newData[4]->clear();
    offi_dataFile.newData[5]->clear();
    offi_dataFile.newData[6]->clear();
    offi_dataFile.newData[7]->clear();
    offi_dataFile.newData[8]->clear();
    offi_dataFile.newData[9]->clear();
    offi_dataFile.newData[10]->clear();
    offi_dataFile.newData[11]->clear();
    offi_dataFile.newData[12]->clear();
    offi_dataFile.newData[13]->clear();
    offi_dataFile.newData[14]->clear();
    offi_dataFile.newData[15]->clear();
    offi_dataFile.newData[16]->clear();
    offi_dataFile.newData[17]->clear();
    offi_dataFile.newData[18]->clear();
    offi_dataFile.newData[19]->clear();
    offi_dataFile.newData[20]->clear();
    offi_dataFile.newData[21]->clear();
    offi_dataFile.newData[22]->clear();
    offi_dataFile.newData[23]->clear();
    offi_dataFile.newData[24]->clear();
    offi_dataFile.newData[25]->clear();
    offi_dataFile.newData[26]->clear();
    offi_dataFile.newData[27]->clear();
    offi_dataFile.newData[28]->clear();
    offi_dataFile.newData[29]->clear();
    offi_dataFile.newData[30]->clear();
    offi_dataFile.newData[31]->clear();
    offi_dataFile.newData[32]->clear();
    offi_dataFile.newData[33]->clear();
    offi_dataFile.newData[34]->clear();
    offi_dataFile.newData[35]->clear();
    offi_dataFile.newData[36]->clear();
    offi_dataFile.newData[37]->clear();
    offi_dataFile.newData[38]->clear();
    offi_dataFile.newData[39]->clear();
    offi_dataFile.newData[40]->clear();
    offi_dataFile.newData[41]->clear();
    offi_dataFile.newData[42]->clear();
    offi_dataFile.newData[43]->clear();
    offi_dataFile.newData[44]->clear();
    offi_dataFile.newData[45]->clear();
    offi_dataFile.newData[46]->clear();
    offi_dataFile.newData[47]->clear();
    offi_dataFile.newData[48]->clear();
    offi_dataFile.newData[49]->clear();
    offi_dataFile.newData[50]->clear();
    offi_dataFile.newData[51]->clear();
    offi_dataFile.newData[52]->clear();
    offi_dataFile.newData[53]->clear();
    offi_dataFile.newData[54]->clear();
    offi_dataFile.newData[55]->clear();
    offi_dataFile.newData[56]->clear();
    offi_dataFile.newData[57]->clear();
    offi_dataFile.newData[58]->clear();
    offi_dataFile.newData[59]->clear();
    offi_dataFile.newData[60]->clear();
    offi_dataFile.newData[61]->clear();
    offi_dataFile.newData[62]->clear();
    offi_dataFile.newData[63]->clear();


    qreal curStartX = offi_realData.curEndX-DATA_CACHE_MAX[offi_timeSlot->CurrentIndex()];
    offi_chart[0].axisXValue->setRange(curStartX,offi_realData.curEndX);
    offi_chart[1].axisXValue->setRange(curStartX,offi_realData.curEndX);
    offi_chart[2].axisXValue->setRange(curStartX,offi_realData.curEndX);
    offi_chart[3].axisXValue->setRange(curStartX,offi_realData.curEndX);

    int removeCount = offi_realData.data[0]->count() - DATA_CACHE_MAX[3];
    if(removeCount > 0){
        offi_realData.data[0]->removePoints(0,removeCount);
        offi_realData.data[1]->removePoints(0,removeCount);
        offi_realData.data[2]->removePoints(0,removeCount);
        offi_realData.data[3]->removePoints(0,removeCount);
        offi_realData.data[4]->removePoints(0,removeCount);
        offi_realData.data[5]->removePoints(0,removeCount);
        offi_realData.data[6]->removePoints(0,removeCount);
        offi_realData.data[7]->removePoints(0,removeCount);
        offi_realData.data[8]->removePoints(0,removeCount);
        offi_realData.data[9]->removePoints(0,removeCount);
        offi_realData.data[10]->removePoints(0,removeCount);
        offi_realData.data[11]->removePoints(0,removeCount);
        offi_realData.data[12]->removePoints(0,removeCount);
        offi_realData.data[13]->removePoints(0,removeCount);
        offi_realData.data[14]->removePoints(0,removeCount);
        offi_realData.data[15]->removePoints(0,removeCount);
        offi_realData.data[16]->removePoints(0,removeCount);
        offi_realData.data[17]->removePoints(0,removeCount);
        offi_realData.data[18]->removePoints(0,removeCount);
        offi_realData.data[19]->removePoints(0,removeCount);
        offi_realData.data[20]->removePoints(0,removeCount);
        offi_realData.data[21]->removePoints(0,removeCount);
        offi_realData.data[22]->removePoints(0,removeCount);
        offi_realData.data[23]->removePoints(0,removeCount);
        offi_realData.data[24]->removePoints(0,removeCount);
        offi_realData.data[25]->removePoints(0,removeCount);
        offi_realData.data[26]->removePoints(0,removeCount);
        offi_realData.data[27]->removePoints(0,removeCount);
        offi_realData.data[28]->removePoints(0,removeCount);
        offi_realData.data[29]->removePoints(0,removeCount);
        offi_realData.data[30]->removePoints(0,removeCount);
        offi_realData.data[31]->removePoints(0,removeCount);
        offi_realData.data[32]->removePoints(0,removeCount);
        offi_realData.data[33]->removePoints(0,removeCount);
        offi_realData.data[34]->removePoints(0,removeCount);
        offi_realData.data[35]->removePoints(0,removeCount);
        offi_realData.data[36]->removePoints(0,removeCount);
        offi_realData.data[37]->removePoints(0,removeCount);
        offi_realData.data[38]->removePoints(0,removeCount);
        offi_realData.data[39]->removePoints(0,removeCount);
        offi_realData.data[40]->removePoints(0,removeCount);
        offi_realData.data[41]->removePoints(0,removeCount);
        offi_realData.data[42]->removePoints(0,removeCount);
        offi_realData.data[43]->removePoints(0,removeCount);
        offi_realData.data[44]->removePoints(0,removeCount);
        offi_realData.data[45]->removePoints(0,removeCount);
        offi_realData.data[46]->removePoints(0,removeCount);
        offi_realData.data[47]->removePoints(0,removeCount);
        offi_realData.data[48]->removePoints(0,removeCount);
        offi_realData.data[49]->removePoints(0,removeCount);
        offi_realData.data[50]->removePoints(0,removeCount);
        offi_realData.data[51]->removePoints(0,removeCount);
        offi_realData.data[52]->removePoints(0,removeCount);
        offi_realData.data[53]->removePoints(0,removeCount);
        offi_realData.data[54]->removePoints(0,removeCount);
        offi_realData.data[55]->removePoints(0,removeCount);
        offi_realData.data[56]->removePoints(0,removeCount);
        offi_realData.data[57]->removePoints(0,removeCount);
        offi_realData.data[58]->removePoints(0,removeCount);
        offi_realData.data[59]->removePoints(0,removeCount);
        offi_realData.data[60]->removePoints(0,removeCount);
        offi_realData.data[61]->removePoints(0,removeCount);
        offi_realData.data[62]->removePoints(0,removeCount);
        offi_realData.data[63]->removePoints(0,removeCount);
    }
}
void OfflineInterface::UpdateFrequencyData(){
    offi_fftw.index = offi_frequency->CurrentIndex();
    if(offi_realData.data[0]->count() > INTERFACE_FREQUENCY_FFTW[offi_fftw.index]){
        offi_fftw.end = offi_realData.data[0]->count() - 1;
        offi_fftw.start = offi_fftw.end - INTERFACE_FREQUENCY_FFTW[offi_fftw.index] + 1;
    }else{
        offi_fftw.end = offi_realData.data[0]->count() -1;
        offi_fftw.start = 0;
    }
    UpdateFrequencyDataCalculation(offi_realData.data[0],offi_frequencyChart.data[0]);
    UpdateFrequencyDataCalculation(offi_realData.data[1],offi_frequencyChart.data[1]);
    UpdateFrequencyDataCalculation(offi_realData.data[16],offi_frequencyChart.data[2]);
    UpdateFrequencyDataCalculation(offi_realData.data[17],offi_frequencyChart.data[3]);
    UpdateFrequencyDataCalculation(offi_realData.data[32],offi_frequencyChart.data[4]);
    UpdateFrequencyDataCalculation(offi_realData.data[33],offi_frequencyChart.data[5]);
    UpdateFrequencyDataCalculation(offi_realData.data[48],offi_frequencyChart.data[6]);
    UpdateFrequencyDataCalculation(offi_realData.data[49],offi_frequencyChart.data[7]);
}

void OfflineInterface::UpdateFrequencyDataCalculation(QLineSeries *source,QLineSeries *object){
    offi_fftw.cursorFrom = offi_fftw.start;
    offi_fftw.cursorTo = 0;
    for(; offi_fftw.cursorFrom <= offi_fftw.end; offi_fftw.cursorFrom++,offi_fftw.cursorTo++)
        offi_fftw.inData[offi_fftw.cursorTo] = source->at(offi_fftw.cursorFrom).y();
    for(;offi_fftw.cursorTo < INTERFACE_FREQUENCY_FFTW[offi_fftw.index]; offi_fftw.cursorTo++)
        offi_fftw.inData[offi_fftw.cursorTo] = 0;
    switch (offi_fftw.index) {
        case 0:
            fftw_execute(offi_fftw.plan30);
            break;
        case 1:
            fftw_execute(offi_fftw.plan60);
            break;
        case 2:
            fftw_execute(offi_fftw.plan120);
            break;
        default:
            break;
    }
    offi_fftw.cursorFrom = 1;
    offi_fftw.cursorTo = 0;
    for(;offi_fftw.cursorTo <= INTERFACE_FREQUENCY_FFTW[offi_fftw.index]; offi_fftw.cursorFrom++,offi_fftw.cursorTo++)
        object->replace(offi_fftw.cursorTo,offi_fftw.cursorTo,offi_fftw.outData[0][offi_fftw.cursorFrom]);
}

void OfflineInterface::StartUpdateData(){
    switch (offi_updateStatus) {
        case UPDATA_STATUS_START:
            RecordLog("目前为播放状态，无需再次执行播放操作");
            return;
            break;
        case UPDATA_STATUS_SUSPEND:
            offi_updateStatus = UPDATA_STATUS_START;
            offi_RMSAndColorCard.updateTimer->start(500);
            RecordLog("继续播放");
            return;
            break;
         case UPDATA_STATUS_STOP:
            break;
        default:
            break;
    }
    if(offi_offlinePathEdit->text() == QString("") &&
           offi_offlineFilePathEdit->text() == QString("") ){
        RecordLog("请指定播放目录或文件");
        return;
    }
    offi_updateStatus = UPDATA_STATUS_START;

    RecordLog("开始播放");

    if(offi_offlinePathEdit->text().size() > 0){
        QDir curDir(offi_offlinePathEdit->text());
        QFileInfoList curFileList = curDir.entryInfoList(QDir::Files|QDir::NoSymLinks|QDir::Readable,QDir::Time|QDir::Reversed);

        for(QFileInfoList::iterator i = curFileList.begin(); i !=  curFileList.end(); i++){
            if(i->fileName().mid(0,QString(FILE_CONFIG_NAME_SUFFIX).count()) == QString(FILE_CONFIG_NAME_SUFFIX))
                continue;
            offi_dataFile.fileList << i->filePath();
        }
    }else if(offi_offlineFilePathEdit->text().size() > 0){
        offi_dataFile.fileList.clear();
        offi_dataFile.fileList.append(offi_offlineFilePathEdit->text());
    }
    offi_dataFile.curReadFilePosInList = 0;
    offi_realData.curEndX = 0;

    if(offi_dataFile.curReadFilePosInList < offi_dataFile.fileList.size()){
        offi_dataFile.file.setFileName(offi_dataFile.fileList.at(offi_dataFile.curReadFilePosInList));
        offi_dataFile.curReadFilePosInList ++;
        offi_dataFile.file.open(QFile::ReadOnly);
    }

    offi_realData.data[0]->clear();
    offi_realData.data[1]->clear();
    offi_realData.data[2]->clear();
    offi_realData.data[3]->clear();
    offi_realData.data[4]->clear();
    offi_realData.data[5]->clear();
    offi_realData.data[6]->clear();
    offi_realData.data[7]->clear();
    offi_realData.data[8]->clear();
    offi_realData.data[9]->clear();
    offi_realData.data[10]->clear();
    offi_realData.data[11]->clear();
    offi_realData.data[12]->clear();
    offi_realData.data[13]->clear();
    offi_realData.data[14]->clear();
    offi_realData.data[15]->clear();
    offi_realData.data[16]->clear();
    offi_realData.data[17]->clear();
    offi_realData.data[18]->clear();
    offi_realData.data[19]->clear();
    offi_realData.data[20]->clear();
    offi_realData.data[21]->clear();
    offi_realData.data[22]->clear();
    offi_realData.data[23]->clear();
    offi_realData.data[24]->clear();
    offi_realData.data[25]->clear();
    offi_realData.data[26]->clear();
    offi_realData.data[27]->clear();
    offi_realData.data[28]->clear();
    offi_realData.data[29]->clear();
    offi_realData.data[30]->clear();
    offi_realData.data[31]->clear();
    offi_realData.data[32]->clear();
    offi_realData.data[33]->clear();
    offi_realData.data[34]->clear();
    offi_realData.data[35]->clear();
    offi_realData.data[36]->clear();
    offi_realData.data[37]->clear();
    offi_realData.data[38]->clear();
    offi_realData.data[39]->clear();
    offi_realData.data[40]->clear();
    offi_realData.data[41]->clear();
    offi_realData.data[42]->clear();
    offi_realData.data[43]->clear();
    offi_realData.data[44]->clear();
    offi_realData.data[45]->clear();
    offi_realData.data[46]->clear();
    offi_realData.data[47]->clear();
    offi_realData.data[48]->clear();
    offi_realData.data[49]->clear();
    offi_realData.data[50]->clear();
    offi_realData.data[51]->clear();
    offi_realData.data[52]->clear();
    offi_realData.data[53]->clear();
    offi_realData.data[54]->clear();
    offi_realData.data[55]->clear();
    offi_realData.data[56]->clear();
    offi_realData.data[57]->clear();
    offi_realData.data[58]->clear();
    offi_realData.data[59]->clear();
    offi_realData.data[60]->clear();
    offi_realData.data[61]->clear();
    offi_realData.data[62]->clear();
    offi_realData.data[63]->clear();

    offi_slave->setEnabled(false);
    offi_offlinePathChoice->setEnabled(false);
    offi_offlineFilePathChoice->setEnabled(false);
    offi_dataUpdateTimer->start(offi_speed);
    offi_RMSAndColorCard.updateTimer->start(500);
}

void OfflineInterface::SuspendUpdateData(){
    switch (offi_updateStatus) {
        case UPDATA_STATUS_START:
            offi_updateStatus = UPDATA_STATUS_SUSPEND;
            offi_RMSAndColorCard.updateTimer->stop();
            RecordLog("暂停播放");
            break;
        case UPDATA_STATUS_SUSPEND:
            RecordLog("目前为暂停状态，无需再次执行暂停操作");
            break;
         case UPDATA_STATUS_STOP:
            RecordLog("目前为停止状态，无法执行暂停操作");
            break;
        default:
            break;
    }
}

void OfflineInterface::StopUpdateData(){
    if(offi_updateStatus == UPDATA_STATUS_STOP){
        RecordLog("目前为停止状态，无需再次执行停止操作");
        return;
    }
    offi_updateStatus = UPDATA_STATUS_STOP;
    RecordLog("停止播放");
    offi_RMSAndColorCard.updateTimer->stop();
    offi_colorCard->DoReset();
    offi_chart[0].rmsGroup->DoReset();
    offi_chart[1].rmsGroup->DoReset();
    offi_chart[2].rmsGroup->DoReset();
    offi_chart[3].rmsGroup->DoReset();
    offi_dataUpdateTimer->stop();

    if(offi_dataFile.file.isOpen())
        offi_dataFile.file.close();
    offi_dataFile.fileList.clear();
    offi_dataFile.curReadFilePosInList = 0;
    offi_realData.curEndX = 0;

    offi_realData.data[0]->clear();
    offi_realData.data[1]->clear();
    offi_realData.data[2]->clear();
    offi_realData.data[3]->clear();
    offi_realData.data[4]->clear();
    offi_realData.data[5]->clear();
    offi_realData.data[6]->clear();
    offi_realData.data[7]->clear();
    offi_realData.data[8]->clear();
    offi_realData.data[9]->clear();
    offi_realData.data[10]->clear();
    offi_realData.data[11]->clear();
    offi_realData.data[12]->clear();
    offi_realData.data[13]->clear();
    offi_realData.data[14]->clear();
    offi_realData.data[15]->clear();
    offi_realData.data[16]->clear();
    offi_realData.data[17]->clear();
    offi_realData.data[18]->clear();
    offi_realData.data[19]->clear();
    offi_realData.data[20]->clear();
    offi_realData.data[21]->clear();
    offi_realData.data[22]->clear();
    offi_realData.data[23]->clear();
    offi_realData.data[24]->clear();
    offi_realData.data[25]->clear();
    offi_realData.data[26]->clear();
    offi_realData.data[27]->clear();
    offi_realData.data[28]->clear();
    offi_realData.data[29]->clear();
    offi_realData.data[30]->clear();
    offi_realData.data[31]->clear();
    offi_realData.data[32]->clear();
    offi_realData.data[33]->clear();
    offi_realData.data[34]->clear();
    offi_realData.data[35]->clear();
    offi_realData.data[36]->clear();
    offi_realData.data[37]->clear();
    offi_realData.data[38]->clear();
    offi_realData.data[39]->clear();
    offi_realData.data[40]->clear();
    offi_realData.data[41]->clear();
    offi_realData.data[42]->clear();
    offi_realData.data[43]->clear();
    offi_realData.data[44]->clear();
    offi_realData.data[45]->clear();
    offi_realData.data[46]->clear();
    offi_realData.data[47]->clear();
    offi_realData.data[48]->clear();
    offi_realData.data[49]->clear();
    offi_realData.data[50]->clear();
    offi_realData.data[51]->clear();
    offi_realData.data[52]->clear();
    offi_realData.data[53]->clear();
    offi_realData.data[54]->clear();
    offi_realData.data[55]->clear();
    offi_realData.data[56]->clear();
    offi_realData.data[57]->clear();
    offi_realData.data[58]->clear();
    offi_realData.data[59]->clear();
    offi_realData.data[60]->clear();
    offi_realData.data[61]->clear();
    offi_realData.data[62]->clear();
    offi_realData.data[63]->clear();

    offi_dataFile.newData[0]->clear();
    offi_dataFile.newData[1]->clear();
    offi_dataFile.newData[2]->clear();
    offi_dataFile.newData[3]->clear();
    offi_dataFile.newData[4]->clear();
    offi_dataFile.newData[5]->clear();
    offi_dataFile.newData[6]->clear();
    offi_dataFile.newData[7]->clear();
    offi_dataFile.newData[8]->clear();
    offi_dataFile.newData[9]->clear();
    offi_dataFile.newData[10]->clear();
    offi_dataFile.newData[11]->clear();
    offi_dataFile.newData[12]->clear();
    offi_dataFile.newData[13]->clear();
    offi_dataFile.newData[14]->clear();
    offi_dataFile.newData[15]->clear();
    offi_dataFile.newData[16]->clear();
    offi_dataFile.newData[17]->clear();
    offi_dataFile.newData[18]->clear();
    offi_dataFile.newData[19]->clear();
    offi_dataFile.newData[20]->clear();
    offi_dataFile.newData[21]->clear();
    offi_dataFile.newData[22]->clear();
    offi_dataFile.newData[23]->clear();
    offi_dataFile.newData[24]->clear();
    offi_dataFile.newData[25]->clear();
    offi_dataFile.newData[26]->clear();
    offi_dataFile.newData[27]->clear();
    offi_dataFile.newData[28]->clear();
    offi_dataFile.newData[29]->clear();
    offi_dataFile.newData[30]->clear();
    offi_dataFile.newData[31]->clear();
    offi_dataFile.newData[32]->clear();
    offi_dataFile.newData[33]->clear();
    offi_dataFile.newData[34]->clear();
    offi_dataFile.newData[35]->clear();
    offi_dataFile.newData[36]->clear();
    offi_dataFile.newData[37]->clear();
    offi_dataFile.newData[38]->clear();
    offi_dataFile.newData[39]->clear();
    offi_dataFile.newData[40]->clear();
    offi_dataFile.newData[41]->clear();
    offi_dataFile.newData[42]->clear();
    offi_dataFile.newData[43]->clear();
    offi_dataFile.newData[44]->clear();
    offi_dataFile.newData[45]->clear();
    offi_dataFile.newData[46]->clear();
    offi_dataFile.newData[47]->clear();
    offi_dataFile.newData[48]->clear();
    offi_dataFile.newData[49]->clear();
    offi_dataFile.newData[50]->clear();
    offi_dataFile.newData[51]->clear();
    offi_dataFile.newData[52]->clear();
    offi_dataFile.newData[53]->clear();
    offi_dataFile.newData[54]->clear();
    offi_dataFile.newData[55]->clear();
    offi_dataFile.newData[56]->clear();
    offi_dataFile.newData[57]->clear();
    offi_dataFile.newData[58]->clear();
    offi_dataFile.newData[59]->clear();
    offi_dataFile.newData[60]->clear();
    offi_dataFile.newData[61]->clear();
    offi_dataFile.newData[62]->clear();
    offi_dataFile.newData[63]->clear();


    for(int dataCur = 0; dataCur <= INTERFACE_FREQUENCY_FFTW[2]; dataCur ++)
        offi_frequencyChart.data[0]->replace(dataCur,dataCur,0);
    for(int dataCur = 0; dataCur <= INTERFACE_FREQUENCY_FFTW[2]; dataCur ++)
        offi_frequencyChart.data[1]->replace(dataCur,dataCur,0);
    for(int dataCur = 0; dataCur <= INTERFACE_FREQUENCY_FFTW[2]; dataCur ++)
        offi_frequencyChart.data[2]->replace(dataCur,dataCur,0);
    for(int dataCur = 0; dataCur <= INTERFACE_FREQUENCY_FFTW[2]; dataCur ++)
        offi_frequencyChart.data[3]->replace(dataCur,dataCur,0);
    for(int dataCur = 0; dataCur <= INTERFACE_FREQUENCY_FFTW[2]; dataCur ++)
        offi_frequencyChart.data[4]->replace(dataCur,dataCur,0);
    for(int dataCur = 0; dataCur <= INTERFACE_FREQUENCY_FFTW[2]; dataCur ++)
        offi_frequencyChart.data[5]->replace(dataCur,dataCur,0);
    for(int dataCur = 0; dataCur <= INTERFACE_FREQUENCY_FFTW[2]; dataCur ++)
        offi_frequencyChart.data[6]->replace(dataCur,dataCur,0);
    for(int dataCur = 0; dataCur <= INTERFACE_FREQUENCY_FFTW[2]; dataCur ++)
        offi_frequencyChart.data[7]->replace(dataCur,dataCur,0);

    offi_slave->setEnabled(true);
    if(offi_slave->CurrentIndex() == 0){
        offi_offlinePathChoice->setEnabled(true);
        offi_offlineFilePathChoice->setEnabled(true);
    }else{
        offi_offlinePathChoice->setEnabled(false);
        offi_offlineFilePathChoice->setEnabled(false);
    }
}

void OfflineInterface::UpdateRMSAndColorCard(){
    QMutexLocker locker(&(offi_realData.mutex));
    offi_RMSAndColorCard.pos = offi_realData.data[0]->count() - 1;
    if(offi_RMSAndColorCard.pos < 0)
        return;
    offi_RMSAndColorCard.valueList[0] = offi_realData.data[0]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][0];
    offi_RMSAndColorCard.valueList[1] = offi_realData.data[1]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][1];
    offi_RMSAndColorCard.valueList[2] = offi_realData.data[2]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][2];
    offi_RMSAndColorCard.valueList[3] = offi_realData.data[3]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][3];
    offi_RMSAndColorCard.valueList[4] = offi_realData.data[4]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][4];
    offi_RMSAndColorCard.valueList[5] = offi_realData.data[5]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][5];
    offi_RMSAndColorCard.valueList[6] = offi_realData.data[6]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][6];
    offi_RMSAndColorCard.valueList[7] = offi_realData.data[7]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][7];
    offi_RMSAndColorCard.valueList[8] = offi_realData.data[8]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][8];
    offi_RMSAndColorCard.valueList[9] = offi_realData.data[9]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][9];
    offi_RMSAndColorCard.valueList[10] = offi_realData.data[10]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][10];
    offi_RMSAndColorCard.valueList[11] = offi_realData.data[11]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][11];
    offi_RMSAndColorCard.valueList[12] = offi_realData.data[12]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][12];
    offi_RMSAndColorCard.valueList[13] = offi_realData.data[13]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][13];
    offi_RMSAndColorCard.valueList[14] = offi_realData.data[14]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][14];
    offi_RMSAndColorCard.valueList[15] = offi_realData.data[15]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][15];
    offi_RMSAndColorCard.valueList[16] = offi_realData.data[16]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][0];
    offi_RMSAndColorCard.valueList[17] = offi_realData.data[17]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][1];
    offi_RMSAndColorCard.valueList[18] = offi_realData.data[18]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][2];
    offi_RMSAndColorCard.valueList[19] = offi_realData.data[19]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][3];
    offi_RMSAndColorCard.valueList[20] = offi_realData.data[20]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][4];
    offi_RMSAndColorCard.valueList[21] = offi_realData.data[21]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][5];
    offi_RMSAndColorCard.valueList[22] = offi_realData.data[22]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][6];
    offi_RMSAndColorCard.valueList[23] = offi_realData.data[23]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][7];
    offi_RMSAndColorCard.valueList[24] = offi_realData.data[24]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][8];
    offi_RMSAndColorCard.valueList[25] = offi_realData.data[25]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][9];
    offi_RMSAndColorCard.valueList[26] = offi_realData.data[26]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][10];
    offi_RMSAndColorCard.valueList[27] = offi_realData.data[27]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][11];
    offi_RMSAndColorCard.valueList[28] = offi_realData.data[28]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][12];
    offi_RMSAndColorCard.valueList[29] = offi_realData.data[29]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][13];
    offi_RMSAndColorCard.valueList[30] = offi_realData.data[30]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][14];
    offi_RMSAndColorCard.valueList[31] = offi_realData.data[31]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][15];
    offi_RMSAndColorCard.valueList[32] = offi_realData.data[32]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][0];
    offi_RMSAndColorCard.valueList[33] = offi_realData.data[33]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][1];
    offi_RMSAndColorCard.valueList[34] = offi_realData.data[34]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][2];
    offi_RMSAndColorCard.valueList[35] = offi_realData.data[35]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][3];
    offi_RMSAndColorCard.valueList[36] = offi_realData.data[36]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][4];
    offi_RMSAndColorCard.valueList[37] = offi_realData.data[37]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][5];
    offi_RMSAndColorCard.valueList[38] = offi_realData.data[38]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][6];
    offi_RMSAndColorCard.valueList[39] = offi_realData.data[39]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][7];
    offi_RMSAndColorCard.valueList[40] = offi_realData.data[40]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][8];
    offi_RMSAndColorCard.valueList[41] = offi_realData.data[41]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][9];
    offi_RMSAndColorCard.valueList[42] = offi_realData.data[42]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][10];
    offi_RMSAndColorCard.valueList[43] = offi_realData.data[43]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][11];
    offi_RMSAndColorCard.valueList[44] = offi_realData.data[44]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][12];
    offi_RMSAndColorCard.valueList[45] = offi_realData.data[45]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][13];
    offi_RMSAndColorCard.valueList[46] = offi_realData.data[46]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][14];
    offi_RMSAndColorCard.valueList[47] = offi_realData.data[47]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][15];
    offi_RMSAndColorCard.valueList[48] = offi_realData.data[48]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][0];
    offi_RMSAndColorCard.valueList[49] = offi_realData.data[49]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][1];
    offi_RMSAndColorCard.valueList[50] = offi_realData.data[50]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][2];
    offi_RMSAndColorCard.valueList[51] = offi_realData.data[51]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][3];
    offi_RMSAndColorCard.valueList[52] = offi_realData.data[52]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][4];
    offi_RMSAndColorCard.valueList[53] = offi_realData.data[53]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][5];
    offi_RMSAndColorCard.valueList[54] = offi_realData.data[54]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][6];
    offi_RMSAndColorCard.valueList[55] = offi_realData.data[55]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][7];
    offi_RMSAndColorCard.valueList[56] = offi_realData.data[56]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][8];
    offi_RMSAndColorCard.valueList[57] = offi_realData.data[57]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][9];
    offi_RMSAndColorCard.valueList[58] = offi_realData.data[58]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][10];
    offi_RMSAndColorCard.valueList[59] = offi_realData.data[59]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][11];
    offi_RMSAndColorCard.valueList[60] = offi_realData.data[60]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][12];
    offi_RMSAndColorCard.valueList[61] = offi_realData.data[61]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][13];
    offi_RMSAndColorCard.valueList[62] = offi_realData.data[62]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][14];
    offi_RMSAndColorCard.valueList[63] = offi_realData.data[63]->at(offi_RMSAndColorCard.pos).y() - INTERFACE_RANGE_BASE_VALUE[offi_rangeBase][15];

    offi_colorCard->DoUpdate(offi_RMSAndColorCard.valueList);
    offi_chart[0].rmsGroup->DoUpdate(offi_RMSAndColorCard.valueList);
    offi_chart[1].rmsGroup->DoUpdate(offi_RMSAndColorCard.valueList+16);
    offi_chart[2].rmsGroup->DoUpdate(offi_RMSAndColorCard.valueList+32);
    offi_chart[3].rmsGroup->DoUpdate(offi_RMSAndColorCard.valueList+48);
}

void OfflineInterface::RecordLog(QString message){
    offi_log->append(QDateTime::currentDateTime().toString("[yyyy.MM.dd-hh:mm:ss] ") + message);
}
