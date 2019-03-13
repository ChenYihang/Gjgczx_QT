#ifndef COLORCARD_H
#define COLORCARD_H

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <../Common/common.h>
#include <QPalette>
#include <QCheckBox>


//强度范围划分为5个档：①0~20uV   ②20~100uV   ③100~5000uV   ④5000~10000uV   ⑤>10000uV

class ColorCard : public QVBoxLayout{
    public:
        ColorCard(QWidget *parent = nullptr);
        ~ColorCard();
    public:
        void DoUpdate(qreal *valueList);
        void DoReset();
    private:
        typedef struct GEAR_IDENTIFY
        {
            qreal gearValueMin;
            qreal gearValueMax;
            QPalette palette;
        } GEAR_ID;
        static GEAR_IDENTIFY GEAR_TABLE[5];
        static volatile bool GEAR_TABLE_INIT_FLAG;
        QLabel *(cc_item[DATA_CHANNEL_COUNT]);
        QHBoxLayout *(cc_layout[8]);

        QCheckBox *(cc_constratCard[5]);
        QHBoxLayout *cc_contrastLayout;

        int cc_channel;
};

#endif // COLORCARD_H
