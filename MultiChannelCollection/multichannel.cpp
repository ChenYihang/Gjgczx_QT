#include "multichannel.h"
#include "ui_multichannel.h"
#include "pcap.h"
QT_CHARTS_USE_NAMESPACE
MultiChannel::MultiChannel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiChannel),clientCount(0)
{
    ui->setupUi(this);
    MprocessHandle = CsocketProcess::instance();

    MserverHandle = CsocketServer::instance();

    connect(MprocessHandle,SIGNAL(signalAddNewClient(QString,int)),this,SLOT(slotAddNewClient(QString,int)));
    connect(MprocessHandle,SIGNAL(signalDelClient(int)),this,SLOT(slotDelClient(int)));
    connect(MprocessHandle,SIGNAL(signalToMainUI(QString)),this,SLOT(fileDealInfo(QString)));


    UIinitialization();


    pcap_if_t *alldevs;
    pcap_if_t *d;
    int i = 0;
    char errbuf[PCAP_ERRBUF_SIZE];
    if(pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        qDebug() << errbuf;
    }
    for(d = alldevs; d; d = d->next)
    {
        qDebug() << ++i << d->name;
        ui->infoShow->append(d->name);
        if(d->description)
            qDebug() << d->description;
        else
            qDebug("(No description available)");
    }
    if(0 == i)
    {
        qDebug("No interfaces found! Make sure WinPcap is installed.");
    }
    pcap_freealldevs(alldevs);







}

MultiChannel::~MultiChannel()
{
    delete ui;
}
void MultiChannel::UIinitialization(){

    ui->portEdit->setText("10000");
    QString filePath = QDir::currentPath();  //静态成员需要在类定义体外定义
    ui->currentPath->setText(filePath);

    MultiChannel::plotInit();






}


void MultiChannel::plotInit(){  //初始界面显示

   timeWidget *initalTimeWidget = new timeWidget;
   initalTimeWidget->setEnabled(false);
   ui->tabWidget->addTab(initalTimeWidget,"Time Series");
   connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(deleteTab(int)));




}
void MultiChannel::slotAddNewClient(QString qsip, int iflag){ //客户端上线



    if(clientCount == 0){  //初始状态
        ui->tabWidget->clear();
          ui->tabWidget->addTab(MprocessHandle->getClientHandle(iflag)->udCharts->tWidget,"TPlot-"+qsip);
          ui->tabWidget->currentWidget()->setWhatsThis(QString::number(iflag));
          ui->tabWidget->setTabsClosable(true);


    }
    else{
        ui->tabWidget->addTab(MprocessHandle->getClientHandle(iflag)->udCharts->tWidget,"TPlot-"+qsip);
        ui->tabWidget->setCurrentWidget(MprocessHandle->getClientHandle(iflag)->udCharts->tWidget);
        ui->tabWidget->currentWidget()->setWhatsThis(QString::number(iflag));

    }
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"新的客户端连接");

    ++clientCount;
   qDebug()<<"slotAddNewClient :"<<qsip<<iflag;


}


void MultiChannel::deleteTab(int index){  //主动删除

    bool ok;
    int iflag = ui->tabWidget->widget(index)->whatsThis().toInt(&ok,10);

   ui->tabWidget->removeTab(index);
   MprocessHandle->proClientDisconnect(iflag);

    --clientCount;
    if(clientCount ==0){  //所有客户端已下线 设置默认界面
        ui->tabWidget->setTabsClosable(false);
        timeWidget *initalTimeWidget = new timeWidget;
        initalTimeWidget->setEnabled(false);
        ui->tabWidget->addTab(initalTimeWidget,"Time Series");

    }
    qDebug()<<"deleteTab:"<<iflag<<index;


}

void MultiChannel::slotDelClient(int iflag){  //客户端主动下线


    ui->tabWidget->removeTab(iflag);
    --clientCount;

    if(clientCount ==0){
        ui->tabWidget->setTabsClosable(false);
        timeWidget *initalTimeWidget = new timeWidget;
        initalTimeWidget->setEnabled(false);
        ui->tabWidget->addTab(initalTimeWidget,"Time Series");


    }

    qDebug()<<"slotDelClient:"<<iflag;


}

void MultiChannel::fileDealInfo(QString info){

    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+info);


}

void MultiChannel::on_siglePB_clicked()
{

        if(clientCount ==0)
            return;
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("414351414351FFFFFFFFFFFFFF514341"));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");

}

void MultiChannel::on_start_clicked()
{
    if(ui->portEdit->text().isEmpty())
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"请输入端口号");
    else{

        if (ui->start->text()=="启动")
        {
            MserverHandle->serverStart(ui->portEdit->text().toInt());
            ui->start->setText("停止");
            ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"启动监听成功");

        }
        else
        {
          MserverHandle->serverStop();
          MprocessHandle->clearAllConnect();
          ui->tabWidget->clear();
          timeWidget *initalTimeWidget = new timeWidget;
          initalTimeWidget->setEnabled(false);
          ui->tabWidget->addTab(initalTimeWidget,"Time Series");
            ui->tabWidget->setTabsClosable(false);
            clientCount =0;
            ui->start->setText("启动");
            ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"停止监听成功");

        }
    }

}

void MultiChannel::on_sendPB_clicked()
{

     if(clientCount ==0)
        return;

    if (ui->hexCheck->isChecked())
    {
        QString data=ui->sendText->text();
        if (data==""){return;}  //没有输入则返回


        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex(data.toLatin1()));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
    else
    {
        QByteArray data=ui->sendText->text().toLocal8Bit();
        if (data==""){return;}  //没有输入则返回


        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(data);

        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
}

void MultiChannel::on_multiPB_clicked()
{
     if(clientCount ==0)
        return;
    MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("41435141435100000000000000514341"));
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");


}

void MultiChannel::on_startPB_clicked()
{
 if(clientCount ==0)
     return;
    MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("53544C53544CFFFFFFFFFFFFFF4C5453"));
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");

}

void MultiChannel::on_stopPB_clicked()
{
    if(clientCount ==0)
        return;
    MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("53544C53544C000000000000004C5453"));
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");

}

void MultiChannel::on_resetPB_clicked()
{
    if(clientCount ==0)
        return;
    MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("52535452455345545245534554545352"));
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");

}
