#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

#include <QWidget>
#include <QtMqtt/QMqttSubscription>
#include <QDateTime>
#include <QDebug>

namespace Ui {
class subscription;
}

class subscription : public QWidget
{
    Q_OBJECT

public:
    explicit subscription(QMqttSubscription *sub,QWidget *parent = nullptr);
    ~subscription();

    int rcvCount;
    QString subState;
    QString subTopic;
    int subqos;
    QMqttSubscription *m_sub;

public slots:
    void updateMessage(const QMqttMessage &msg);
    void updateStatus(QMqttSubscription::SubscriptionState state);
    void updataQos(quint8 qos);

signals:
     void rcvToMainUpdate();
     void staToMainUpdate();
private:
    Ui::subscription *ui;


};

#endif // SUBSCRIPTION_H
