#ifndef ONLINEINTERFACE_H
#define ONLINEINTERFACE_H

#include <QWidget>
#include <QPushButton>
#include <QSplitter>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtCharts/QLineSeries>
#include <QtCharts/QtCharts>
#include <QtCharts/QScatterSeries>
#include <QTabBar>
#include <SubInterface/comboboxt.h>
#include <SubInterface/namelineeditt.h>
#include <FFTW/fftw3.h>
#include <SubInterface/colorcard.h>
#include <SubInterface/rmsgroup.h>

class OnlineInterface : public QWidget
{
    Q_OBJECT
    friend class SourceManager;
    protected:
        explicit OnlineInterface(QWidget *parent = nullptr);

    public:
        ~OnlineInterface();
    signals:
        void OrderSend(QString);
    public slots:
        void TimeSlotChanged(int index);
        void RangeChanged(int index);
        void ChannelChanged(int index);
        void FrequencyChanged(int index);
        void FilterChanged(int index);

        void frequencyMagChanged(int index);


        void TabChanged(int index);
        void TabClosed(int index);
        void UpdateChart();
        void CollectionStart();
        void CollectionStop();
        void DoReset();
        void StimulateStart();
        void StimulateStop();
        void DownDataText();
        void DownDataFile();
        void ChoiceDownDataPath();
        void UpdateRMSAndColorCard();
    public:
        static OnlineInterface *GetInstance();
        void ChartStart(int unit,int rate,QString dataDir);
        void ChartStop();
        //采集服务发生的信息
        void MessageFromCollectionServer(QString msg);
        //新增下位机
        void AddInterface(QString msg);
        //删除下位机
        void DeleteInterface(QString msg);
        //更新下位机
        void UpdateInterface(QString msg);

        int FFTNum = 0;

        struct FFTW_SETTING{
            double *inData;
            fftw_complex *outData;
            fftw_plan plan;
//            fftw_plan plan60;
//            fftw_plan plan120;
            int index;
            int start;
            int end;
            int cursorFrom;
            int cursorTo;
        }oni_fftw;

        struct{
            QChart *chart;
            QChartView *view;
            QValueAxis *axisX;
            QValueAxis *axisY;
            QLineSeries *(data[8]);
        } oni_frequencyChart;

    private:
        void InitLeftInterface();
        void InitRightInterface();
        void DestoryLeftInterface();
        void DestoryRightInterface();
        void UpdateFrequencyChart();
        void UpdateFrequencyDataCalculation(QLineSeries *source,QLineSeries *object);
        bool UpdateTimeChart();
        void ResetData();
        void Inlog(QString msg);

    private:
        static OnlineInterface *oni_onlineI;

        struct{
            QTabBar *tabBar;
            QMutex mutex;
        }oni_tab;
        //左边控件
        QPushButton *oni_collectStart;
        QLabel *oni_collectName;
        QPushButton *oni_collectEnd;
        QVBoxLayout *oni_collectLayout;
        QPushButton *oni_reset;
        QPushButton *oni_stimulateStart;
        QLabel *oni_stimulateName;
        QPushButton *oni_stimulateEnd;
        QVBoxLayout *oni_stimulateLayout;
        QTextEdit *oni_downDataEidt;
        QCheckBox *oni_downDataFormat;
        QPushButton *oni_downDataSend;
        QHBoxLayout *oni_downDataControl;
        QVBoxLayout *oni_downDataLayout;
        QHBoxLayout *oni_controlLayout;
        QLineEdit *oni_downFilePathEdit;
        QPushButton *oni_downFilePathChoice;
        QHBoxLayout *oni_downFilePathLayout;
        QPushButton *oni_downFileSubmit;
        QHBoxLayout *oni_downFileLayout;



        QTextEdit *oni_log;
        ColorCard *oni_colorCard;
        QVBoxLayout *oni_leftLayout;
        QWidget *oni_leftLayoutW;

        //右边控件
        ComboBoxT *oni_timeSlot;
        ComboBoxT *oni_range;
        int oni_rangeBase;
        ComboBoxT *oni_channel;
        ComboBoxT *oni_frequency;

        ComboBoxT *oni_filter;

        ComboBoxT *frequencyMag;

        NameLineEditT *oni_temperature;
        NameLineEditT *oni_voltage;
        QSplitter *oni_comboBoxS;

        struct{
            QMutex mutex;
            QLineSeries *(data[DATA_CHANNEL_COUNT]);
            qreal curEndX;
        }oni_realData;
        struct{
            QChart *chart;
            QChartView *view;
            QValueAxis *axisXShow;
            QValueAxis *axisXValue;
            QCategoryAxis *axisY;
            RMSGroup *rmsGroup;
        } oni_chart[4];
        QSplitter *oni_splitH01to32;
        QSplitter *oni_splitH33to64;
        QSplitter *oni_splitV;
        QVBoxLayout *oni_rightLayout;

        QTimer *oni_interfaceFlushTimer;
        struct{
            int realUnitCount;
            int dataRate;
            QString basePath;
            QString guid;
            unsigned int fileSeq;
            QFile file;
            QTextStream ts;
            QByteArray buff;
            //double filterBuff[64];
            QVector<double> filterBuff;
            QList<QPointF> *(newData[DATA_CHANNEL_COUNT]);
        } oni_dataFile;

        struct{
            QTimer *updateTimer;
            qreal valueList[DATA_CHANNEL_COUNT];
            int pos;
            volatile bool hasNewData;
        } oni_RMSAndColorCard;
        //全局布局控件
        QHBoxLayout *oni_bottomLayout;
        QVBoxLayout *oni_mainLayout;

         int pfile ;
         int wfile ;

         int filterChangeTemp;
         int filterChangeFlag = 0;
         int count ;

};

#endif // ONLINEINTERFACE_H
