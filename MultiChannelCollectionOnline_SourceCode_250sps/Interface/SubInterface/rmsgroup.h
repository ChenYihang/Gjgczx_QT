#ifndef RMSGROUP_H
#define RMSGROUP_H

#include <QVBoxLayout>
#include <QLabel>
#include <../Common/common.h>

class RMSGroup : public QVBoxLayout
{
    public:
        RMSGroup(QWidget *parent = nullptr);
        ~RMSGroup();
    public:
        void DoUpdate(qreal *valueList);
        void DoReset();
    private:
        const static int GROUP_SIZE = DATA_CHANNEL_COUNT/4;
        QLabel *rmsg_title;
        QLabel *(rmsg_label[GROUP_SIZE]);
        qreal rmsg_curValue[GROUP_SIZE];
        int rmsg_count;
        int rmsg_channel;
        qreal rmsg_temp;
};

#endif // RMSGROUP_H
