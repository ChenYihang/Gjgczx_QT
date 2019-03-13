#ifndef MULTICHANNELCOLLECTIONONLINE_H
#define MULTICHANNELCOLLECTIONONLINE_H

#include <QWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QProcess>
#include <SubInterface/onlineinterface.h>
#include <SubInterface/offlineinterface.h>

class MultiChannelCollectionOnline : public QWidget
{
    Q_OBJECT

    public:
        MultiChannelCollectionOnline(QWidget *parent = 0);
        ~MultiChannelCollectionOnline();

    signals:
        void StopOn();

    public slots:
        void DoOnline();
        void DoOffLine();
        void DoStop();
        void DoTextChanged(const QString text);
        void SendOrderToCollectionServer(QString);
        void ReadFromCollectionServer();
        void ProcessFinished(int, QProcess::ExitStatus);
        void ProcessError(QProcess::ProcessError);
        void ChoiceDataPath();
    protected:
        void closeEvent(QCloseEvent *);

    private:
        void Init();

    private:
        //控件
        QPushButton *mcco_online;
        QPushButton *mcco_offline;
        QPushButton *mcco_stop;
        QLabel *mcco_portName;
        QLineEdit *mcco_port;
        QLabel *mcco_baseName;
        QLineEdit *mcco_base;
        QLabel *mcco_replayUnitName;
        QLineEdit *mcco_replayUnit;
        QLabel *mcco_dataRateName;
        QLineEdit *mcco_dataRate;
        QLabel *mcco_replaySpeedName;
        QLineEdit *mcco_replaySpeed;

        QLineEdit *mcco_dataPathEdit;
        QPushButton *mcco_dataPathChoice;
        QHBoxLayout *mcco_dataPathLayout;

        OnlineInterface *mcco_onlineInterface;
        OfflineInterface *mcco_offlineInterface;
        QWidget *mcco_stopInterface;
        QLabel *mcco_stopLabel;
        QHBoxLayout *mcco_stopLaout;
        QHBoxLayout *mcco_buttonLayout;
        QVBoxLayout *mcco_mainLayout;
        QProcess *mcco_collectionServer;
};



#endif // MULTICHANNELCOLLECTIONONLINE_H
