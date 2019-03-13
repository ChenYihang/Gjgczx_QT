#ifndef JSONSOLVE_H
#define JSONSOLVE_H

#include <QObject>
#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QDebug>
class jsonSolve : public QObject
{
    Q_OBJECT
public:
    explicit jsonSolve(QObject *parent = nullptr);

signals:

public slots:
    QStringList jsonAnalyse(QByteArray,int);
};

#endif // JSONSOLVE_H
