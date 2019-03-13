#include "dataprocessor.h"

DataProcessor::DataProcessor(){
    dp_MQ.mq = MessageQueue::GetInstance();
    dp_SM = SourceManager::GetInstance();
}
DataProcessor::~DataProcessor(){

}


QMutex   m_mutex;

void DataProcessor::run(){
    while(1){
        m_mutex.lock();
        //qDebug()<<(int)QThread::currentThreadId();
        dp_MQ.mq->Pop(&(dp_MQ.message));
     qDebug("%d %x",(int)QThread::currentThreadId(),dp_MQ.message.content[5]);

        dp_SM->InData(&(dp_MQ.message));
        m_mutex.unlock();
    }
}
