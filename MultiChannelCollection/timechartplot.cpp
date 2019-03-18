#include "timechartplot.h"



timeChartPlot::timeChartPlot(QWidget *parent) :QGraphicsView(new QGraphicsScene, parent)
{

    chart = new QChart();
    for(int i=0;i<8;i++){
        chartElement.series[i] = new QLineSeries;
         chartElement.series[i]->setName("Channel "+QString::number(i+1,10));

          chartElement.yValueLabel[i] = new QGraphicsSimpleTextItem(chart);
    }
    chartElement.yValueLabel[8] = new QGraphicsSimpleTextItem(chart);
    chartElement.yValueLabel[9] = new QGraphicsSimpleTextItem(chart);
    chartElement.yValueLabel[10] = new QGraphicsSimpleTextItem(chart);


    chartElement.XplotRange = 200;
    chartElement.YplotRange = 40;

    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);
    m_coordX = new QGraphicsSimpleTextItem(chart);
    m_coordY = new QGraphicsSimpleTextItem(chart);






}

timeChartPlot::~timeChartPlot()
{
}

void timeChartPlot::resizeEvent(QResizeEvent *event)
{
    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
         chart->resize(event->size());

    }
    QGraphicsView::resizeEvent(event);
}


void timeChartPlot::mouseMoveEvent(QMouseEvent *event){

    m_coordX->setText(QString("X: %1").arg(chart->mapToValue(event->pos()).x()));

     double temp = fmod(chart->mapToValue(event->pos()).y(),5);
     if(temp>2.5){
         temp =temp-2.5;
         m_coordY->setText(QString("Y: %1").arg(temp));
    }
     else{

         temp=2.5-temp;
         m_coordY->setText(QString("Y: -%1").arg(temp));
        }


    QGraphicsView::mouseMoveEvent(event);

}

void timeChartPlot::chartInit(){

    QObject::connect(&m_timer, &QTimer::timeout, this, &timeChartPlot::handleTimeout);
    m_timer.setInterval(10);

   // chart->setTitle("8 Channel TimeChart");

    chartElement.axisX = new QValueAxis;
    chartElement.axisX->setTickCount(11);
    chartElement.axisX->setLabelFormat("%.0f");
    chartElement.axisX->setRange(0,chartElement.XplotRange);

    chart->addAxis(chartElement.axisX, Qt::AlignBottom);
    chartElement.axisY= new QValueAxis;
    chartElement.axisY->setTickCount(17);
    chartElement.axisY->setRange(0,chartElement.YplotRange);
    chartElement.axisY->setLabelsVisible(false);
    chartElement.axisY->setLabelFormat("%2.0f");
    chart->addAxis(chartElement.axisY, Qt::AlignRight);

    chart->createDefaultAxes();
    for(int i=0;i<8;i++){


         chart->addSeries( chartElement.series[i]);
         chartElement.series[i]->attachAxis(chartElement.axisX);
         chartElement.series[i]->attachAxis(chartElement.axisY);
         chartElement.series[i]->setUseOpenGL(true);

    }

   chartElement.axisY_3 = new QCategoryAxis;
    chartElement.axisY_3->append("C1", 5);
    chartElement.axisY_3->append("C2", 10);
    chartElement.axisY_3->append("C3", 15);
    chartElement.axisY_3->append("C4", 20);
    chartElement.axisY_3->append("C5", 25);
    chartElement.axisY_3->append("C6", 30);
    chartElement.axisY_3->append("C7", 35);
    chartElement.axisY_3->append("C8", 40);
    chartElement.axisY_3->setRange(0,chartElement.YplotRange);
    QPen
    pen(QColor(31, 69, 110),
    0.5,
    Qt::DashLine
    );

    m_coordX->setPos(690,14);
    m_coordY->setPos(758,14);


    chartElement.axisY_3->setGridLinePen(pen);
    chart->addAxis(chartElement.axisY_3, Qt::AlignLeft);

    for(int i=0;i<8;++i){

     chartElement.yValueLabel[i]->setText("0.0");

      chartElement.yValueLabel[i]->setPos(838,55+i*66);

    }



    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setMargins(QMargins(0,10,0,0));



    scene()->addItem(chart);
//    m_timer.start();


}
void timeChartPlot::handleTimeout(){
    qint64 range = 200;

    for(int i =0;i<8;i++){
        QVector<QPointF> oldPoints =  chartElement.series[i]->pointsVector();
        QVector<QPointF> points;
        if (oldPoints.count() < range) {
            points =  chartElement.series[i]->pointsVector();
        } else {

            for (int i = 1; i < oldPoints.count(); i++){
                points.append(QPointF(i-1, oldPoints.at(i).y()));
            }
        }
        qint64 size = points.count();
        points.append(QPointF(size, qrand()%3+1+i*5));

         chartElement.series[i]->replace(points);


    }

}

