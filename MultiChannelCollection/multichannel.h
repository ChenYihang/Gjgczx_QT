#ifndef MULTICHANNEL_H
#define MULTICHANNEL_H

#include <QWidget>
#include "csocketserver.h"
#include "csocketprocess.h"
#include "common.h"
#include "multifunc.h"
#include "cdataprocess.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QSplineSeries>
#include <QtWidgets/QGraphicsView>
#include "timechartplot.h"


QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MultiChannel;
}

class MultiChannel : public QWidget
{
    Q_OBJECT

public:
    explicit MultiChannel(QWidget *parent = nullptr);
    ~MultiChannel();
private slots:

    void slotAddNewClient(QString qsip,int iflag);
    void slotDelClient(int iflag);
    void UIinitialization();
    void plotInit();
    void fileDealInfo(QString info);
    void deleteTab(int index);




    void on_siglePB_clicked();

    void on_start_clicked();

    void on_sendPB_clicked();

    void on_multiPB_clicked();

    void on_startPB_clicked();

    void on_stopPB_clicked();

    void on_resetPB_clicked();

private:
    Ui::MultiChannel *ui;

    CsocketServer *MserverHandle;

    CsocketProcess *MprocessHandle;

    int clientCount;



};

#endif // MULTICHANNEL_H
