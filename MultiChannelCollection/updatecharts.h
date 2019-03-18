#ifndef UPDATECHARTS_H
#define UPDATECHARTS_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <timechartplot.h>

class updateCharts : public QThread
{
    Q_OBJECT

    inline qreal getValue(QByteArray twoBytes);

public:
    explicit updateCharts(QThread  *parent = nullptr);
    ~updateCharts();
    timeWidget *tWidget;

signals:
    void signalCurrentPlayState(QString state);
public slots:
    void run();
    void startPlotFile(QString fName);

//       if(twoBytes[0]&0x0020) //负值
//           return (qreal)((((twoBytes[0]&0x001f)<<8)|(twoBytes[1]&0x00ff))/8192.0*2.5);
//        else
//            return (qreal)((((twoBytes[0]&0x001f)<<8)|(twoBytes[1]&0x00ff))/8192.0*2.5+2.5);

//       }


private slots:

    void stateChange(bool);
    void speedChange(unsigned long);




private:
    QString fileName;
    qint64 fileSize;
    QFile *rFile;
    bool runState;
    unsigned long runSpeed;
    unsigned int MBcount;





};

#endif // UPDATECHARTS_H
