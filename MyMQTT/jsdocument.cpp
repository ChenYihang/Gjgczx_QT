#include "jsdocument.h"

jsDocument::jsDocument(QObject *parent) : QObject(parent)
{

}

void jsDocument::onMsg(const QString &msg)
{
    emit recvdMsg(msg);
}

void jsDocument::sendMsg(QWebEnginePage* page, const QString& msg)
{
    page->runJavaScript(QString("recvMessage('%1');").arg(msg));
}
