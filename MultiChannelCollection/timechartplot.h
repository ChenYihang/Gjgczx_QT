#ifndef TIMECHARTPLOT_H
#define TIMECHARTPLOT_H
#include <QDebug>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QLineSeries>
#include <QComboBox>
#include <QThread>
#include <QFile>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QtWidgets/QGraphicsView>
#include <QFileInfo>
#include <QFileDialog>

QT_CHARTS_USE_NAMESPACE


class timeChartPlot : public QGraphicsView
{
    Q_OBJECT

public:
    explicit timeChartPlot(QWidget *parent = nullptr);
    ~timeChartPlot();

    friend class timeWidget;
    void updateTimeChart(QList<qreal> *timeData[] );


protected:
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);



public slots:

private slots:
    void handleTimeout();
    void chartInit();




private:
    struct{

         QValueAxis *axisX;
         QValueAxis *axisY;
         QCategoryAxis *axisY_3;
         QLineSeries *series[8];
         QGraphicsSimpleTextItem *yValueLabel[11];

         qint64 XplotRange;
         qint64 YplotRange;

     }chartElement;

    QChart *chart;
    QTimer m_timer;
    QGraphicsSimpleTextItem *m_coordX;
    QGraphicsSimpleTextItem *m_coordY;





};

class timeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit timeWidget(QWidget *parent = nullptr);
    ~timeWidget();

    timeChartPlot *timeChart;
    void updatePlayState(QString state);


private slots:

    void initWidget();
    QComboBox *createTimeScaleBox() const;
    QComboBox *createChSelectBox() const;
    QComboBox *createSPSelectBox() const;

    QPushButton *createStartButton() const;
    QPushButton *createFileButton() const;


    void stateChange();
    void channelChange(int index);
    void speedChange(int index);

    void timeScaleChange(int index);
    void playDataFile();

signals:
    void plotState(bool);
    void plotSpeed(unsigned long);
    void signalPlayFile(QString fileName);

private:
    QComboBox *timeScale;
    QComboBox *channelSelect;
    QComboBox *speedSelect;

    QPushButton *startButton;
    QPushButton *selectPlayFile;
    QLabel *playState;


    bool plotFlag;
    unsigned long  speed;




};

#endif // TIMECHARTPLOT_H
