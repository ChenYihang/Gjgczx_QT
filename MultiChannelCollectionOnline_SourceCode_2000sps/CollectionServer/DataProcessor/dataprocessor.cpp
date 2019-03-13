#include "dataprocessor.h"

DataProcessor::DataProcessor(){
    dp_MQ.mq = MessageQueue::GetInstance();
    dp_SM = SourceManager::GetInstance();
}
DataProcessor::~DataProcessor(){

}
QMutex *DataProcessor::m_lock=new QMutex();

void DataProcessor::run(){
    while(1){
        m_lock->lock();
        //qDebug()<<QThread::currentThreadId();
        dp_MQ.mq->Pop(&(dp_MQ.message));
        dp_SM->InData(&(dp_MQ.message));
        m_lock->unlock();
    }
}
