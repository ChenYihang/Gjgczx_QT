#include <SourceManager/filemaintainer.h>
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include "sourcemanager.h"
FileMaintainer::FileMaintainer(int flag,SourceManager *manager,QObject *parent) : QObject(parent){
    fm_flag = flag;
    fm_SM = manager;
    filter_Flag = 0;
    filterChange = 0;
    tempFlag = 0;
    fm_filePathDir = "";

    fm_storeInfo.handle = 0;
    fm_storeInfo.lessCount = 0;
    fm_storeInfo.seq = 0;
}

FileMaintainer::~FileMaintainer(){
    LoseSlaveClean();
}

void FileMaintainer::AcceptSlaveInit(QString path,QString guid){
    QMutexLocker locker(&(fm_storeInfo.mutex));
    if(guid.size() == 0)
        return;
    fm_curGuid = guid;
    fm_filePathDir = path + guid + "/";
    F50HZ = fm_filePathDir + "Filter_50HZ/";  //滤波数据存放文件夹
    F1_3HZ = fm_filePathDir + "Filter_1_3HZ/";  //滤波数据存放文件夹
    F4_30HZ = fm_filePathDir + "Filter_4_30HZ/";  //滤波数据存放文件夹
    F4_30_S50HZ = fm_filePathDir + "Filter_4_30_S50HZ/";  //滤波数据存放文件夹
    F1_90_S50HZ = fm_filePathDir + "Filter_1_90_S50HZ/";  //滤波数据存放文件夹
    F4_90_S50HZ = fm_filePathDir + "Filter_4_90_S50HZ/";  //滤波数据存放文件夹


    QDir dir(fm_filePathDir);
    if(dir.exists()){
        dir.remove(fm_filePathDir);
    }
    dir.mkdir(fm_filePathDir);

    QDir dir1(F50HZ);
    if(dir1.exists()){
        dir1.remove(F50HZ);
    }
    dir1.mkdir(F50HZ);

    QDir dir2(F1_3HZ);
    if(dir2.exists()){
        dir2.remove(F1_3HZ);
    }
    dir2.mkdir(F1_3HZ);

    QDir dir3(F4_30HZ);
    if(dir3.exists()){
        dir3.remove(F4_30HZ);
    }
    dir3.mkdir(F4_30HZ);

    QDir dir4(F4_30_S50HZ);
    if(dir4.exists()){
        dir4.remove(F4_30_S50HZ);
    }
    dir4.mkdir(F4_30_S50HZ);

    QDir dir5(F1_90_S50HZ);
    if(dir5.exists()){
        dir5.remove(F1_90_S50HZ);
    }
    dir5.mkdir(F1_90_S50HZ);

    QDir dir6(F4_90_S50HZ);
    if(dir6.exists()){
        dir6.remove(F4_90_S50HZ);
    }
    dir6.mkdir(F4_90_S50HZ);

    if(fm_storeInfo.handle){
        fm_storeInfo.handle->flush();
        QString oldFile = fm_storeInfo.handle->fileName();
        delete fm_storeInfo.handle;
        QFile::rename(oldFile,oldFile.mid(0,oldFile.size()-FILE_TEMP_SUFFIX_LEN));
    }
    fm_storeInfo.lessCount = MQ_FILE_MAX_COUNT;
    fm_storeInfo.seq = 0;

    fm_storeInfo.handle = new QFile(fm_filePathDir+FILE_FINAL_PREFIX+QString("%1").arg((fm_storeInfo.seq),10,10,QChar('0'))+QString(FILE_TEMP_SUFFIX));
    tempFlag =1; //首次标志位
    fm_storeInfo.handle->open(QIODevice::WriteOnly);

   fm_storeInfo.seq ++;

}

void FileMaintainer::LoseSlaveClean(){
    QMutexLocker locker(&(fm_storeInfo.mutex));
    fm_curGuid = "";
    if(fm_storeInfo.handle){
        fm_storeInfo.handle->flush();
        QString oldFile = fm_storeInfo.handle->fileName();
        delete fm_storeInfo.handle;
        QFile::rename(oldFile,oldFile.mid(0,oldFile.size()-FILE_TEMP_SUFFIX_LEN));
        fm_storeInfo.handle = 0;
        QFile::rename(oldFile,oldFile.mid(0,oldFile.size()-FILE_TEMP_SUFFIX_LEN));
    }
    QMutexLocker locker_config(&(fm_confiMutex));
    fm_filePathDir = "";
}

