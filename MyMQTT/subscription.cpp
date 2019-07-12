#include "subscription.h"
#include "ui_subscription.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

subscription::subscription(QMqttSubscription *sub,QWidget *parent):
    QWidget(parent),
    ui(new Ui::subscription),
    m_sub(sub)
{
    rcvCount = 0;
    ui->setupUi(this);
    subTopic = sub->topic().filter();
    subqos = sub->qos();
    this->setWindowTitle(tr("主题: %1").arg(subTopic));
    updateStatus(m_sub->state());
    connect(m_sub, &QMqttSubscription::messageReceived, this, &subscription::updateMessage);
    connect(m_sub, &QMqttSubscription::stateChanged, this, &subscription::updateStatus);
    connect(m_sub,&QMqttSubscription::qosChanged,this,&subscription::updataQos);
}

subscription::~subscription()
{
    delete ui;
    qDebug()<<"del";
}

void subscription::updateMessage(const QMqttMessage &msg)
{
    //ui->listWidget->addItem(msg.payload());
    rcvCount++;
    const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss  ")
                    + QLatin1String(msg.payload());

    ui->msgShow->appendPlainText(content);

    qDebug()<<msg.topic();
    qDebug()<<msg.payload().length();
    emit rcvToMainUpdate();
}

void subscription::updateStatus(QMqttSubscription::SubscriptionState state)
{
    switch (state)
    {
    case QMqttSubscription::Unsubscribed:
        subState = QLatin1String("Unsubscribed");
        break;
    case QMqttSubscription::SubscriptionPending:
        subState = QLatin1String("Pending");
        break;
    case QMqttSubscription::Subscribed:
        subState = QLatin1String("Subscribed");
        break;
    case QMqttSubscription::Error:
        subState = QLatin1String("Error");
        break;
    default:
        break;
    }
    emit staToMainUpdate();
}

void subscription::updataQos(quint8 qos)
{

}