void timeChartPlot::updateTimeChart(QList<qreal> *timeData[8]){
   // qDebug()<<timeYdata[0];
   // qDebug()<<timeYdata[1];

    for(int i =0;i<8;i++){
        QVector<QPointF> oldPoints =  chartElement.series[i]->pointsVector();
        QVector<QPointF> points;
        if (oldPoints.count() < chartElement.XplotRange) {
            points =  chartElement.series[i]->pointsVector();
        } else {

            for (int j = 64; j < oldPoints.count(); j++){
                points.append(QPointF(j-64, oldPoints.at(j).y()));
            }
        }

        qint64 size = points.count();
       // qDebug()<<size;
        for (int k = 0; k <64; k++)
              points.append(QPointF(k+size, timeData[i]->at(k)));
            //points.append(QPointF(k+size, 1+i*5.0));

         chartElement.series[i]->replace(points);


    }

}



timeWidget::timeWidget(QWidget *parent) :QWidget(parent),timeScale(createTimeScaleBox()),
    channelSelect(createChSelectBox()),startButton(createStartButton()),selectPlayFile(createFileButton()),speedSelect(createSPSelectBox()),plotFlag(false),speed(100)
{

    timeChart = new timeChartPlot;
    playState = new QLabel("");
    timeChart->chartInit();
    this->initWidget();





}
void timeWidget::initWidget(){

   QVBoxLayout *baseLayout = new QVBoxLayout();
   QHBoxLayout *settingsLayout = new QHBoxLayout();
   settingsLayout->addWidget(selectPlayFile);
   settingsLayout->addSpacing(20);
   settingsLayout->addWidget(startButton);
   settingsLayout->addSpacing(50);
   settingsLayout->addWidget(new QLabel("Channel:"));
   settingsLayout->addWidget(channelSelect);
   settingsLayout->addSpacing(20);

   settingsLayout->addWidget(new QLabel("TimeScale:"));
   settingsLayout->addWidget(timeScale);
   settingsLayout->addSpacing(20);

   settingsLayout->addWidget(new QLabel("Speed:"));
   settingsLayout->addWidget(speedSelect);
   settingsLayout->addSpacing(20);


   settingsLayout->addWidget(playState);

   settingsLayout->addStretch();
   baseLayout->addLayout(settingsLayout,0);
   baseLayout->addWidget(timeChart);
  // qDebug()<<timeChart->size();

   this->setLayout(baseLayout);

}



timeWidget::~timeWidget(){



}


QComboBox *timeWidget::createTimeScaleBox() const{

    QComboBox *timeScale = new QComboBox;
    timeScale->addItem("20Pt(2us)");
    timeScale->addItem("200Pt(20us)");
    timeScale->addItem("2000Pt(200us)");
    connect(timeScale, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&timeWidget::timeScaleChange);
    return timeScale;

}

QComboBox *timeWidget::createSPSelectBox() const{

    QComboBox *speedSelect = new QComboBox;
    speedSelect->addItem("10KB/S");
    speedSelect->addItem("20KB/S");
    speedSelect->addItem("40KB/S");
    connect(speedSelect, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&timeWidget::speedChange);
    return speedSelect;

}

