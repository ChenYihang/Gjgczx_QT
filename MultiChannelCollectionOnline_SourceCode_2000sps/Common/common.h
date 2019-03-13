#ifndef COMMON_H
#define COMMON_H

#define HIGH_PERFORMANCE              0                                     //是否为高性能电脑
#define MAX_SLAVE_COMPUTER_COUNT      64                                    //最大下位机数
#define DATA_CHANNEL_COUNT            64                                    //每个数据包含的通道数
#define MQ_MESSAGE_LENGHT             198                                   //每条消息的总长度
#define MQ_FILE_MAX_COUNT             2000                                  //每个文件最大消息条数
#define FILE_CONFIG_NAME_SUFFIX       "mcco_"                               //配置文件前缀
#define FILE_LOG_NAME                 FILE_CONFIG_NAME_SUFFIX"log"          //日志文件的文件名
#define FILE_IP_NAME                  FILE_CONFIG_NAME_SUFFIX"ip"           //IP配置文件名
#define FILE_ID_NAME                  FILE_CONFIG_NAME_SUFFIX"id"           //ID配置文件名
#define FILE_VOLTAGE_NAME             FILE_CONFIG_NAME_SUFFIX"voltage"      //电压配置文件名
#define FILE_TEMPERATURE_NAME         FILE_CONFIG_NAME_SUFFIX"temperature"  //温度配置文件名
#define FILE_FINAL_PREFIX              "data_"                              //采集文件前缀
#define FILE_FINAL_PREFIX_LEN          5                                    //采集文件前缀长度
#define FILE_FINAL_SUFFIX             ".txt"                                //最终文件标志
#define FILE_TEMP_SUFFIX              ".now"                                //采集临时标志
#define FILE_TEMP_SUFFIX_LEN          4                                     //采集临时标志的长度




#define NZEROS_F1 4
#define NPOLES_F1 4
#define GAIN_F1  1.004452767e+00



#define NZEROS_F2 4
#define NPOLES_F2 4
#define GAIN_F2  1.015734829e+05


#define NZEROS_F3 6
#define NPOLES_F3 6
#define GAIN_F3   1.587834332e+04


#define NZEROS_F4_1 6
#define NPOLES_F4_1 6
#define GAIN_F4_1  1.587834332e+04
#define NZEROS_F4_2 4
#define NPOLES_F4_2 4
#define GAIN_F4_2  1.004452767e+00


#define NZEROS_F5_1 6
#define NPOLES_F5_1 6
#define GAIN_F5_1  4.719716097e+02
#define NZEROS_F5_2 4
#define NPOLES_F5_2 4
#define GAIN_F5_2  1.004452767e+00



#define NZEROS_F6_1 6
#define NPOLES_F6_1 6
#define GAIN_F6_1   5.199612004e+02
#define NZEROS_F6_2 4
#define NPOLES_F6_2 4
#define GAIN_F6_2   1.004452767e+00



typedef enum LINK_MESSAGE_TYPE {                                            //消息类型，包括上行和下行消息
    LINK_MESSAGE_TYPE_USELESS           = -1,
    UPLINK_MESSAGE_TYPE_COLLECTION     = 0,
    UPLINK_MESSAGE_TYPE_B              = 1,
    UPLINK_MESSAGE_TYPE_C              = 2,
    UPLINK_MESSAGE_TYPE_W              = 3,
    UPLINK_MESSAGE_TYPE_F              = 4,
    DOWNLINK_MESSAGE_TYPE_T_RESET      = 5,
    DOWNLINK_MESSAGE_TYPE_T_COLLECTION = 6,
    DOWNLINK_MESSAGE_TYPE_T_STIMULATE  = 7,
    DOWNLINK_MESSAGE_TYPE_S            = 8
} LINK_MESSAGE_TYPE;



typedef struct MESSAGE_UNIT_DEFINE{                        //本地消息队列的消息包
    int flag;
    LINK_MESSAGE_TYPE type;
    char content[MQ_MESSAGE_LENGHT+2];
} MSG_UNIT;

extern volatile int DATA_CACHE_MAX[4];                     //最大显示数据

//上报命令
#define SLAVE_CONNECT           "ADD"                           //新下位机接入
#define SLAVE_DISCONNECT        "DEL"                           //旧下位机断开
#define SLAVE_UPDATE            "UPD"                           //当前下位机有信息更新
#define SLAVE_UPD_TEP           "TEP"                           //有温度更新
#define SLAVE_UPD_VTG           "VTG"                           //有电压更新
#define SLAVE_UPD_LOG           "LOG"                           //有日志更新
#define SLAVE_UPD_IDF           "IDF"                           //有标识更新
#define FILE_WRITED             "WRE"                           //已写文件
#define FILTER_CHANGE           "FLC"                           //滤波切换
typedef enum{                                             //下发命令
    RESET_OPERATE    = 0,
    COLLECTION_START = 1,
    COLLECTION_STOP  = 2,
    STIMULATE_STAET  = 3,
    STIMULATE_STOP   = 4,
    DOWN_DATA_TEXT   = 5,
    DOWN_DATA_FILE   = 6,
    TAB_CLOSED       = 7,
    FILTER          = 8
} INTERFACE_OPERATE;

#define GET_VALUE(buff,high,midd,low) \
    ((buff)[(high)] & 0x0080) ? \
    ((int)((((buff)[(high)]&0x00FF)<<16 | ((buff)[(midd)]&0x00FF)<<8 | ((buff)[(low)]&0x00FF)) | 0xFF000000))*(4.5/8388608/12*1000000) :\
    ((int)((((buff)[(high)]&0x00FF)<<16 | ((buff)[(midd)]&0x00FF)<<8 | ((buff)[(low)]&0x00FF)) & 0x00FFFFFF))*(4.5/8388608/12*1000000)


        static double x_f1[64][NZEROS_F1+1];
        static double y_f1[64][NPOLES_F1+1];

        static double x_f2[64][NZEROS_F2+1];
        static double y_f2[64][NPOLES_F2+1];

        static double x_f3[64][NZEROS_F3+1];
        static double y_f3[64][NPOLES_F3+1];

        static double x_f4_1[64][NZEROS_F4_1+1];
        static double y_f4_1[64][NPOLES_F4_1+1];
        static double x_f4_2[64][NZEROS_F4_2+1];
        static double y_f4_2[64][NPOLES_F4_2+1];

        static double x_f5_1[64][NZEROS_F5_1+1];
        static double y_f5_1[64][NPOLES_F5_1+1];
        static double x_f5_2[64][NZEROS_F5_2+1];
        static double y_f5_2[64][NPOLES_F5_2+1];

        static double x_f6_1[64][NZEROS_F6_1+1];
        static double y_f6_1[64][NPOLES_F6_1+1];
        static double x_f6_2[64][NZEROS_F6_2+1];
        static double y_f6_2[64][NPOLES_F6_2+1];


#endif // COMMON_H
