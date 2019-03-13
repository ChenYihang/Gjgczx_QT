#include "messagequeue.h"
#include <stdlib.h>
#include <QDebug>

MessageQueue *MessageQueue::mq_MQ = 0;

const char MessageQueue::LINK_MESSAGE_TYPE_CONTENT[MessageQueue::LINK_MESSAGE_TYPE_COUNT][3] = {
            {0x45,0x45,0x47},   //（上行）采集
            {0x53,0x4F,0x43},   //（上行）B
            {0x54,0x45,0x4D},   //（上行）C
            {0x49,0x44,0x4E},   //（上行）W
            {0x46,0x42,0x43},   //（上行）F
            {0x52,0x53,0x54},   //（下行）T复位
            {0x41,0x43,0x51},   //（下行）T采集
            {0x53,0x54,0x4C},   //（下行）T刺激
            {0x44,0x41,0x43}    //（下行）S自定义文件
};

MessageQueue::MessageQueue(){
    pthread_mutex_init(&mq_mutexProducer,0);
    pthread_mutex_init(&mq_mutexConsumer,0);

    sem_init(&mq_semProducer,0,MQ_MESSAGE_COUNT_MAX);
    sem_init(&mq_semConsumer,0,0);

    mq_cacheBlockHead = (MSG_UNIT*)malloc(MQ_MESSAGE_COUNT_MAX * sizeof(MSG_UNIT));
    mq_cacheBlockTail = mq_cacheBlockHead + MQ_MESSAGE_COUNT_MAX - 1;
    mq_cacheBlockConsumer = mq_cacheBlockHead;
    mq_cacheBlockProducer = mq_cacheBlockHead;
}

MessageQueue::~MessageQueue(){
    pthread_mutex_destroy(&mq_mutexProducer);
    pthread_mutex_destroy(&mq_mutexConsumer);

    sem_destroy(&mq_semProducer);
    sem_destroy(&mq_semConsumer);

    free(mq_cacheBlockHead);
}

MessageQueue *MessageQueue::GetInstance(){
    if(!mq_MQ)
        mq_MQ = new MessageQueue();
    return mq_MQ;
}

void MessageQueue::Push(MSG_UNIT* msg){
    sem_wait(&mq_semProducer);
    pthread_mutex_lock(&mq_mutexProducer);
    *mq_cacheBlockProducer = *msg;
    if(mq_cacheBlockProducer == mq_cacheBlockTail)
        mq_cacheBlockProducer = mq_cacheBlockHead;
    else
        mq_cacheBlockProducer ++;

    pthread_mutex_unlock(&mq_mutexProducer);
    sem_post(&mq_semConsumer);
}
void MessageQueue::Pop(MSG_UNIT* msg){
    sem_wait(&mq_semConsumer);
    pthread_mutex_lock(&mq_mutexConsumer);

    *msg = *mq_cacheBlockConsumer;
    if(mq_cacheBlockConsumer == mq_cacheBlockTail)
        mq_cacheBlockConsumer = mq_cacheBlockHead;
    else
        mq_cacheBlockConsumer ++;

    pthread_mutex_unlock(&mq_mutexConsumer);
    sem_post(&mq_semProducer);
}
bool MessageQueue::IsEmpty(){
    return (mq_cacheBlockConsumer == mq_cacheBlockProducer);
}
LINK_MESSAGE_TYPE MessageQueue::UplinkDataMatch(char *content){
    int i = 0;
    for(; i <= UPLINK_MESSAGE_TYPE_F; i++){
        if(content[0] == LINK_MESSAGE_TYPE_CONTENT[i][0] &&
                content[1] == LINK_MESSAGE_TYPE_CONTENT[i][1] &&
                content[2] == LINK_MESSAGE_TYPE_CONTENT[i][2])
            return (LINK_MESSAGE_TYPE)i;
    }
    return LINK_MESSAGE_TYPE_USELESS;
}

bool MessageQueue::RightEndCheck(char *content,LINK_MESSAGE_TYPE type){
    if(LINK_MESSAGE_TYPE_CONTENT[type][0] == content[MQ_MESSAGE_LENGHT-1] &&
            LINK_MESSAGE_TYPE_CONTENT[type][1] == content[MQ_MESSAGE_LENGHT-2] &&
            LINK_MESSAGE_TYPE_CONTENT[type][2] == content[MQ_MESSAGE_LENGHT-3])
        return true;
    else
        return false;
}
