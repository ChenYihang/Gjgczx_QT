#include "sourcemanager.h"
#include <time.h>
#include <QTime>
#include <fcntl.h>

SourceManager *SourceManager::sm_SM = 0;
SourceManager::SourceManager(QObject *parent) : QObject(parent){
    sm_MQ = MessageQueue::GetInstance();
    for(int slave = 0; slave<MAX_SLAVE_COMPUTER_COUNT; slave++){
        sm_clientHandle[slave] = new SocketClient(slave,this);
        sm_fileHandle[slave] = new FileMaintainer(slave,this);
    }

    sm_IC = new InterfaceConnect(this);
    sm_IC->start();

    sm_curFlag = -1;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    sm_dataPath = "./";
}

SourceManager::~SourceManager(){
    delete sm_IC;

    for(int slave = 0; slave<MAX_SLAVE_COMPUTER_COUNT; slave++){
        delete sm_clientHandle[slave];
        delete sm_fileHandle[slave];
    }
}

SourceManager *SourceManager::GetInstance(){
    if(!sm_SM){
        sm_SM = new SourceManager();
         qDebug()<<"init sourceManager" ;
    }
    return sm_SM;
}

void SourceManager::AcceptNewConnect(QTcpSocket* newConnect){
    int flag = 0;
    for(; flag < MAX_SLAVE_COMPUTER_COUNT; flag++){
        if(sm_clientHandle[flag]->Hold(newConnect)){
            sm_fileHandle[flag]->AcceptSlaveInit(sm_dataPath,MakeGuid());
            sm_fileHandle[flag]->DoStoreIP(sm_clientHandle[flag]->GetIP());
            QByteArray content = QByteArray(SLAVE_CONNECT) + sm_fileHandle[flag]->GetGUID().toLatin1() + sm_clientHandle[flag]->GetIP().toLatin1();
            sm_IC->WriteMessage(content.data(),content.length());
            break;
        }
    }
    if(flag >= MAX_SLAVE_COMPUTER_COUNT){
        newConnect->disconnect();
        newConnect->waitForDisconnected(0);
        newConnect->close();
    }
}

void SourceManager::LoseConnect(int flag){
    QByteArray content = QByteArray(SLAVE_DISCONNECT) + sm_fileHandle[flag]->GetGUID().toLatin1();
    sm_IC->WriteMessage(content.data(),content.length());
    sm_fileHandle[flag]->LoseSlaveClean();
}

void SourceManager::InData(MSG_UNIT *msg){
    QMutexLocker locker(&(datain_mutex));
  // qDebug("%d po%x",(int)QThread::currentThreadId(),msg->content[5]);
  // qDebug("po%x",msg->content[5]);
    switch (msg->type) {
        case LINK_MESSAGE_TYPE::UPLINK_MESSAGE_TYPE_COLLECTION:{
            sm_fileHandle[msg->flag]->DoStoreCollection(msg->content);
            break;
        }
        case LINK_MESSAGE_TYPE::UPLINK_MESSAGE_TYPE_B:{
            QString tep = QString::number(float(((msg->content[3]&0x00FF)<<16)|((msg->content[4]&0x00FF)<<8)|(msg->content[5]&0x00FF))*4.5/8388608,'f',2);
            sm_fileHandle[msg->flag]->DoStoreB(tep);
            QByteArray content = QByteArray(SLAVE_UPDATE) +
                    QByteArray(SLAVE_UPD_VTG) +
                    sm_fileHandle[msg->flag]->GetGUID().toLatin1() +
                    tep.toLatin1();
            sm_IC->WriteMessage(content.data(),content.length());
            break;
        }
        case LINK_MESSAGE_TYPE::UPLINK_MESSAGE_TYPE_C:{
            QString vtg = QString::number(((float(((msg->content[3]&0x00FF)<<16)|((msg->content[4]&0x00FF)<<8)|(msg->content[5]&0x00FF))*4.5/8388608*1000000)-145300)/490+25,'f',2);
            sm_fileHandle[msg->flag]->DoStoreC(vtg);
            QByteArray content = QByteArray(SLAVE_UPDATE) +
                    QByteArray(SLAVE_UPD_TEP) +
                    sm_fileHandle[msg->flag]->GetGUID().toLatin1() +
                    vtg.toLatin1();
            sm_IC->WriteMessage(content.data(),content.length());
            break;
        }
        case LINK_MESSAGE_TYPE::UPLINK_MESSAGE_TYPE_W:{
            sm_fileHandle[msg->flag]->DoStoreW(
                        QString::fromUtf8(msg->content + 3,6));
            QByteArray content = QByteArray(SLAVE_UPDATE) +
                    QByteArray(SLAVE_UPD_IDF) +
                    sm_fileHandle[msg->flag]->GetGUID().toLatin1() +
                    QString::fromUtf8(msg->content + 3,6).toLatin1();
            sm_IC->WriteMessage(content.data(),content.length());
            break;
        }
        case LINK_MESSAGE_TYPE::UPLINK_MESSAGE_TYPE_F:{
            QString log = QDateTime::currentDateTime().toString("[yyyy.MM.dd-hh:mm:ss] ") +
                    QString::fromUtf8(msg->content + 3,16);
            sm_fileHandle[msg->flag]->DoStoreLog(log);
            QByteArray content = QByteArray(SLAVE_UPDATE) +
                    QByteArray(SLAVE_UPD_LOG) +
                    sm_fileHandle[msg->flag]->GetGUID().toLatin1() +
                    log.toLatin1();
            sm_IC->WriteMessage(content.data(),content.length());
            break;
        }
        default:

            break;
    }
}


