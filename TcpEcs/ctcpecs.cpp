#include "ctcpecs.h"
#include "ui_ctcpecs.h"

CtcpEcs::CtcpEcs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CtcpEcs){
    ui->setupUi(this);
    uiUinitialize();
    MprocessHandle = CsocketProcess::instance();
    MserverHandle = CsocketServer::instance();

    connect(MprocessHandle,SIGNAL(signalAddNewClient(QString,int)),this,SLOT(slotAddNewClient(QString,int)));
    connect(MprocessHandle,SIGNAL(signalDelClient(int)),this,SLOT(slotDelClient(int)));
    connect(MprocessHandle,SIGNAL(disPlay(QByteArray,QString)),this,SLOT(dataDisPlay(QByteArray,QString)));
    connect(MprocessHandle,SIGNAL(forward(QByteArray,QString)),this,SLOT(forwardDeal(QByteArray,QString)));

    connect(ui->treeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),this,SLOT(getCurrentClient()));

}

CtcpEcs::~CtcpEcs(){
    delete ui;
}


/*
    UI控件初始化
*/
void CtcpEcs::uiUinitialize(){

    ui->portNum->setText("10200");
    ui->ADnum->blockSignals(1);
    ui->ADrate->blockSignals(1);
    ui->DArate->blockSignals(1);

    ui->ADnum->addItem(tr(" "));
    ui->ADnum->addItem(tr("1K"));
    ui->ADnum->addItem(tr("10K"));
    ui->ADnum->addItem(tr("100K"));

    ui->ADrate->addItem(tr(" "));
    ui->ADrate->addItem(tr("2.1MSPS"));
    ui->ADrate->addItem(tr("1.4MSPS"));
    ui->ADrate->addItem(tr("0.7MSPS"));
    ui->ADrate->addItem(tr("0.35MSPS"));

    ui->DArate->addItem(tr(" "));
    ui->DArate->addItem(tr("2.00MSPS"));
    ui->DArate->addItem(tr("1.33MSPS"));
    ui->DArate->addItem(tr("0.68MSPS"));
    ui->DArate->addItem(tr("0.33MSPS"));

    ui->folders->setPlaceholderText("文件夹序号");
    ui->files->setPlaceholderText("文件序号");

    ui->ADnum->blockSignals(0);
    ui->ADrate->blockSignals(0);
    ui->DArate->blockSignals(0);

    ui->motorCtrl->setEnabled(1);
    ui->configure->setEnabled(1);
    ui->pulseCtrl->setEnabled(1);

    ui->treeWidget->expandAll();
    currentClient = -1;
    speedflag = 0;

    ui->progressBar->setMinimum(0);

    ui->doubleSpinBox->setFocusPolicy(Qt::NoFocus);
    ui->doubleSpinBox_2->setFocusPolicy(Qt::NoFocus);

}

void CtcpEcs::on_start_clicked(){    //启动监听函数
    if(ui->portNum->text().isEmpty())
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"请输入端口号");
    else{

        if (ui->start->text()=="启动")
        {
            MserverHandle->serverStart(ui->portNum->text().toInt());
            ui->start->setText("停止");
            ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"启动监听成功");
            ui->infoShow->append("["+multifunc::GetLongDateTime()+"]默认路径:"+QDir::currentPath());

        }
        else
        {
          MserverHandle->serverStop();
          MprocessHandle->clearAllConnect();
            ui->start->setText("启动");
            ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"停止监听成功");

        }
    }

}

/*
    treeWidget相关槽
*/

void CtcpEcs::slotAddNewClient(QString qsip, int iflag){

 QTreeWidgetItem* clientNum = new QTreeWidgetItem();
 clientNum->setText(0, "序号-"+QString::number(iflag,10));
 ui->treeWidget->addTopLevelItem(clientNum);
 clientNum->setTextColor(0,Qt::blue);


 QTreeWidgetItem* clientIP = new QTreeWidgetItem();
 clientIP->setText(0, "IP-"+qsip);
 clientNum->addChild(clientIP);


 ui->treeWidget->expandAll();

 ui->treeWidget->setCurrentItem(clientNum);


}

