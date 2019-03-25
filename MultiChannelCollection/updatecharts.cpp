#include "updatecharts.h"

updateCharts::updateCharts(QThread  *parent) : QThread (parent)
  ,runState(false),runSpeed(100),MBcount(0)
{
    tWidget = new timeWidget;
    connect(tWidget,&timeWidget::plotState,this,&updateCharts::stateChange);
    connect(tWidget,&timeWidget::plotSpeed,this,&updateCharts::speedChange);

    connect(tWidget,&timeWidget::signalPlayFile,this,&updateCharts::startPlotFile);
    connect(this,&updateCharts::signalCurrentPlayState,tWidget,&timeWidget::updatePlayState);

}

updateCharts::~updateCharts(){
    delete tWidget;
    requestInterruption();
    quit();
    wait();

}


void updateCharts::startPlotFile(QString fName){

    if(fName =="Cancel"){   //取消播放操作
        requestInterruption();
        quit();
        wait();
        MBcount=0;
        emit signalCurrentPlayState("");
        return;

    }
        fileName = fName;
        // fileSize = 1024;
        rFile = new QFile(fileName);
        if(!rFile->isOpen())
        rFile->open(QIODevice::ReadOnly);
        fileSize = rFile->size();
        if (this->isRunning())
        {
            this->wait();  //wait for thread ending
        }
        MBcount=0;
        emit signalCurrentPlayState("Played: "+QString::number(MBcount)+"MB");



        this->start();

}

void updateCharts::stateChange(bool state){

    runState = state;

}

void updateCharts::speedChange(unsigned long speed){

    runSpeed = speed;

}



qreal updateCharts::getValue(QByteArray twoBytes){

    if(twoBytes[0]&0x0020){  //负值  设置为0-2.5
        return (qreal)((((twoBytes[0]&0x001f)<<8)|(twoBytes[1]&0x00ff))/8192.0*2.5);
       //  qDebug()<<(int)(twoBytes[1]&0x001f);
    }
    else{
        return (qreal)((((twoBytes[0]&0x001f)<<8)|(twoBytes[1]&0x00ff))/8192.0*2.5+2.5);


    }
}

//绘图进程
void updateCharts::run(){
    QByteArray dataPack;
    QList<qreal> timeYData[8];
    QList<qreal> *timeP[8] = {timeYData,timeYData+1,timeYData+2,
                             timeYData+3,timeYData+4,timeYData+5,timeYData+6,timeYData+7};


        while(!isInterruptionRequested()){
          if(MBcount<16){
                for(int i=0;i<1024;i++){  //绘制1MB数据
                    while(!runState){//暂停与否
                        if(isInterruptionRequested()){//取消操作
                            rFile->close();
                            return;}
                             }
                    dataPack = rFile->read(1024);
                      for(int j =0;j<64;j++){
                          for(int k=0;k<8;k++)
                             timeYData[k]<<getValue(dataPack.mid(j*16+k*2,2))+k*5;
                                           // ((qreal)(dataPack.mid(j*16+k*2,2).toHex().toInt(&ok,16)/13107.0)+(qreal)(k*5.0));

                      }
                      tWidget->timeChart->updateTimeChart(timeP);
                      msleep(runSpeed);
                        for(int l=0;l<8;l++)
                             timeYData[l].clear();



                    }
                ++MBcount;
                emit signalCurrentPlayState("Played: "+QString::number(MBcount)+"MB");

          }
          else{
                qDebug()<<"read done";
                qDebug()<<rFile->pos();
                rFile->close();
                MBcount=0;

                emit signalCurrentPlayState("Completed.");

                this->requestInterruption();
                this->quit();

          }

        }



}


