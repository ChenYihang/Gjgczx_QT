#ifndef JSDOCUMENT_H
#define JSDOCUMENT_H

#include <QObject>
#include <QWebEnginePage>
class jsDocument : public QObject
{
    Q_OBJECT
public:
    explicit jsDocument(QObject *parent = nullptr);

signals:
    void recvdMsg(const QString& msg);

public:

    void sendMsg(QWebEnginePage* page, const QString& msg);


public slots:

    void onMsg(const QString& msg);
};

#endif // JSDOCUMENT_H