/*
    treeWidget相关槽
*/
void CtcpEcs::slotDelClient(int iflag){

    QTreeWidgetItemIterator it(ui->treeWidget);
        while (*it) {
         QStringList qsl = (*it)->text(0).split("-");
            int itemp = qsl.at(1).toInt();
         // qDebug()<<a.toInt();
         if(iflag == itemp){
             ui->treeWidget->removeItemWidget(*it,0);

             delete (*it)->child(0);  //删除子节点
             delete *it;            //删除顶层节点
             break;
         }
         //不需要遍历下层节点 目前有一个子节点
         ++it;
         ++it;
        }
}

void CtcpEcs::dataDisPlay(QByteArray socketData,QString ip){

   // qDebug()<<socketData;
    if(socketData == "CREATFILE"){
       ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+tr("创建文件成功,正在接收文件..[IP:%1]").arg(ip));
    }
    else if(socketData == "SHIPDATA"){
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+tr("船数据写入完成！[IP:%1]").arg(ip));
    if(ui->upload2->text()=="取消上传")
        ui->upload2->setText("上传采集数据");
        if(ui->upload3->text()=="取消上传")
            ui->upload3->setText("上传历史数据");
    }
    else if(socketData == "PULSEDATA"){
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+tr("脉搏数据写入完成[IP:%1]").arg(ip));
    }
    else if(socketData == "AUDIODATA"){
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+tr("音频数据写入完成[IP:%1]").arg(ip));
    }
    else if(socketData.startsWith("$W")&&socketData.endsWith("W$")){  // \x68 \x4f\x5a
        forwardDeal(socketData,0);
        if (ui->ckHexReceive->isChecked()){
             ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+tr("转发[IP:%1] 数据").arg(ip));
           //  ui->infoShow->append(QString(multifunc::ByteArrayToHexStr(socketData)));
        }
        else
        {
            ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+tr("转发[IP:%1] 数据").arg(ip));
        //  ui->infoShow->append(QString::fromLocal8Bit(socketData));
        }
        return;
    }
    else{
        if (ui->ckHexReceive->isChecked()){
             ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+tr("接收来自 [IP:%1] 数据:").arg(ip));
             ui->infoShow->append(QString(multifunc::ByteArrayToHexStr(socketData)));
        }
        else
        {
            ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+tr("接收来自 [IP:%1] 数据:").arg(ip));
          ui->infoShow->append(QString::fromLocal8Bit(socketData));
        }
    }

}


void CtcpEcs::forwardDeal(QByteArray cmd,QString ip){

    if(cmd.startsWith("$W"))  //船体 => 服务器 => PAD
    {
        qDebug()<<cmd;
        QTreeWidgetItemIterator it(ui->treeWidget);
        while(*it){

              if((*it)->text(0).contains("Ctrl")){
                  QStringList temp = (*it)->text(0).split("-");
                        qDebug()<<temp.at(1).toInt();
                      MprocessHandle->getClientHandle(temp.at(1).toInt())->sendData(cmd);


              }
              ++it;
          }


      }
      QList<QByteArray>list;
      list = cmd.split(':');
     // qDebug()<<list.size();
      if(list.at(0)=="#clogin"){ //控制器注册包
          QTreeWidgetItem* tItem = ui->treeWidget->currentItem()->parent();
           if(!tItem){
               QString temp = ui->treeWidget->currentItem()->text(0);
               if(temp.contains("Ctrl"))
                   return;
               ui->treeWidget->currentItem()->setText(0,temp.append("-Ctrl-").append(list.at(1).left(2)));
           }
           else{
               QString temp = ui->treeWidget->currentItem()->parent()->text(0);
               if(temp.contains("Ctrl"))
                   return;
                ui->treeWidget->currentItem()->parent()->setText(0,temp.append("-Ctrl-").append(list.at(1).left(2)));
           }
      }
      else if(list.at(0)=="#slogin"){  //船体注册包

          QTreeWidgetItem* tItem = ui->treeWidget->currentItem()->parent();
           if(!tItem){
               QString temp = ui->treeWidget->currentItem()->text(0);
               if(temp.contains("Ship"))
                   return;
               ui->treeWidget->currentItem()->setText(0,temp.append("-Ship-").append(list.at(1).left(2)));
           }
           else{
               QString temp = ui->treeWidget->currentItem()->parent()->text(0);
               if(temp.contains("Ship"))
                   return;
                ui->treeWidget->currentItem()->parent()->setText(0,temp.append("-Ship-").append(list.at(1).left(2)));
           }

      }
      else if(list.at(0)=="#ctrl"){
          if(list.size()!=3)
              return;
          QByteArray cmd = list.at(2);
          int id = list.at(1).toInt();

        //  qDebug()<<"id= "<<id;
          cmd.remove(cmd.size()-1,1);


          QTreeWidgetItemIterator it(ui->treeWidget);
          while(*it){

              if((*it)->text(0).contains("Ship")){
                  QStringList temp = (*it)->text(0).split("-");
               //   qDebug()<<temp;
                     int itemp = temp.last().toInt();
                     if(itemp == id){//正确的id号
                       //  qDebug()<<itemp;
                      //   qDebug()<<temp.at(1).toInt();
                         forwardFlag = id;
                         if(cmd=="D100D1"){
                             MprocessHandle->getClientHandle(currentClient)->initTrans(4);   //传感器数据上传

                         }
                      MprocessHandle->getClientHandle(temp.at(1).toInt())->sendData(multifunc::HexStrToByteArray(cmd));

                     }

              }
              ++it;
          }
      }

}

