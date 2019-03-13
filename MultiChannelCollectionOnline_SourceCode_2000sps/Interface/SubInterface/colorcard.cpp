#include "colorcard.h"


volatile bool ColorCard::GEAR_TABLE_INIT_FLAG = false;
ColorCard::GEAR_ID ColorCard::GEAR_TABLE[5] = {
    {-20,20,QPalette()},
    {-100,100,QPalette()},
    {-5000,5000,QPalette()},
    {-10000,10000,QPalette()},
    {-10001,10001,QPalette()}
};

ColorCard::ColorCard(QWidget *parent) : QVBoxLayout(parent){
    if(!GEAR_TABLE_INIT_FLAG){
        GEAR_TABLE_INIT_FLAG = true;
        GEAR_TABLE[0].palette.setColor(QPalette::Background,QColor(255,255,255,255));
        GEAR_TABLE[1].palette.setColor(QPalette::Background,QColor(255,0,0,30));
        GEAR_TABLE[2].palette.setColor(QPalette::Background,QColor(255,0,0,90));
        GEAR_TABLE[3].palette.setColor(QPalette::Background,QColor(255,0,0,150));
        GEAR_TABLE[4].palette.setColor(QPalette::Background,QColor(255,0,0,255));
    }
    for(int i = 0; i < 8; i++){
        cc_layout[i] = new QHBoxLayout();
        cc_layout[i]->setContentsMargins(7,0,8,0);
        cc_layout[i]->setSpacing(0);
        addLayout(cc_layout[i]);
    }
    for(int channel = 0; channel < DATA_CHANNEL_COUNT; channel++){
        if(channel < 9)
            cc_item[channel] = new QLabel(QString("0") + QString::number(channel+1));
        else
            cc_item[channel] = new QLabel(QString::number(channel+1));
        cc_item[channel]->setAlignment(Qt::AlignCenter);
        cc_item[channel]->setAutoFillBackground(true);
        cc_item[channel]->setPalette(GEAR_TABLE[0].palette);
        cc_layout[channel/8]->addWidget(cc_item[channel]);
    }

    cc_constratCard[0] = new QCheckBox("0-20");
    cc_constratCard[0]->setEnabled(false);
    cc_constratCard[0]->setStyleSheet("QCheckBox::indicator {width: 7px; height: 7px; }\
                                      QCheckBox::indicator:unchecked{background-color:rgb(255,255,255,255)}");
    cc_constratCard[1] = new QCheckBox("20-100");
    cc_constratCard[1]->setEnabled(false);
    cc_constratCard[1]->setStyleSheet("QCheckBox::indicator {width: 7px; height: 7px; }\
                                      QCheckBox::indicator:unchecked{background-color:rgb(255,0,0,30)}");
    cc_constratCard[2] = new QCheckBox("100-5000");
    cc_constratCard[2]->setEnabled(false);
    cc_constratCard[2]->setStyleSheet("QCheckBox::indicator {width: 7px; height: 7px; }\
                                      QCheckBox::indicator:unchecked{background-color:rgb(255,0,0,90)}");
    cc_constratCard[3] = new QCheckBox("5000-10000");
    cc_constratCard[3]->setEnabled(false);
    cc_constratCard[3]->setStyleSheet("QCheckBox::indicator {width: 7px; height: 7px; }\
                                      QCheckBox::indicator:unchecked{background-color:rgb(255,0,0,150)}");
    cc_constratCard[4] = new QCheckBox("10000+");
    cc_constratCard[4]->setEnabled(false);
    cc_constratCard[4]->setStyleSheet("QCheckBox::indicator {width: 7px; height: 7px; }\
                                      QCheckBox::indicator:unchecked{background-color:rgb(255,0,0,255)}");
    cc_contrastLayout = new QHBoxLayout();
    cc_contrastLayout->setContentsMargins(30,5,30,5);
    cc_contrastLayout->addWidget(cc_constratCard[0]);
    cc_contrastLayout->addWidget(cc_constratCard[1]);
    cc_contrastLayout->addWidget(cc_constratCard[2]);
    cc_contrastLayout->addWidget(cc_constratCard[3]);
    cc_contrastLayout->addWidget(cc_constratCard[4]);
    addLayout(cc_contrastLayout);

    setSpacing(0);
    setContentsMargins(0,0,0,0);
}

ColorCard::~ColorCard(){
    for(int channel = 0; channel < DATA_CHANNEL_COUNT; channel++)
        delete cc_item[channel];
    delete cc_constratCard[0];
    delete cc_constratCard[1];
    delete cc_constratCard[2];
    delete cc_constratCard[3];
    delete cc_constratCard[4];
    delete cc_contrastLayout;
    for(int i = 0; i < 8; i++)
        delete cc_layout[i];
}

void ColorCard::DoUpdate(qreal *valueList){
    for(cc_channel = 0; cc_channel < DATA_CHANNEL_COUNT; cc_channel++){
        if(valueList[cc_channel] > GEAR_TABLE[0].gearValueMin && valueList[cc_channel] < GEAR_TABLE[0].gearValueMax){
            cc_item[cc_channel]->setPalette(GEAR_TABLE[0].palette);
        }else if(valueList[cc_channel] > GEAR_TABLE[1].gearValueMin && valueList[cc_channel] < GEAR_TABLE[1].gearValueMax){
            cc_item[cc_channel]->setPalette(GEAR_TABLE[1].palette);
        }else if(valueList[cc_channel] > GEAR_TABLE[2].gearValueMin && valueList[cc_channel] < GEAR_TABLE[2].gearValueMax){
            cc_item[cc_channel]->setPalette(GEAR_TABLE[2].palette);
        }else if(valueList[cc_channel] > GEAR_TABLE[3].gearValueMin && valueList[cc_channel] < GEAR_TABLE[3].gearValueMax){
            cc_item[cc_channel]->setPalette(GEAR_TABLE[3].palette);
        }else{
            cc_item[cc_channel]->setPalette(GEAR_TABLE[4].palette);
        }
    }
}


void ColorCard::DoReset(){
    for(int channel = 0; channel < DATA_CHANNEL_COUNT; channel++)
        cc_item[channel]->setPalette(GEAR_TABLE[0].palette);
}