QComboBox *timeWidget::createChSelectBox() const{

    QComboBox *channelSelect = new QComboBox;
    channelSelect->addItem("ALL");
    channelSelect->addItem("CH1");
    channelSelect->addItem("CH2");
    channelSelect->addItem("CH3");
    channelSelect->addItem("CH4");
    channelSelect->addItem("CH5");
    channelSelect->addItem("CH6");
    channelSelect->addItem("CH7");
    channelSelect->addItem("CH8");
    connect(channelSelect, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&timeWidget::channelChange);

    return channelSelect;
}
QPushButton *timeWidget::createStartButton() const {

    QPushButton *startButton = new QPushButton("Start");
    connect(startButton,&QPushButton::clicked,this,&timeWidget::stateChange);
    return startButton;



}
QPushButton *timeWidget::createFileButton() const {

    QPushButton *fileButton = new QPushButton("Select File");
    connect(fileButton,&QPushButton::clicked,this,&timeWidget::playDataFile);
    return fileButton;

}


void timeWidget::playDataFile(){

    QString fileFullPath;
    QFileInfo fileInfo;
    QString fileName;

    if(selectPlayFile->text()=="Select File"){
     fileFullPath = QFileDialog::getOpenFileName(this);


        if(!fileFullPath.isEmpty())
        {
            fileInfo = QFileInfo(fileFullPath);
            fileName = fileInfo.absoluteFilePath();
            selectPlayFile->setText("Cancel");
            qDebug()<<fileName;
            emit signalPlayFile(fileName);


        }
    }
    else{
        if(startButton->text()=="Start"){
        selectPlayFile->setText("Select File");
        for(int i=0;i<8;i++)
        timeChart->chartElement.series[i]->clear();
        emit signalPlayFile("Cancel");
        }
      //  timeChart->chart->close();

    }



}

void timeWidget::timeScaleChange(int index){

    timeChart->chart->setAnimationOptions(QChart::AllAnimations);

    switch (index) {
    case 0:
        timeChart->chartElement.XplotRange = 200;
        break;
    case 1:
       timeChart->chartElement.XplotRange = 2000;
        break;
    case 2:
        timeChart->chartElement.XplotRange = 20000;
        break;
    default:
        break;


    }
   timeChart->chartElement.axisX->setRange(0,timeChart->chartElement.XplotRange);


}


void timeWidget::stateChange(){

    if(startButton->text()=="Start"){
        startButton->setText("Stop");
        plotFlag = true;

    }
    else{
        startButton->setText("Start");
        plotFlag =  false;
    }
    emit plotState(plotFlag);
}

void timeWidget::speedChange(int index){

    switch (index) {
    case 0:
        speed = 100;
        break;
    case 1:
        speed = 50;
        break;
    case 2:
        speed =25;
        break;
    default:
        break;


    }

    emit plotSpeed(speed);
}


void timeWidget::updatePlayState(QString state){

    playState->setText(state);
    if(state=="Completed."){
        selectPlayFile->setText("Select File");
        startButton->setText("Start");
    }

}

void timeWidget::channelChange(int index){


     timeChart->chart->setAnimationOptions(QChart::AllAnimations);

    qDebug()<<index;
    if(index !=0){

       timeChart->chartElement.YplotRange = index *5;
      timeChart->chartElement.axisY->setRange(timeChart->chartElement.YplotRange-5,timeChart->chartElement.YplotRange);
      timeChart->chartElement.axisY->setTickCount(11);
      timeChart->chartElement.axisY_3->setRange(timeChart->chartElement.YplotRange-5,timeChart->chartElement.YplotRange);
       for(int i=0;i<11;++i){

        timeChart->chartElement.yValueLabel[i]->setText(QString::number(-2.5+i*0.5,'f',1));

         timeChart->chartElement.yValueLabel[i]->setPos(838,547-i*53);
         if(i>7)
             timeChart->chartElement.yValueLabel[i]->show();
       }


     }

    else{
        timeChart->chartElement.YplotRange = 40;
        timeChart->chartElement.axisY->setRange(0,timeChart->chartElement.YplotRange);
        timeChart->chartElement.axisY->setTickCount(17);

        timeChart->chartElement.axisY_3->setRange(0,timeChart->chartElement.YplotRange);

       for(int i=0;i<11;++i){

         timeChart->chartElement.yValueLabel[i]->setText("0.0");

        timeChart-> chartElement.yValueLabel[i]->setPos(838,55+i*66);

          if(i>7)
              timeChart->chartElement.yValueLabel[i]->hide();

        }

    }


}