//获取当前选中的下位机序号
void CtcpEcs::getCurrentClient(){

    if(!ui->treeWidget->currentItem())  //return when empty;
        return;
    QTreeWidgetItem* tItem = ui->treeWidget->currentItem()->parent();
     if(!tItem){
        // qDebug()<<"parent";
         QStringList qsl = ui->treeWidget->currentItem()->text(0).split("-");
        currentClient = qsl.at(1).toInt();
     }
     else{
       // qDebug()<<"child";
         QStringList qsl = ui->treeWidget->currentItem()->parent()->text(0).split("-");
        //ui->treeWidget->setCurrentItem(ui->treeWidget->currentItem()->parent());
         currentClient = qsl.at(1).toInt();
     }
     qDebug()<<currentClient;

}


void CtcpEcs::on_del_clicked()
{

    if(!ui->treeWidget->currentItem())  //return when empty;
        return;

   MprocessHandle->proClientDisconnect(currentClient);
   QTreeWidgetItem* tItem = ui->treeWidget->currentItem()->parent();
    if(!tItem){
        qDebug()<<"parent";
        delete ui->treeWidget->currentItem()->child(0);
        delete ui->treeWidget->currentItem();
    }
    else{
       qDebug()<<"child";
       delete ui->treeWidget->currentItem()->parent();
       delete ui->treeWidget->currentItem();

    }
}




void CtcpEcs::on_Send_clicked()
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;

    if (ui->ckHexSend->isChecked())
    {
        QString data=ui->Datasend->text();
        if (data==""){return;}  //没有输入则返回

        if(ui->NewLine->isChecked())   //自动换行
        data=data+multifunc::ByteArrayToHexStr("\r\n");

        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(data));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
    else
    {
        QByteArray data=ui->Datasend->text().toLocal8Bit();
        if (data==""){return;}  //没有输入则返回

        if(ui->NewLine->isChecked())   //自动换行
        data=data+"\r\n";

        MprocessHandle->getClientHandle(currentClient)->sendData(data);

        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
}

void CtcpEcs::on_single_clicked()
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;
    QString singel = "AA 00 AA";

    MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(singel));
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"单次采集指令发送成功");
}

void CtcpEcs::on_upload2_clicked()
{
    if(ui->upload2->text()=="取消上传"){
        ui->upload2->setText("上传采集数据");
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"取消上传");

        MprocessHandle->getClientHandle(currentClient)->initTrans(0);
        return;

    }
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;
    QString upload_2 = "AA 00 AA D2 00 D2";
    MprocessHandle->getClientHandle(currentClient)->initTrans(1);  //设置数据接收模式
    if(ui->upload2->text()=="上传采集数据")
        ui->upload2->setText("取消上传");
    MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(upload_2));

}

void CtcpEcs::on_upload1_clicked()
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;
    QString upload_1 = "D1 00 D1";

    MprocessHandle->getClientHandle(currentClient)->initTrans(4);   //传感器数据上传

    MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(upload_1));
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
}




void CtcpEcs::on_Stop_clicked()
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;
    MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4353FD00FD"));

    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
}

