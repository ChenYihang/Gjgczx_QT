#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <../Common/common.h>
//#include <SourceManager/sourcemanager.h>

class MessageQueue
{
    private:
        #if HIGH_PERFORMANCE
        const static int MQ_MESSAGE_COUNT_MAX   = 10000000;
        #else
        const static int MQ_MESSAGE_COUNT_MAX   = 1000000;
        #endif
        const static int LINK_MESSAGE_TYPE_COUNT = 9;
        const static char LINK_MESSAGE_TYPE_CONTENT[LINK_MESSAGE_TYPE_COUNT][3];

    public:
        ~MessageQueue();
    protected:
        MessageQueue();

    public:
        static MessageQueue *GetInstance();
        void Push(MSG_UNIT* msg);
        void Pop(MSG_UNIT* msg);
        bool IsEmpty();
        LINK_MESSAGE_TYPE UplinkDataMatch(char *content);
        bool RightEndCheck(char *content,LINK_MESSAGE_TYPE type);

    private:
        static MessageQueue *mq_MQ;
        pthread_mutex_t mq_mutexProducer;
        pthread_mutex_t mq_mutexConsumer;
        sem_t mq_semProducer;
        sem_t mq_semConsumer;
        MSG_UNIT* mq_cacheBlockHead;
        MSG_UNIT* mq_cacheBlockTail;
        MSG_UNIT* mq_cacheBlockProducer;
        MSG_UNIT* mq_cacheBlockConsumer;
};

#endif // MESSAGEQUEUE_H
