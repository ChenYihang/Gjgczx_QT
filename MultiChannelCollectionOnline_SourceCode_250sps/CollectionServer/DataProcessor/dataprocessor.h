#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QThread>
#include <QDebug>
#include <DataProcessor/messagequeue.h>
#include <SourceManager/sourcemanager.h>
#include <../Common/common.h>

class DataProcessor : public QThread
{
    public:
        DataProcessor();
        ~DataProcessor();
    protected:
        void run();

    private:
        struct{
            MessageQueue *mq;
            MSG_UNIT message;
        } dp_MQ;

        SourceManager *dp_SM;

};

#endif // DATAPROCESSOR_H
