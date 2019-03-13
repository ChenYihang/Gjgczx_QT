#include "rmsgroup.h"
#include <qmath.h>

RMSGroup::RMSGroup(QWidget *parent) : QVBoxLayout(parent){
    QFont ft;
    ft.setPointSize(8);
    rmsg_title = new QLabel("uVrms");
    rmsg_title->setFont(ft);
    rmsg_title->setFixedHeight(20);
    rmsg_title->setAlignment(Qt::AlignRight);
    addWidget(rmsg_title);

    //

    for(int i = GROUP_SIZE - 1; i >= 0 ; i--){
        rmsg_label[i] = new QLabel("0");
        rmsg_label[i]->setFont(ft);
        rmsg_label[i]->setAlignment(Qt::AlignRight);
        addWidget(rmsg_label[i]);
    }
    setAlignment(Qt::AlignRight);
    addSpacing(75);

    for(int channel = 0; channel < GROUP_SIZE; channel++)
        rmsg_curValue[channel] = 0;
    rmsg_count = 0;
}

RMSGroup::~RMSGroup(){
    for(int i = 0; i < GROUP_SIZE; i++)
        delete rmsg_label[i];
}

void RMSGroup::DoUpdate(qreal *valueList){
    for(rmsg_channel = 0; rmsg_channel < GROUP_SIZE; rmsg_channel++){
        rmsg_temp = (rmsg_curValue[rmsg_channel] * rmsg_curValue[rmsg_channel] * rmsg_count);
        rmsg_temp += (valueList[rmsg_channel] * valueList[rmsg_channel]);
        rmsg_temp /= (rmsg_count+1);
        rmsg_curValue[rmsg_channel] = qSqrt(rmsg_temp);
        if(rmsg_curValue[rmsg_channel] > 10000)
            rmsg_label[rmsg_channel]->setText(QString("overload"));
        else
            rmsg_label[rmsg_channel]->setText(QString::number(rmsg_curValue[rmsg_channel],'f',2));
    }
    rmsg_count ++;
}

void RMSGroup::DoReset(){
    for(rmsg_channel = 0; rmsg_channel < GROUP_SIZE; rmsg_channel++){
        rmsg_label[rmsg_channel]->setText("0");
        rmsg_curValue[rmsg_channel] = 0;
        rmsg_count = 0;
    }
}