void CtcpEcs::on_pulseStart_clicked()
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;
    MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("AD00AD"));

    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
}

void CtcpEcs::on_PulseStop_clicked()
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;
    MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("ADFFAD"));

    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
}

void CtcpEcs::on_pulseReset_clicked()
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;
    MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("660066"));

    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
}

void CtcpEcs::on_upload3_clicked()
{
    if(ui->upload3->text()=="取消上传"){
          ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"取消上传");
        ui->upload3->setText("上传历史数据");
        MprocessHandle->getClientHandle(currentClient)->initTrans(0);
        return;

    }
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;

    if(ui->folders->text()==""||ui->files->text()=="")
       return;
    else
    {   //上传历史数据
        if(ui->upload3->text()=="上传历史数据")
            ui->upload3->setText("取消上传");
        MprocessHandle->getClientHandle(currentClient)->initTrans(1);  //设置数据接收模式
        QString folder =ui->folders->text();
        QString file =ui->files->text();
        int temp1 = multifunc::StrDecimalToDecimal(folder);
        int temp2 = multifunc::StrDecimalToDecimal(file);
        QString upload_3="D3";
        QString t1 = multifunc::IntDecToStrHex(temp1);
        QString t2 = multifunc::IntDecToStrHex(temp2);
       if(temp1<16&&temp1>0)
        upload_3+=" 0";
       else
         upload_3+=" ";
        upload_3+=t1;
        if(temp2<16&&temp2>0)
        upload_3+=" D1 D3 0";
        else
        upload_3+=" D1 D3 ";

        upload_3+=t2;
        upload_3+=" D2";
        qDebug()<<upload_3;
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(upload_3));

        //ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
}

void CtcpEcs::on_setpath_clicked()
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;
    QString filePath = QFileDialog::
            getExistingDirectory(this,tr("设置文件保存目录"),".", QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(filePath.size() == 0)
        return;
    filePath += "/";
     MprocessHandle->getClientHandle(currentClient)->setFile(filePath);
}

void CtcpEcs::on_pulseUpload_clicked()
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;
//    if(pulsePath.size()==0){
//        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"需要设置保存路径");
//        return;
//    }
    if(ui->sizeSet->text()=="")
       return;

    QString size =ui->sizeSet->text();

    int temp1 = multifunc::StrDecimalToDecimal(size);
    if(temp1 >65535)
        return;

    MprocessHandle->getClientHandle(currentClient)->initTrans(2);  //设置数据接收模式

   unsigned int pulseFileSize = temp1*60*1024;
   // unsigned int pulseFileSize = 1577;
    MprocessHandle->getClientHandle(currentClient)->setPulseSize(pulseFileSize);



    QString hexStr = multifunc::IntDecToStrHex(temp1);
    QString head,tail;
  //  qDebug()<<hexStr;
    if(hexStr.size()<3){  //0-255
        head="00";
         if(temp1<16&&temp1>0)
                tail="0"+hexStr;
         else
             tail=hexStr;
    }
    else if(hexStr.size() == 3){
        head = "0"+ hexStr.mid(0,1);
        tail = hexStr.mid(1,2);
    }
    else{
        head = hexStr.mid(0,2);
        tail = hexStr.mid(2,2);
    }

    QString realStr = "D3"+head+"D1D3"+tail+"D2";
    MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(realStr));
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");



}


void CtcpEcs::on_ADnum_currentIndexChanged(const QString &arg1)
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;

    if(ui->ADnum->currentText()==" ")
        return;

    else if(ui->ADnum->currentText()=="1K")
    {
        QString anum = "AC 08 AC";
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(anum));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
    else if(ui->ADnum->currentText()=="10K")
    {
        QString anum = "AC 10 AC";
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(anum));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
    else if( ui->ADnum->currentText()=="100K")
    {
        QString anum = "AC 20 AC";
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(anum));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
    else
        return;
}

