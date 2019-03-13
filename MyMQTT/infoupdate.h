#ifndef INFOUPDATE_H
#define INFOUPDATE_H

#include <QObject>
#include <QThread>
#include "httpapi.h"
#include "common.h"
#include "jsonsolve.h"
#include <QMutex>



class infoUpdate : public QThread
{
    Q_OBJECT
public:
    explicit infoUpdate(QObject *parent = nullptr);
     ~infoUpdate();

    enum requestType{
        basicInfo = 0,
        monitorInfo,
        clientList,
        sessionsList,
        subscriptionList
    };

    int hflag;


signals:
    void requestSignal(QUrl);
    void infoToUi(QStringList,int);
public slots:

    void run();

private:

    httpAPI *requestInfo;
    jsonSolve *jsAna;


};

#endif // INFOUPDATE_H
