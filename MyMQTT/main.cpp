#include "mymqtt.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyMQTT w;
    w.show();

    return a.exec();
}