void FileMaintainer::DoStoreCollection(const char *content){
    QMutexLocker locker(&(fm_storeInfo.mutex));

    //qDebug("%x",content[5]);

    if(fm_filePathDir.size() == 0)
        return;
    //qDebug()<<fm_storeInfo.lessCount;
    if(fm_storeInfo.lessCount <= 0){  //写完一个文件
        if(fm_storeInfo.handle){
            //qDebug()<<"OK";
            fm_storeInfo.handle->flush();
            QString oldFile = fm_storeInfo.handle->fileName();
            delete fm_storeInfo.handle;
            QFile::rename(oldFile,oldFile.mid(0,oldFile.size()-FILE_TEMP_SUFFIX_LEN) + FILE_FINAL_SUFFIX);  //重命名

        }
        fm_storeInfo.lessCount = MQ_FILE_MAX_COUNT;

        if(filter_Flag ==1)  //滤波操作下的新建文件
         {
            fm_storeInfo.handle = new QFile(F50HZ+FILE_FINAL_PREFIX+QString("%1").arg((fm_storeInfo.seq),10,10,QChar('0'))+QString(FILE_TEMP_SUFFIX));

        }
        else if(filter_Flag == 2){
             fm_storeInfo.handle = new QFile(F1_3HZ+FILE_FINAL_PREFIX+QString("%1").arg((fm_storeInfo.seq),10,10,QChar('0'))+QString(FILE_TEMP_SUFFIX));
        }
        else if(filter_Flag == 3){
             fm_storeInfo.handle = new QFile(F4_30HZ+FILE_FINAL_PREFIX+QString("%1").arg((fm_storeInfo.seq),10,10,QChar('0'))+QString(FILE_TEMP_SUFFIX));
        }
        else if(filter_Flag == 4){
             fm_storeInfo.handle = new QFile(F4_30_S50HZ+FILE_FINAL_PREFIX+QString("%1").arg((fm_storeInfo.seq),10,10,QChar('0'))+QString(FILE_TEMP_SUFFIX));
        }
        else if(filter_Flag == 5){
             fm_storeInfo.handle = new QFile(F1_90_S50HZ+FILE_FINAL_PREFIX+QString("%1").arg((fm_storeInfo.seq),10,10,QChar('0'))+QString(FILE_TEMP_SUFFIX));
        }
        else if(filter_Flag == 6){
             fm_storeInfo.handle = new QFile(F4_90_S50HZ+FILE_FINAL_PREFIX+QString("%1").arg((fm_storeInfo.seq),10,10,QChar('0'))+QString(FILE_TEMP_SUFFIX));
        }
        else
        {
            fm_storeInfo.handle = new QFile(fm_filePathDir+FILE_FINAL_PREFIX+QString("%1").arg((fm_storeInfo.seq),10,10,QChar('0'))+QString(FILE_TEMP_SUFFIX));
        }

        fm_storeInfo.handle->open(QIODevice::WriteOnly);
        fm_storeInfo.seq ++;
        fm_SM->sm_IC->WriteMessage(FILE_WRITED,3);
    }

    if(filter_Flag == 1||filter_Flag == 2||filter_Flag == 3||filter_Flag == 4||filter_Flag == 5||filter_Flag == 6)    //滤波操作
    {
       QByteArray t = QString::number(fm_storeInfo.lessCount).toLatin1();
        if(filterChange == 1){  //如果是滤波模式转换，需要关闭旧文件建立新文件
            filterChange = 0;
//            fm_SM->sm_IC->WriteMessage("enter",5);
//            fm_SM->sm_IC->WriteMessage(t.data(),t.length());
//            fm_SM->sm_IC->WriteMessage(t1.data(),t1.length());

                if(tempFlag == 1)
                {
                    tempFlag =0;
                    fm_storeInfo.seq --;
                }
                if(fm_storeInfo.handle){
                    fm_storeInfo.handle->flush();
                    if(fm_storeInfo.handle->size()==0)
                        fm_storeInfo.handle->remove();
                    QString oldFile = fm_storeInfo.handle->fileName();
                    delete fm_storeInfo.handle;
                    QFile::rename(oldFile,oldFile.mid(0,oldFile.size()-FILE_TEMP_SUFFIX_LEN) + FILE_FINAL_SUFFIX);  //重命名

                 }
                 fm_storeInfo.lessCount = MQ_FILE_MAX_COUNT;
                 if(filter_Flag == 1){
                     fm_storeInfo.seq ++;
              fm_storeInfo.handle = new QFile(F50HZ+FILE_FINAL_PREFIX+QString("%1").arg((fm_storeInfo.seq),10,10,QChar('0'))+QString(FILE_TEMP_SUFFIX));

                 }else if(filter_Flag == 2){
                 fm_storeInfo.seq ++;
                     fm_storeInfo.handle = new QFile(F1_3HZ+FILE_FINAL_PREFIX+QString("%1").arg((fm_storeInfo.seq),10,10,QChar('0'))+QString(FILE_TEMP_SUFFIX));
                 }
                 else if(filter_Flag == 3){
                  fm_storeInfo.seq ++;
                      fm_storeInfo.handle = new QFile(F4_30HZ+FILE_FINAL_PREFIX+QString("%1").arg((fm_storeInfo.seq),10,10,QChar('0'))+QString(FILE_TEMP_SUFFIX));
                  }
                 else if(filter_Flag == 4){
                  fm_storeInfo.seq ++;
                      fm_storeInfo.handle = new QFile(F4_30_S50HZ+FILE_FINAL_PREFIX+QString("%1").arg((fm_storeInfo.seq),10,10,QChar('0'))+QString(FILE_TEMP_SUFFIX));
                  }
                 else if(filter_Flag == 5){
                  fm_storeInfo.seq ++;
                      fm_storeInfo.handle = new QFile(F1_90_S50HZ+FILE_FINAL_PREFIX+QString("%1").arg((fm_storeInfo.seq),10,10,QChar('0'))+QString(FILE_TEMP_SUFFIX));
                  }
                 else if(filter_Flag == 6){
                  fm_storeInfo.seq ++;
                      fm_storeInfo.handle = new QFile(F4_90_S50HZ+FILE_FINAL_PREFIX+QString("%1").arg((fm_storeInfo.seq),10,10,QChar('0'))+QString(FILE_TEMP_SUFFIX));
                  }
                 QString temp = QString::number(fm_storeInfo.seq,10);
                 QByteArray temp1 = QByteArray(FILTER_CHANGE)+temp.toLatin1();
               fm_SM->sm_IC->WriteMessage(temp1.data(),temp1.length());//通知转换后的起始文件序列
              fm_storeInfo.handle->open(QIODevice::WriteOnly);
              fm_storeInfo.seq ++;
         }

        QTextStream ts(fm_storeInfo.handle);
        double TempValue[64];
        for(int i = 0; i < 64; i++){
            TempValue[i] = GET_VALUE(content,3*(i+1),3*(i+1)+1,3*(i+1)+2);
//            if(i == 0)
//             ts<<TempValue[i]<<endl;
        }
        if(filter_Flag == 1){
//        /*BPF corner1 49 corner2 51*/  4 order
            for(int t = 0;t < 64;t++){//64个通道
                x_f1[t][0] = x_f1[t][1]; x_f1[t][1] = x_f1[t][2]; x_f1[t][2] = x_f1[t][3]; x_f1[t][3] = x_f1[t][4]; x_f1[t][4] = x_f1[t][5]; x_f1[t][5] = x_f1[t][6];
                x_f1[t][6] = TempValue[t] / GAIN_F1;
                y_f1[t][0] = y_f1[t][1]; y_f1[t][1] = y_f1[t][2]; y_f1[t][2] = y_f1[t][3]; y_f1[t][3] = y_f1[t][4]; y_f1[t][4] = y_f1[t][5]; y_f1[t][5] = y_f1[t][6];
                y_f1[t][6] =   (x_f1[t][0] + x_f1[t][6]) -   1.8546876965 * (x_f1[t][1] + x_f1[t][5]) +   4.1466221505 * (x_f1[t][2] + x_f1[t][4])
                             -   3.9456671702 * x_f1[t][3]
                             + ( -0.9043475314 * y_f1[t][0]) + (  1.7053824225 * y_f1[t][1])
                             + ( -3.8760796425 * y_f1[t][2]) + (  3.7507324860 * y_f1[t][3])
                             + ( -4.0081587919 * y_f1[t][4]) + (  1.8236153700 * y_f1[t][5]);
                ts<<y_f1[t][6]<<endl;    //将滤波后的数据点写入文件
                }
            fm_storeInfo.lessCount --;
            //ts<<fm_storeInfo.lessCount<<endl;
        }
        else if(filter_Flag == 2){
            /*BSF corner1 1HZ corner2 3HZ */
            for(int t = 0;t < 64;t++){//64个通道
                x_f2[t][0] = x_f2[t][1]; x_f2[t][1] = x_f2[t][2]; x_f2[t][2] = x_f2[t][3]; x_f2[t][3] = x_f2[t][4];
                       x_f2[t][4] =  TempValue[t] / GAIN_F2;
                       y_f2[t][0] = y_f2[t][1]; y_f2[t][1] = y_f2[t][2]; y_f2[t][2] = y_f2[t][3]; y_f2[t][3] = y_f2[t][4];
                       y_f2[t][4] =   (x_f2[t][0] + x_f2[t][4]) - 2 * x_f2[t][2]
                                    + ( -0.9313816821 * y_f2[t][0]) + (  3.7881128654 * y_f2[t][1])
                                    + ( -5.7819541142 * y_f2[t][2]) + (  3.9252194651 * y_f2[t][3]);
                      TempValue[t] = y_f2[t][4];
                ts<<TempValue[t]<<endl;    //将滤波后的数据点写入文件

                }

                fm_storeInfo.lessCount --;


        }
        else if(filter_Flag == 3){
             /*BSF corner1 4HZ corner2 30HZ */
            for(int t = 0;t < 64;t++){//64个通道
                x_f3[t][0] = x_f3[t][1]; x_f3[t][1] = x_f3[t][2]; x_f3[t][2] = x_f3[t][3]; x_f3[t][3] = x_f3[t][4]; x_f3[t][4] = x_f3[t][5]; x_f3[t][5] = x_f3[t][6]; x_f3[t][6] = x_f3[t][7]; x_f3[t][7] = x_f3[t][8];
                        x_f3[t][8] = TempValue[t] / GAIN_F3;
                        y_f3[t][0] = y_f3[t][1]; y_f3[t][1] = y_f3[t][2]; y_f3[t][2] = y_f3[t][3]; y_f3[t][3] = y_f3[t][4]; y_f3[t][4] = y_f3[t][5]; y_f3[t][5] = y_f3[t][6]; y_f3[t][6] = y_f3[t][7]; y_f3[t][7] = y_f3[t][8];
                        y_f3[t][8] =   (x_f3[t][0] + x_f3[t][8]) - 4 * (x_f3[t][2] + x_f3[t][6]) + 6 * x_f3[t][4]
                                     + ( -0.1747391497 * y_f3[t][0]) + (  1.6274382519 * y_f3[t][1])
                                     + ( -6.7618116747 * y_f3[t][2]) + ( 16.3619996390 * y_f3[t][3])
                                     + (-25.2315671400 * y_f3[t][4]) + ( 25.3948071150 * y_f3[t][5])
                                     + (-16.2750022740 * y_f3[t][6]) + (  6.0588596681 * y_f3[t][7]);
                       TempValue[t] = y_f3[t][8];

                ts<<TempValue[t]<<endl;    //将滤波后的数据点写入文件

                }

                fm_storeInfo.lessCount --;

        }
        else if(filter_Flag == 4){
             /*BSF corner1 4HZ corner2 30HZ  + 50 BSF*/
            for(int t = 0;t < 64;t++){//64个通道
                   //带通
                x_f4_1[t][0] = x_f4_1[t][1]; x_f4_1[t][1] = x_f4_1[t][2]; x_f4_1[t][2] = x_f4_1[t][3]; x_f4_1[t][3] = x_f4_1[t][4]; x_f4_1[t][4] = x_f4_1[t][5]; x_f4_1[t][5] = x_f4_1[t][6]; x_f4_1[t][6] = x_f4_1[t][7]; x_f4_1[t][7] = x_f4_1[t][8];
                        x_f4_1[t][8] = TempValue[t] / GAIN_F4_1;
                        y_f4_1[t][0] = y_f4_1[t][1]; y_f4_1[t][1] = y_f4_1[t][2]; y_f4_1[t][2] = y_f4_1[t][3]; y_f4_1[t][3] = y_f4_1[t][4]; y_f4_1[t][4] = y_f4_1[t][5]; y_f4_1[t][5] = y_f4_1[t][6]; y_f4_1[t][6] = y_f4_1[t][7]; y_f4_1[t][7] = y_f4_1[t][8];
                        y_f4_1[t][8] =   (x_f4_1[t][0] + x_f4_1[t][8]) - 4 * (x_f4_1[t][2] + x_f4_1[t][6]) + 6 * x_f4_1[t][4]
                                     + ( -0.1747391497 * y_f4_1[t][0]) + (  1.6274382519 * y_f4_1[t][1])
                                     + ( -6.7618116747 * y_f4_1[t][2]) + ( 16.3619996390 * y_f4_1[t][3])
                                     + (-25.2315671400 * y_f4_1[t][4]) + ( 25.3948071150 * y_f4_1[t][5])
                                     + (-16.2750022740 * y_f4_1[t][6]) + (  6.0588596681 * y_f4_1[t][7]);


                        //TempValue[t] = y_f4_1[t][12];
                        //陷波
                        x_f4_2[t][0] = x_f4_2[t][1]; x_f4_2[t][1] = x_f4_2[t][2]; x_f4_2[t][2] = x_f4_2[t][3]; x_f4_2[t][3] = x_f4_2[t][4]; x_f4_2[t][4] = x_f4_2[t][5]; x_f4_2[t][5] = x_f4_2[t][6];
                                x_f4_2[t][6] = y_f4_1[t][8] / GAIN_F4_2;
                                y_f4_2[t][0] = y_f4_2[t][1]; y_f4_2[t][1] = y_f4_2[t][2]; y_f4_2[t][2] = y_f4_2[t][3]; y_f4_2[t][3] = y_f4_2[t][4]; y_f4_2[t][4] = y_f4_2[t][5]; y_f4_2[t][5] = y_f4_2[t][6];
                                y_f4_2[t][6] =   (x_f4_2[t][0] + x_f4_2[t][6]) -   1.8546876965 * (x_f4_2[t][1] + x_f4_2[t][5]) +   4.1466221505 * (x_f4_2[t][2] + x_f4_2[t][4])
                                             -   3.9456671702 * x_f4_2[t][3]
                                             + ( -0.9043475314 * y_f4_2[t][0]) + (  1.7053824225 * y_f4_2[t][1])
                                             + ( -3.8760796425 * y_f4_2[t][2]) + (  3.7507324860 * y_f4_2[t][3])
                                             + ( -4.0081587919 * y_f4_2[t][4]) + (  1.8236153700 * y_f4_2[t][5]);

                        TempValue[t] =  y_f4_2[t][6];

                        ts<<TempValue[t]<<endl;    //将滤波后的数据点写入文件

                }

                fm_storeInfo.lessCount --;

        }
        else if(filter_Flag == 5){
             /*BSF corner1 4HZ corner2 30HZ */
            for(int t = 0;t < 64;t++){//64个通道
                x_f5_1[t][0] = x_f5_1[t][1]; x_f5_1[t][1] = x_f5_1[t][2]; x_f5_1[t][2] = x_f5_1[t][3]; x_f5_1[t][3] = x_f5_1[t][4]; x_f5_1[t][4] = x_f5_1[t][5]; x_f5_1[t][5] = x_f5_1[t][6]; x_f5_1[t][6] = x_f5_1[t][7]; x_f5_1[t][7] = x_f5_1[t][8];
                        x_f5_1[t][8] =  TempValue[t] / GAIN_F5_1;
                        y_f5_1[t][0] = y_f5_1[t][1]; y_f5_1[t][1] = y_f5_1[t][2]; y_f5_1[t][2] = y_f5_1[t][3]; y_f5_1[t][3] = y_f5_1[t][4]; y_f5_1[t][4] = y_f5_1[t][5]; y_f5_1[t][5] = y_f5_1[t][6]; y_f5_1[t][6] = y_f5_1[t][7]; y_f5_1[t][7] = y_f5_1[t][8];
                        y_f5_1[t][8] =   (x_f5_1[t][0] + x_f5_1[t][8]) - 4 * (x_f5_1[t][2] + x_f5_1[t][6]) + 6 * x_f5_1[t][4]
                                     + ( -0.0851539233 * y_f5_1[t][0]) + ( -0.1844294329 * y_f5_1[t][1])
                                     + (  0.2824819463 * y_f5_1[t][2]) + (  0.9338703964 * y_f5_1[t][3])
                                     + ( -0.5977548688 * y_f5_1[t][4]) + ( -1.0898646315 * y_f5_1[t][5])
                                     + ( -0.4720522120 * y_f5_1[t][6]) + (  2.2129008208 * y_f5_1[t][7]);
                       //TempValue[t] = y_f5_1[t][8];

                        //第二级 陷波
                        x_f5_2[t][0] = x_f5_2[t][1]; x_f5_2[t][1] = x_f5_2[t][2]; x_f5_2[t][2] = x_f5_2[t][3]; x_f5_2[t][3] = x_f5_2[t][4]; x_f5_2[t][4] = x_f5_2[t][5]; x_f5_2[t][5] = x_f5_2[t][6];
                        x_f5_2[t][6] = y_f5_1[t][8] / GAIN_F5_2;
                        y_f5_2[t][0] = y_f5_2[t][1]; y_f5_2[t][1] = y_f5_2[t][2]; y_f5_2[t][2] = y_f5_2[t][3]; y_f5_2[t][3] = y_f5_2[t][4]; y_f5_2[t][4] = y_f5_2[t][5]; y_f5_2[t][5] = y_f5_2[t][6];
                        y_f5_2[t][6] =   (x_f5_2[t][0] + x_f5_2[t][6]) -   1.8546876965 * (x_f5_2[t][1] + x_f5_2[t][5]) +   4.1466221505 * (x_f5_2[t][2] + x_f5_2[t][4])
                                     -   3.9456671702 * x_f5_2[t][3]
                                     + ( -0.9043475314 * y_f5_2[t][0]) + (  1.7053824225 * y_f5_2[t][1])
                                     + ( -3.8760796425 * y_f5_2[t][2]) + (  3.7507324860 * y_f5_2[t][3])
                                     + ( -4.0081587919 * y_f5_2[t][4]) + (  1.8236153700 * y_f5_2[t][5]);


                        TempValue[t] = y_f5_2[t][6];

                ts<<TempValue[t]<<endl;    //将滤波后的数据点写入文件

                }

                fm_storeInfo.lessCount --;

        }
        else if(filter_Flag == 6){
             /*BSF corner1 4HZ corner2 30HZ */
            for(int t = 0;t < 64;t++){//64个通道
                x_f6_1[t][0] = x_f6_1[t][1]; x_f6_1[t][1] = x_f6_1[t][2]; x_f6_1[t][2] = x_f6_1[t][3]; x_f6_1[t][3] = x_f6_1[t][4]; x_f6_1[t][4] = x_f6_1[t][5]; x_f6_1[t][5] = x_f6_1[t][6]; x_f6_1[t][6] = x_f6_1[t][7]; x_f6_1[t][7] = x_f6_1[t][8];
                        x_f6_1[t][8] =TempValue[t] / GAIN_F6_1;
                        y_f6_1[t][0] = y_f6_1[t][1]; y_f6_1[t][1] = y_f6_1[t][2]; y_f6_1[t][2] = y_f6_1[t][3]; y_f6_1[t][3] = y_f6_1[t][4]; y_f6_1[t][4] = y_f6_1[t][5]; y_f6_1[t][5] = y_f6_1[t][6]; y_f6_1[t][6] = y_f6_1[t][7]; y_f6_1[t][7] = y_f6_1[t][8];
                        y_f6_1[t][8] =   (x_f6_1[t][0] + x_f6_1[t][8]) - 4 * (x_f6_1[t][2] + x_f6_1[t][6]) + 6 * x_f6_1[t][4]
                                     + ( -0.0681311735 * y_f6_1[t][0]) + ( -0.1330494364 * y_f6_1[t][1])
                                     + (  0.2562480475 * y_f6_1[t][2]) + (  0.7687308490 * y_f6_1[t][3])
                                     + ( -0.6666422465 * y_f6_1[t][4]) + ( -0.8758889252 * y_f6_1[t][5])
                                     + ( -0.3187473578 * y_f6_1[t][6]) + (  2.0370107223 * y_f6_1[t][7]);

                        //第二级  陷波
                        x_f6_2[t][0] = x_f6_2[t][1]; x_f6_2[t][1] = x_f6_2[t][2]; x_f6_2[t][2] = x_f6_2[t][3]; x_f6_2[t][3] = x_f6_2[t][4]; x_f6_2[t][4] = x_f6_2[t][5]; x_f6_2[t][5] = x_f6_2[t][6];
                        x_f6_2[t][6] = y_f6_1[t][8] / GAIN_F6_2;
                        y_f6_2[t][0] = y_f6_2[t][1]; y_f6_2[t][1] = y_f6_2[t][2]; y_f6_2[t][2] = y_f6_2[t][3]; y_f6_2[t][3] = y_f6_2[t][4]; y_f6_2[t][4] = y_f6_2[t][5]; y_f6_2[t][5] = y_f6_2[t][6];
                        y_f6_2[t][6] =   (x_f6_2[t][0] + x_f6_2[t][6]) -   1.8546876965 * (x_f6_2[t][1] + x_f6_2[t][5]) +   4.1466221505 * (x_f6_2[t][2] + x_f6_2[t][4])
                                     -   3.9456671702 * x_f6_2[t][3]
                                     + ( -0.9043475314 * y_f6_2[t][0]) + (  1.7053824225 * y_f6_2[t][1])
                                     + ( -3.8760796425 * y_f6_2[t][2]) + (  3.7507324860 * y_f6_2[t][3])
                                     + ( -4.0081587919 * y_f6_2[t][4]) + (  1.8236153700 * y_f6_2[t][5]);

                        TempValue[t] = y_f6_2[t][6];

                ts<<TempValue[t]<<endl;    //将滤波后的数据点写入文件

                }

                fm_storeInfo.lessCount --;

        }
    }
    else{        //无滤波
        if(filterChange == 1) //如果是滤波模式转换，需要关闭旧文件建立新文件
         {
            filterChange = 0 ;
            if(fm_storeInfo.handle){

                fm_storeInfo.handle->flush();
                QString oldFile = fm_storeInfo.handle->fileName();
                delete fm_storeInfo.handle;
                QFile::rename(oldFile,oldFile.mid(0,oldFile.size()-FILE_TEMP_SUFFIX_LEN) + FILE_FINAL_SUFFIX);  //重命名
            }
            fm_storeInfo.lessCount = MQ_FILE_MAX_COUNT;

            fm_storeInfo.handle = new QFile(fm_filePathDir+FILE_FINAL_PREFIX+QString("%1").arg((fm_storeInfo.seq),10,10,QChar('0'))+QString(FILE_TEMP_SUFFIX));
            fm_storeInfo.handle->open(QIODevice::WriteOnly);

            QString temp = QString::number(fm_storeInfo.seq,10);
            QByteArray temp1 = QByteArray(FILTER_CHANGE)+temp.toLatin1();
             fm_SM->sm_IC->WriteMessage(temp1.data(),temp1.length());//通知转换后的起始文件序列

            fm_storeInfo.seq ++;

        }
        fm_storeInfo.handle->write((char *)content,198);
        //qDebug()<<(int)QThread::currentThreadId();
        fm_storeInfo.handle->flush();
        fm_storeInfo.lessCount --;
    }
}

