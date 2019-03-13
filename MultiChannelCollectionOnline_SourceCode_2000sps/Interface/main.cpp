#include "multichannelcollectiononline.h"
#include <QApplication>
#include <QMessageBox>
#include <time.h>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MultiChannelCollectionOnline w;
    w.setWindowTitle("多通道在线采集系统");
    w.showMaximized();
    return a.exec();
}