void CtcpEcs::on_ADrate_currentIndexChanged(const QString &arg1)
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;

    if(ui->ADrate->currentText()==" ")
        return;

    else if(ui->ADrate->currentText()=="2.1MSPS")
    {
        QString arate = "AD 00 AD";
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(arate));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");

    }
    else if(ui->ADrate->currentText()=="1.4MSPS")
    {
        QString arate = "AD 02 AD";
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(arate));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
    else if(ui->ADrate->currentText()=="0.7MSPS")
    {
        QString arate = "AD 04 AD";
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(arate));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
    else if(ui->ADrate->currentText()=="0.35MSPS")
    {
        QString arate = "AD 08 AD";
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(arate));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
    else
        return;
}

void CtcpEcs::on_DArate_currentIndexChanged(const QString &arg1)
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;

    if(ui->DArate->currentText()==" ")
        return;

    else if(ui->DArate->currentText()=="2.00MSPS")
    {
        QString drate = "DA 00 DA";
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(drate));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
    else if(ui->DArate->currentText()=="1.33MSPS")
    {
        QString drate = "DA 02 DA";
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(drate));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
    else if(ui->DArate->currentText()=="0.68MSPS")
    {
        QString drate = "DA 04 DA";
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(drate));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
    else if(ui->DArate->currentText()=="0.33MSPS")
    {
        QString drate = "DA 08 DA";
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(drate));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
    else
        return;
}

void CtcpEcs::on_SendDataFile_clicked(){

    if(!ui->treeWidget->currentItem())  //return when empty;
        return;

    QString audioFileFullPath;
    QFileInfo audioFileInfo;
    QString audioFileName;

    if(ui->SendDataFile->text()=="下发音频文件")
    audioFileFullPath = QFileDialog::getOpenFileName(this);

    if(ui->SendDataFile->text()=="发送中")
        {
        ui->SendDataFile->setText("下发音频文件");  //发送不成功时复位按键
        audioFile->close();
        //return;

        }

    if(!audioFileFullPath.isEmpty())
    {
        int i = 0;
        audioFileInfo = QFileInfo(audioFileFullPath);
        audioFileName = audioFileInfo.absoluteFilePath();
        ui->SendDataFile->setText("发送中");
        audioFile=new QFile(audioFileName);
        audioFile->open(QIODevice::ReadOnly);
        qDebug()<<audioFile->size();
        ui->progressBar->setMaximum(audioFile->size()/794);
        ui->progressBar->setValue(0);
        do{
        QByteArray audioPack =audioFile->read(794); //返回QBytearrary;
        if(audioFile->pos() ==audioFile->size() ){ //最后一个数据包
            audioPack.append("%$#@");
            qDebug()<<audioPack;
        }
        MprocessHandle->getClientHandle(currentClient)->sendData(audioPack);
        ui->progressBar->setValue(++i);
        //qDebug()<<"send";
       //  qDebug()<<audioFile->pos();
        }while(audioFile->pos()<audioFile->size());

        ui->SendDataFile->setText("下发音频文件");
       ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");

    }


}

void CtcpEcs::on_SendDataFile_DA_clicked(){

    if(!ui->treeWidget->currentItem())  //return when empty;
        return;

    QString DAFileFullPath;
    QFileInfo DAFileInfo;
    QString DAFileName;

    if(ui->SendDataFile_DA->text()=="下发DA文件")
    DAFileFullPath = QFileDialog::getOpenFileName(this);

    if(ui->SendDataFile_DA->text()=="发送中")
        {
        ui->SendDataFile_DA->setText("下发DA文件");  //发送不成功时复位按键
        DAFile->close();
        //return;

        }

    if(!DAFileFullPath.isEmpty())
    {
        int i = 0;
        DAFileInfo = QFileInfo(DAFileFullPath);
        DAFileName = DAFileInfo.absoluteFilePath();
        ui->SendDataFile_DA->setText("发送中");
        DAFile=new QFile(DAFileName);

        if(DAFileInfo.suffix()=="dat")    //判断文件后缀名，.dat则不能使用TEXT_IODEVICE格式打开，否则会丢失0x0d字符，导致文件发送出错
            DAFile->open(QIODevice::ReadOnly);
        else
        DAFile->open(QIODevice::ReadOnly | QIODevice::Text);//读取DA数据源文件，格式为TXT
        ui->progressBar->setMaximum(DAFile->size()/794);
        ui->progressBar->setValue(0);

        do{
        QByteArray DAPack =DAFile->read(794); //返回QBytearrary;
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(DAPack));
        ui->progressBar->setValue(++i);

        }while(DAFile->pos()<DAFile->size());

        ui->progressBar->setValue(DAFile->size()/794);

       ui->SendDataFile_DA->setText("下发DA文件");
       ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");

    }
}

