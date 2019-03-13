#ifndef OFFLINEINTERFACE_H
#define OFFLINEINTERFACE_H

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
#include <QList>
#include <../Common/common.h>
#include <SubInterface/comboboxt.h>
#include <FFTW/fftw3.h>
#include <SubInterface/rmsgroup.h>
#include <SubInterface/colorcard.h>

class OfflineInterface : public QWidget
{
    Q_OBJECT
    protected:
        explicit OfflineInterface(QWidget *parent = nullptr);
    public:
        ~OfflineInterface();

    signals:

    public slots:
        void TimeSlotChanged(int index);
        void RangeChanged(int index);
        void ChannelChanged(int index);
        void FrequencyChanged(int index);
        void ChoiceOfflinePath();
        void ChoiceOfflineFilePath();
        void SlaveChanged(int index);
        void UpdateData();
        void UpdateRMSAndColorCard();
        void StartUpdateData();
        void SuspendUpdateData();
        void StopUpdateData();
    public:
        static OfflineInterface *GetInstance();
        void DoInit(int unit,int speed,QString dataDir);
        void DoFree();

    private:
        void InitInterface();
        void DestoryLeftInterface();
        void DestoryRightInterface();
        void UpdateTimeData();
        void UpdateFrequencyData();
        void UpdateFrequencyDataCalculation(QLineSeries *source,QLineSeries *object);
        void RecordLog(QString message);
    private:
        static OfflineInterface *offi_offlineI;
        int offi_speed;
        typedef enum{
            UPDATA_STATUS_START   = 0,
            UPDATA_STATUS_SUSPEND = 1,
            UPDATA_STATUS_STOP    = 2,
        } UPDATA_STATUS;
        QPushButton *offi_start;
        QPushButton *offi_suspend;
        QPushButton *offi_stop;
        int offi_updateStatus;

        ComboBoxT *offi_slave;
        QHBoxLayout *offi_leftTop;
        typedef struct {
            QString id;
            QString path;
        } SLAVE_LIST;
        QList<SLAVE_LIST> *offi_slaveList;

        QLineEdit *offi_offlinePathEdit;
        QPushButton *offi_offlinePathChoice;
        QHBoxLayout *offi_offlinePathLayout;
        QLineEdit *offi_offlineFilePathEdit;
        QPushButton *offi_offlineFilePathChoice;
        QHBoxLayout *offi_offlineFilePathLayout;

        struct{
            QChart *chart;
            QChartView *view;
            QValueAxis *axisX;
            QValueAxis *axisY;
            QLineSeries *(data[8]);
        } offi_frequencyChart;

        struct FFTW_SETTING{
            double *inData;
            fftw_complex *outData;
            fftw_plan plan30;
            fftw_plan plan60;
            fftw_plan plan120;
            int index;
            int start;
            int end;
            int cursorFrom;
            int cursorTo;
        }offi_fftw;

        QTextEdit *offi_log;
        ColorCard *offi_colorCard;
        QVBoxLayout *offi_left;
        QWidget *offi_leftW;

        ComboBoxT *offi_channel;
        ComboBoxT *offi_timeSlot;
        ComboBoxT *offi_range;
        ComboBoxT *offi_frequency;
        QHBoxLayout *offi_rightTop;
        int offi_rangeBase;
        struct{
            QMutex mutex;
            QLineSeries *(data[DATA_CHANNEL_COUNT]);
            qreal curEndX;
        }offi_realData;
        struct{
            QChart *chart;
            QChartView *view;
            QValueAxis *axisXShow;
            QValueAxis *axisXValue;
            QCategoryAxis *axisY;
            RMSGroup *rmsGroup;
        } offi_chart[4];
        QSplitter *offi_splitH01to32;
        QSplitter *offi_splitH33to64;
        QSplitter *offi_splitV;
        QVBoxLayout *offi_right;

        //全局布局控件
        QHBoxLayout *offi_mainLayout;

        int offi_realUnitCount;
        QTimer *offi_dataUpdateTimer;
        struct{
            QStringList fileList;
            int curReadFilePosInList;
            QFile file;
            QByteArray buff;
            QList<QPointF> *(newData[DATA_CHANNEL_COUNT]);
        } offi_dataFile;

        struct{
            QTimer *updateTimer;
            qreal valueList[DATA_CHANNEL_COUNT];
            int pos;
        } offi_RMSAndColorCard;
};


#endif // OFFLINEINTERFACE_H
