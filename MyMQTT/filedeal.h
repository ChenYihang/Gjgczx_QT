#ifndef FILEDEAL_H
#define FILEDEAL_H

#include <QObject>
#include <QRunnable>
#include <QThread>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QByteArrayMatcher>
#include <QMutex>
#include "multifunc.h"
#include "common.h"

/**
 * @brief 文件处理类
 * 使用 QT线程池
 */
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