void CtcpEcs::on_infoShow_textChanged()
{
    ui->infoShow->moveCursor(QTextCursor::End);
}

void CtcpEcs::on_Clear_clicked()
{
    ui->infoShow->clear();
}


void CtcpEcs::on_verticalSlider_valueChanged(int value){




    if(value>=1&&value <=25)
        speedflag = 1;
    else if(value >=26&& value<=50)
        speedflag = 2;
    else if(value >=51 && value <=75)
        speedflag =3;
    else if(value >=76 && value <=100)
        speedflag =4;
    else if(value >=101 && value <=125)
        speedflag =5;
    else if(value >=126 && value <=150)
        speedflag =6;
    else if(value >=151 && value <=175)
        speedflag =7;
    else if(value >=176 && value <=200)
        speedflag =8;
    else if(value >=-25&& value<=-1)
        speedflag = -1;
    else if(value >=-50&& value<=-26)
        speedflag = -2;
    else if(value >=-75 && value <=-51)
        speedflag =-3;
    else if(value >=-100 && value <=-76)
        speedflag =-4;
    else if(value >=-125 && value <=-101)
        speedflag =-5;
    else if(value >=-150&& value<=-126)
        speedflag = -6;
    else if(value >=-175&& value<=-151)
        speedflag = -7;
    else if(value >=-200 && value <=-176)
        speedflag =-8;
    else
        speedflag = 0;

    ui->label_11->setText(tr("%1\%").arg(speedflag*12.5));
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;
    if(speedflag == 1 && speedtemp!=1){

        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4601FD00FD"));
        speedtemp = 1;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 2 && speedtemp !=2){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4602FD00FD"));
        speedtemp = 2;
        qDebug()<<speedtemp;

    }
    else if(speedflag == 3 && speedtemp !=3){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4603FD00FD"));
        speedtemp = 3;
        qDebug()<<speedtemp;

    }
    else if(speedflag == 4 && speedtemp !=4){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4604FD00FD"));
        speedtemp = 4;
        qDebug()<<speedtemp;

    }
    else if(speedflag == 5 && speedtemp !=5){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4605FD00FD"));
        speedtemp =5;
        qDebug()<<speedtemp;

    }
    else if(speedflag == 6 && speedtemp !=6){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4606FD00FD"));
        speedtemp = 6;
        qDebug()<<speedtemp;

    }
    else if(speedflag == 7 && speedtemp !=7){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4607FD00FD"));
        speedtemp = 7;
        qDebug()<<speedtemp;

    }
    else if(speedflag == 8 && speedtemp !=8){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4608FD00FD"));
        speedtemp = 8;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -1 && speedtemp != -1){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4201FD00FD"));
        speedtemp = -1;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -2 && speedtemp !=-2){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4202FD00FD"));
        speedtemp = -2;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -3 && speedtemp !=-3){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4203FD00FD"));
        speedtemp = -3;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -4 && speedtemp != -4){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4204FD00FD"));
        speedtemp = -4;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -5 && speedtemp != -5){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4205FD00FD"));
        speedtemp = -5;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -6 && speedtemp !=-6){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4206FD00FD"));
        speedtemp = -6;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -7 && speedtemp !=-7){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4207FD00FD"));
        speedtemp = -7;
        qDebug()<<speedtemp;

    }
    else if(speedflag == -8 && speedtemp != -8){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4208FD00FD"));
        speedtemp = -8;
        qDebug()<<speedtemp;

    }


}


