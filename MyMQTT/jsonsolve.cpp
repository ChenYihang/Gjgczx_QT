#include "jsonsolve.h"

jsonSolve::jsonSolve(QObject *parent) : QObject(parent)
{

}

QStringList jsonSolve::jsonAnalyse(QByteArray jsonSource,int type){

    QString str(jsonSource);
    QScriptEngine engine;
    QScriptValue source = engine.evaluate("value=" + str);
   // qDebug()<<source.property("code").toInt32();
   // QScriptValue subsc22 = source.property("result");
    // QScriptValue subsc11 = subsc22.property("version");
  //  qDebug()<<subsc11.toString();
    QStringList result;
   //qDebug()<<type;
    switch (type) {
    case 0: {
        if(source.property("code").toInt32() != 0)
            break;
        QScriptValue subsc = source.property("result");
        QScriptValue subsc1 = subsc.property("version");
        QScriptValue subsc2 = subsc.property("sysdescr");
        QScriptValue subsc3 = subsc.property("uptime");
        QScriptValue subsc4 = subsc.property("datetime");
        result<<subsc1.toString()<<subsc2.toString()<<subsc3.toString()<<subsc4.toString();
        break;
    }
    case 1: {
        if(source.property("code").toInt32() != 0)
            break;
        QScriptValue subsc = source.property("result");
        QScriptValue subsc1 = subsc.property("name");
        QScriptValue subsc2 = subsc.property("memory_total");
        QScriptValue subsc3 = subsc.property("memory_used");
        QScriptValue subsc4 = subsc.property("process_available");
        QScriptValue subsc5 = subsc.property("process_used");
        QScriptValue subsc6 = subsc.property("max_fds");
        QScriptValue subsc7 = subsc.property("node_status");
        QScriptValue subsc8 = subsc.property("otp_release");

        result<<subsc1.toString()<<subsc2.toString()<<subsc3.toString()<<subsc4.toString()<<subsc5.toString()<<subsc6.toString()<<subsc7.toString()<<subsc8.toString();
        break;
    }
    case 2:{
        if(source.property("code").toInt32() != 0)
            break;
        QScriptValue subsc = source.property("result");
        QScriptValue subsc1 = subsc.property("objects");
      //  QScriptValue subsc11 = subsc1.property(0);
      //  QScriptValue subsc111 = subsc11.property("client_id");
        QScriptValueIterator it(subsc1);
        while(it.hasNext()){
            it.next();
            if(it.flags()&&QScriptValue::SkipInEnumeration)continue;  //avoid read more one   by https://stackoverflow.com/questions/9167237/qt-qscriptvalueiterator-has-an-extra-element
            result<<it.value().property("client_id").toString();
            result<<it.value().property("username").toString();
            result<<it.value().property("ipaddress").toString();
            result<<it.value().property("port").toString();
            result<<it.value().property("clean_sess").toString();
            result<<it.value().property("proto_ver").toString();
            result<<it.value().property("keepalive").toString();
            result<<it.value().property("connected_at").toString();

        }
        break;
    }
    case 3:{
        if(source.property("code").toInt32() != 0)
            break;
        QScriptValue subsc = source.property("result");
        QScriptValue subsc1 = subsc.property("objects");
      //  QScriptValue subsc11 = subsc1.property(0);
      //  QScriptValue subsc111 = subsc11.property("client_id");
        QScriptValueIterator it(subsc1);
        while(it.hasNext()){
            it.next();
            if(it.flags()&&QScriptValue::SkipInEnumeration)continue;  //avoid read more one   by https://stackoverflow.com/questions/9167237/qt-qscriptvalueiterator-has-an-extra-element
            result<<it.value().property("client_id").toString();
            result<<it.value().property("clean_sess").toString();
            result<<it.value().property("subscriptions").toString();
            result<<it.value().property("max_inflight").toString();
            result<<it.value().property("inflight_len").toString();
            result<<it.value().property("mqueue_len").toString();
            result<<it.value().property("mqueue_dropped").toString();
            result<<it.value().property("awaiting_rel_len").toString();
            result<<it.value().property("deliver_msg").toString();
            result<<it.value().property("enqueue_msg").toString();
            result<<it.value().property("created_at").toString();

        }
        break;
    }
    case 4:{
        if(source.property("code").toInt32() != 0)
            break;
        QScriptValue subsc = source.property("result");
        QScriptValue subsc1 = subsc.property("objects");
        QScriptValueIterator it(subsc1);
        while(it.hasNext()){
            it.next();
            if(it.flags()&&QScriptValue::SkipInEnumeration)continue;  //avoid read more one   by https://stackoverflow.com/questions/9167237/qt-qscriptvalueiterator-has-an-extra-element
            result<<it.value().property("client_id").toString();
            result<<it.value().property("topic").toString();
            result<<it.value().property("qos").toString();


        }
        break;
    }
    default:
        break;


    }
  //  qDebug()<<result;
    return result;




}