void SourceManager::ReceiveCommandFromInterface(int command,QByteArray content){
    static QByteArray RESET_DATA            = QByteArray::fromHex("52535452455345545245534554545352");
    static QByteArray COLLECTION_START_DATA = QByteArray::fromHex("414351414351FFFFFFFFFFFFFF514341");
    static QByteArray COLLECTION_STOP_DATA  = QByteArray::fromHex("41435141435100000000000000514341");
    static QByteArray STIMULATE_START_DATA  = QByteArray::fromHex("53544C53544CFFFFFFFFFFFFFF4C5453");
    static QByteArray STIMULATE_STOP_DATA   = QByteArray::fromHex("53544C53544C000000000000004C5453");
    static QByteArray DOWN_DATA_FILE_DATA   = QByteArray("444143%020X434144");
//    static QString RESET_MSG                    = "执行复位操作";
//    static QString COLLECTION_START_MSG         = "开始采集";
//    static QString COLLECTION_STOP_MSG          = "停止采集";
//    static QString STIMULATE_STAET_MSG          = "开始刺激";
//    static QString STIMULATE_STOP_MSG           = "停止刺激";
//    static QString DOWN_DATA_TEXT_MSG           = "下发文本数据";
//    static QString DOWN_DATA_TEXT_NULL_MSG      = "下发内容为空";
//    static QString DOWN_DATA_FILE_MSG           = "下发文件数据";
//    static QString DOWN_DATA_FILE_INVALID_MSG   = "打开文件失败";
//    static QString INVALID_MSG                  = "无效操作";
    static QString RESET_MSG                    = "RESET_MSG";
    static QString COLLECTION_START_MSG         = "COLLECTION_START_MSG";
    static QString COLLECTION_STOP_MSG          = "COLLECTION_STOP_MSG";
    static QString STIMULATE_START_MSG          = "STIMULATE_STAET_MSG";
    static QString STIMULATE_STOP_MSG           = "STIMULATE_STOP_MSG";
    static QString DOWN_DATA_TEXT_MSG           = "DOWN_DATA_TEXT_MSG";
    static QString DOWN_DATA_TEXT_NULL_MSG      = "DOWN_DATA_TEXT_NULL_MSG";
    static QString DOWN_DATA_FILE_MSG           = "DOWN_DATA_FILE_MSG";
    static QString DOWN_DATA_FILE_INVALID_MSG   = "DOWN_DATA_FILE_INVALID_MSG";
    static QString INVALID_MSG                  = "INVALID_MSG";
    static QString FILTER_MSG                  = "FILTER";

//    QByteArray msg = QByteArray(SLAVE_UPDATE) +
//            QByteArray(SLAVE_UPD_LOG) +
//            content + QString::number(command).toLatin1();
//    sm_IC->WriteMessage(msg.data(),msg.length());

    QString guid = QString(content).mid(0,36);
    int curFlag = 0;
    for(; curFlag < MAX_SLAVE_COMPUTER_COUNT; curFlag ++){
        if(sm_fileHandle[curFlag]->GetGUID() == guid)
            break;
    }
    if(curFlag >= MAX_SLAVE_COMPUTER_COUNT)
        return;
    switch(command){
        case RESET_OPERATE:{
            sm_clientHandle[curFlag]->SendData(RESET_DATA);
//            QString message = QDateTime::currentDateTime().toString("[yyyy.MM.dd-hh:mm:ss] ") + RESET_MSG;
//            sm_fileHandle[curFlag]->DoStoreLog(message);
//            QByteArray responseMsg = QByteArray(SLAVE_UPDATE) +
//                    QByteArray(SLAVE_UPD_LOG) +
//                    guid.toLatin1() +
//                    message.toLatin1();
//            sm_IC->WriteMessage(responseMsg.data(),responseMsg.length());
            break;
        }
        case COLLECTION_START:{
            sm_clientHandle[curFlag]->SendData(COLLECTION_START_DATA);
//            QString message = QDateTime::currentDateTime().toString("[yyyy.MM.dd-hh:mm:ss] ") + COLLECTION_START_MSG;
//            sm_fileHandle[curFlag]->DoStoreLog(message);
//            QByteArray responseMsg = QByteArray(SLAVE_UPDATE) +
//                    QByteArray(SLAVE_UPD_LOG) +
//                    guid.toLatin1() +
//                    message.toLatin1();
//            sm_IC->WriteMessage(responseMsg.data(),responseMsg.length());
            break;
        }
        case COLLECTION_STOP:{
            sm_clientHandle[curFlag]->SendData(COLLECTION_STOP_DATA);
//            QString message = QDateTime::currentDateTime().toString("[yyyy.MM.dd-hh:mm:ss] ") + COLLECTION_STOP_MSG;
//            sm_fileHandle[curFlag]->DoStoreLog(message);
//            QByteArray responseMsg = QByteArray(SLAVE_UPDATE) +
//                    QByteArray(SLAVE_UPD_LOG) +
//                    guid.toLatin1() +
//                    message.toLatin1();
//            sm_IC->WriteMessage(responseMsg.data(),responseMsg.length());
            break;
        }
        case STIMULATE_STAET:{
            sm_clientHandle[curFlag]->SendData(STIMULATE_START_DATA);
//            QString message = QDateTime::currentDateTime().toString("[yyyy.MM.dd-hh:mm:ss] ") + STIMULATE_START_MSG;
//            sm_fileHandle[curFlag]->DoStoreLog(message);
//            QByteArray responseMsg = QByteArray(SLAVE_UPDATE) +
//                    QByteArray(SLAVE_UPD_LOG) +
//                    guid.toLatin1() +
//                    message.toLatin1();
//            sm_IC->WriteMessage(responseMsg.data(),responseMsg.length());
            break;
        }
        case STIMULATE_STOP:{
            sm_clientHandle[curFlag]->SendData(STIMULATE_STOP_DATA);
//            QString message = QDateTime::currentDateTime().toString("[yyyy.MM.dd-hh:mm:ss] ") + STIMULATE_STOP_MSG;
//            sm_fileHandle[curFlag]->DoStoreLog(message);
//            QByteArray responseMsg = QByteArray(SLAVE_UPDATE) +
//                    QByteArray(SLAVE_UPD_LOG) +
//                    guid.toLatin1() +
//                    message.toLatin1();
//            sm_IC->WriteMessage(responseMsg.data(),responseMsg.length());
            break;
        }
        case DOWN_DATA_TEXT:{
            QByteArray info = QByteArray::fromHex(content.mid(36));
            if(info == ""){
//                QString message = QDateTime::currentDateTime().toString("[yyyy.MM.dd-hh:mm:ss] ") + DOWN_DATA_TEXT_NULL_MSG;
//                sm_fileHandle[curFlag]->DoStoreLog(message);
//                QByteArray responseMsg = QByteArray(SLAVE_UPDATE) +
//                        QByteArray(SLAVE_UPD_LOG) +
//                        guid.toLatin1() +
//                        message.toLatin1();
//                sm_IC->WriteMessage(responseMsg.data(),responseMsg.length());
                return;
            }
            sm_clientHandle[curFlag]->SendData(info);
//            QString message = QDateTime::currentDateTime().toString("[yyyy.MM.dd-hh:mm:ss] ") + DOWN_DATA_TEXT_MSG;
//            sm_fileHandle[curFlag]->DoStoreLog(message);
//            QByteArray responseMsg = QByteArray(SLAVE_UPDATE) +
//                    QByteArray(SLAVE_UPD_LOG) +
//                    guid.toLatin1() +
//                    message.toLatin1();
//            sm_IC->WriteMessage(responseMsg.data(),responseMsg.length());
            break;
        }
        case DOWN_DATA_FILE:{
            QByteArray info = content.mid(36);
            QFile file(info);
            if(info == "" || !file.open(QIODevice::ReadOnly)){
//                QString message = QDateTime::currentDateTime().toString("[yyyy.MM.dd-hh:mm:ss] ") + DOWN_DATA_FILE_INVALID_MSG + QString(info);
//                sm_fileHandle[curFlag]->DoStoreLog(message);
//                QByteArray responseMsg = QByteArray(SLAVE_UPDATE) +
//                        QByteArray(SLAVE_UPD_LOG) +
//                        guid.toLatin1() +
//                        message.toLatin1();
//                sm_IC->WriteMessage(responseMsg.data(),responseMsg.length());
                return;
            }

            QByteArray content = QByteArray::fromHex(file.readAll());
            QByteArray suffixMsg = QByteArray::fromHex(QString::asprintf(DOWN_DATA_FILE_DATA,content.size()).toLatin1());
            sm_clientHandle[curFlag]->SendData(suffixMsg);
            sm_clientHandle[curFlag]->SendData(content);
//            QString message = QDateTime::currentDateTime().toString("[yyyy.MM.dd-hh:mm:ss] ") + DOWN_DATA_FILE_MSG + QString(info);
//            sm_fileHandle[curFlag]->DoStoreLog(message);
//            QByteArray responseMsg = QByteArray(SLAVE_UPDATE) +
//                    QByteArray(SLAVE_UPD_LOG) +
//                    guid.toLatin1() +
//                    message.toLatin1();
//            sm_IC->WriteMessage(responseMsg.data(),responseMsg.length());
            break;
        }
        case TAB_CLOSED:{
            sm_clientHandle[curFlag]->DoSocketDisconnect();

            break;
        }

        case FILTER:{

        QByteArray info = content.mid(36);
        int temp =info.toInt();
        //qDebug()<<i;
        sm_fileHandle[curFlag]->filter_Flag = temp;

        sm_fileHandle[curFlag]->filterChange = 1;
            sm_IC->WriteMessage(info.data(),1);


         }
        default:{
//            QString message = QDateTime::currentDateTime().toString("[yyyy.MM.dd-hh:mm:ss] ") + INVALID_MSG.toUtf8();
//            sm_fileHandle[curFlag]->DoStoreLog(message);
//            QByteArray content = QByteArray(SLAVE_UPDATE) +
//                    QByteArray(SLAVE_UPD_LOG) +
//                    guid.toLatin1() +
//                    message.toLatin1();
//            sm_IC->WriteMessage(content.data(),content.length());
        }

    }
}

QString SourceManager::MakeGuid(){
    return QString::asprintf("%04X%04X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                       qrand()%0xFFFF,qrand()%0xFFFF,qrand()%0xFFFF,qrand()%0xFFFF,qrand()%0xFF,qrand()%0xFF,
                       qrand()%0xFF,qrand()%0xFF,qrand()%0xFF,qrand()%0xFF,qrand()%0xFF,qrand()%0xFF);
}
void SourceManager::SetDataPath(QString path){
    if(path.endsWith("/"))
        sm_dataPath = path;
    else
        sm_dataPath = path + "/";
}