void CtcpEcs::on_horizontalSlider_valueChanged(int value)
{



    if(value>=1&&value <=25)
        speedflag_lr = 1;
    else if(value >=26&& value<=50)
        speedflag_lr = 2;
    else if(value >=51 && value <=75)
        speedflag_lr =3;
    else if(value >=76 && value <=100)
        speedflag_lr =4;
    else if(value >=101 && value <=125)
        speedflag_lr =5;
    else if(value >=126 && value <=150)
        speedflag_lr =6;
    else if(value >=151 && value <=175)
        speedflag_lr =7;
    else if(value >=176 && value <=200)
        speedflag_lr =8;
    else if(value >=-25&& value<=-1)
        speedflag_lr = -1;
    else if(value >=-50&& value<=-26)
        speedflag_lr = -2;
    else if(value >=-75 && value <=-51)
        speedflag_lr =-3;
    else if(value >=-100 && value <=-76)
        speedflag_lr =-4;
    else if(value >=-125 && value <=-101)
        speedflag_lr =-5;
    else if(value >=-150&& value<=-126)
        speedflag_lr = -6;
    else if(value >=-175&& value<=-151)
        speedflag_lr = -7;
    else if(value >=-200 && value <=-176)
        speedflag_lr =-8;
    else
        speedflag_lr = 0;

     ui->label_12->setText(tr("%1\%").arg(speedflag_lr*12.5));

    if(!ui->treeWidget->currentItem())  //return when empty;
        return;
    if(speedflag_lr == 1 && speedtemp_lr!=1){

        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B5201FD00FD"));
        speedtemp_lr = 1;
        qDebug()<<speedtemp_lr;
    }
    else if(speedflag_lr == 2 && speedtemp_lr !=2){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B5202FD00FD"));
        speedtemp_lr = 2;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 3 && speedtemp_lr !=3){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B5203FD00FD"));
        speedtemp_lr = 3;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 4 && speedtemp_lr !=4){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B5204FD00FD"));
        speedtemp_lr = 4;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 5 && speedtemp_lr !=5){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B5205FD00FD"));
        speedtemp_lr =5;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 6 && speedtemp_lr !=6){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B5206FD00FD"));
        speedtemp_lr = 6;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 7 && speedtemp_lr !=7){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B5207FD00FD"));
        speedtemp_lr = 7;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 8 && speedtemp_lr !=8){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B5208FD00FD"));
        speedtemp_lr = 8;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -1 && speedtemp_lr != -1){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4C01FD00FD"));
        speedtemp_lr = -1;
       qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -2 && speedtemp_lr !=-2){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4C02FD00FD"));
        speedtemp_lr = -2;
       qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -3 && speedtemp_lr !=-3){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4C03FD00FD"));
        speedtemp_lr = -3;
       qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -4 && speedtemp_lr != -4){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4C04FD00FD"));
        speedtemp_lr = -4;
      qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -5 && speedtemp_lr != -5){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4C05FD00FD"));
        speedtemp_lr = -5;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -6 && speedtemp_lr !=-6){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4C06FD00FD"));
        speedtemp_lr = -6;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -7 && speedtemp_lr !=-7){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4C07FD00FD"));
        speedtemp_lr = -7;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -8 && speedtemp_lr != -8){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B4C08FD00FD"));
        speedtemp_lr = -8;
        qDebug()<<speedtemp_lr;

    }

}



void CtcpEcs::on_upload1_2_clicked()
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;

    if(ui->upload1_2->text()=="实时上传"){
        ui->upload1_2->setText("停止上传");
        QString upload = "D4 00 D4";
        MprocessHandle->getClientHandle(currentClient)->initTrans(5);   //传感器数据上传
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(upload));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
    else{
          ui->upload1_2->setText("实时上传");
          QString upload = "D4 FF D4";
          MprocessHandle->getClientHandle(currentClient)->initTrans(6);   //停止上传标志位
          MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray(upload));
          ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");

    }

}

void CtcpEcs::on_doubleSpinBox_valueChanged(double arg1)
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;
    static double helper = 0;
    if(arg1 > helper)
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B6C46FD00FD"));
    else
         MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B6C52FD00FD"));
    helper = arg1;
}

void CtcpEcs::on_doubleSpinBox_2_valueChanged(double arg1)
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;
    static double helper1 = 0;
    if(arg1 > helper1){
        MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B7246FD00FD"));
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");


    }
    else{
         MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF2B7252FD00FD"));
         ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");

    }helper1 = arg1;
}

void CtcpEcs::on_horizontalSlider_actionTriggered(int action)
{

}

void CtcpEcs::on_motorReset_clicked()
{
    if(!ui->treeWidget->currentItem())  //return when empty;
        return;
    MprocessHandle->getClientHandle(currentClient)->sendData(multifunc::HexStrToByteArray("DF00DF110011FD00FD"));

    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
}
