#include "multichannel.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MultiChannel w;
    w.show();

    return a.exec();
}
