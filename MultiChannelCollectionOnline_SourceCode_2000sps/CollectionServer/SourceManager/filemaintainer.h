#ifndef FILEMAINTAINER_H
#define FILEMAINTAINER_H

#include <QObject>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <DataProcessor/messagequeue.h>
class SourceManager;

class FileMaintainer : public QObject
{
    Q_OBJECT
    friend class SourceManager;
    protected:
        FileMaintainer(int flag,SourceManager *manager,QObject *parent = nullptr);
    public:
        ~FileMaintainer();

    signals:

    public slots:


    public:
        inline int GetFlag(){return fm_flag;}
        void AcceptSlaveInit(QString path,QString guid);
        QString GetGUID(){return fm_curGuid;}
        void DoStoreCollection(const char *content);
        void DoStoreB(const QString content);
        void DoStoreC(const QString content);
        void DoStoreIP(const QString content);
        void DoStoreW(const QString content);
        void DoStoreLog(const QString content);
        void LoseSlaveClean();

    private:
        //标识，最大支持64路下位机
        int fm_flag;
        //管理者句柄
        SourceManager *fm_SM;
        QString fm_filePathDir;

         QString F50HZ;
         QString F1_3HZ;
         QString F4_30HZ;
         QString F4_30_S50HZ;
         QString F1_90_S50HZ;
         QString F4_90_S50HZ;
        //文件信息
        struct{
            unsigned int seq;       //文件序列号
            unsigned int lessCount; //剩下可写入的数据个数
            QFile *handle;          //当前文件句柄
            QMutex mutex;
        } fm_storeInfo;
        QString fm_curGuid;
        QMutex fm_confiMutex;
        int filter_Flag;  //滤波标志位
        int filterChange;  //临时变量
        int tempFlag;





};

#endif // FILEMAINTAINER_H