void FileMaintainer::DoStoreB(const QString content){
    QMutexLocker locker_config(&(fm_confiMutex));
    if(fm_filePathDir.size() == 0)
        return;
    QFile file(fm_filePathDir+FILE_VOLTAGE_NAME);
    file.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text);
    file.write(content.toLatin1());
    file.close();
}
void FileMaintainer::DoStoreC(const QString content){
    QMutexLocker locker_config(&(fm_confiMutex));
    if(fm_filePathDir.size() == 0)
        return;
    QFile file(fm_filePathDir+FILE_TEMPERATURE_NAME);
    file.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text);
    file.write(content.toLatin1());
    file.close();
}
void FileMaintainer::DoStoreIP(const QString content){
    QMutexLocker locker_config(&(fm_confiMutex));
    if(fm_filePathDir.size() == 0)
        return;
    QFile file(fm_filePathDir+FILE_IP_NAME);
    file.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text);
    file.write(content.toLatin1());
    file.close();
}
void FileMaintainer::DoStoreW(const QString content){
    QMutexLocker locker_config(&(fm_confiMutex));
    if(fm_filePathDir.size() == 0)
        return;
    QFile file(fm_filePathDir+FILE_ID_NAME);
    file.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text);
    file.write(content.toLatin1());
    file.close();
}
void FileMaintainer::DoStoreLog(const QString content){
    QMutexLocker locker_config(&(fm_confiMutex));
    if(fm_filePathDir.size() == 0)
        return;
    QFile file(fm_filePathDir+FILE_LOG_NAME);
    file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
    file.write((content+"\n").toLocal8Bit());
    file.close();
}
