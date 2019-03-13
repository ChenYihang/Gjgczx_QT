#ifndef FILEDEAL_H
#define FILEDEAL_H

#include <QObject>
#include <QRunnable>
#include <QThread>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QByteArrayMatcher>
#include "multifunc.h"

#include "common.h"
//use threadpool


class fileDeal : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit fileDeal(QString Path, const QByteArray &data, dataType dType, QObject *parent = nullptr);
protected:
    void run();

signals:

private:
    QByteArray rcvData;
    QString savePath;
    enum dataType dType;




public slots:
};

#endif // FILEDEAL_H
