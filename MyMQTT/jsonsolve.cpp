#include "jsonsolve.h"

jsonSolve::jsonSolve(QObject *parent) : QObject(parent)
{

}

QStringList jsonSolve::jsonAnalyse(QByteArray jsonSource,int type)
{
    QString str(jsonSource);
    QScriptEngine engine;
    QScriptValue source = engine.evaluate("value=" + str);
    QStringList data;
    switch (type)
    {
    //节点基本信息
    case 0: {
        if(source.property("code").toInt32() != 0)
            break;
        QScriptValue subsc = source.property("data");
        QScriptValue subsc2 = subsc.property("sysdescr");//代理名称
        QScriptValue subsc1 = subsc.property("version");//版本
        QScriptValue subsc3 = subsc.property("uptime");//运行时间
        QScriptValue subsc4 = subsc.property("datetime");//系统时间
        data<<subsc1.toString()<<subsc2.toString()<<subsc3.toString()<<subsc4.toString();
        break;
    }
    //节点监控信息
    case 1: {
        if(source.property("code").toInt32() != 0)
            break;
        QScriptValue subsc = source.property("data");
        QScriptValue subsc1 = subsc.property("name");//节点名称
        QScriptValue subsc2 = subsc.property("memory_total");//内存
        QScriptValue subsc3 = subsc.property("memory_used");//内存使用
        QScriptValue subsc4 = subsc.property("process_available");//erlang进程
        QScriptValue subsc5 = subsc.property("process_used");//erlang进程
        QScriptValue subsc6 = subsc.property("max_fds");//最大句柄数
        QScriptValue subsc7 = subsc.property("node_status");//系统状态
        QScriptValue subsc8 = subsc.property("otp_release");//OTP版本
        QScriptValue subsc9 = subsc.property("load1");//CPU
        QScriptValue subsc10 = subsc.property("load5");//
        QScriptValue subsc11 = subsc.property("load15");//
        data<<subsc1.toString()<<subsc2.toString()<<subsc3.toString()<<subsc4.toString()<<subsc5.toString()<<subsc6.toString()<<subsc7.toString()<<subsc8.toString()<<subsc9.toString()<<subsc10.toString()<<subsc11.toString();
        break;
    }
     //连接列表
    case 2:{
        if(source.property("code").toInt32() != 0)
            break;
        QScriptValue subsc = source.property("data");

        //迭代器是一种检查容器内元素并遍历元素的数据类型。
       QScriptValueIterator it(subsc);//迭代器
        while(it.hasNext())//若迭代器不在序列后面
        {
            it.next();//使迭代器前进一个位置
            //https://stackoverflow.com/questions/9167237/qt-qscriptvalueiterator-has-an-extra-element
            if(it.flags() && QScriptValue::SkipInEnumeration)//若已不是最后一个
                continue;
            data<<it.value().property("node").toString();//节点
            data<<it.value().property("client_id").toString();//客户端ID
            data<<it.value().property("username").toString();//用户名
            data<<it.value().property("ipaddress").toString();//IP地址
            data<<it.value().property("port").toString();//IP地址
            data<<it.value().property("clean_start").toString();//清除会话
            data<<it.value().property("proto_ver").toString();//协议版本
            data<<it.value().property("keepalive").toString();//心跳（秒）
            data<<it.value().property("connected_at").toString();//连接时间
        }
        break;
    }
    //会话列表
    case 3:{
        if(source.property("code").toInt32() != 0)
            break;
        QScriptValue subsc = source.property("data");
        QScriptValueIterator it(subsc);
        while(it.hasNext())
        {
            it.next();
            //https://stackoverflow.com/questions/9167237/qt-qscriptvalueiterator-has-an-extra-element
            if(it.flags() && QScriptValue::SkipInEnumeration)
                continue;
            data<<it.value().property("node").toString();//节点
            data<<it.value().property("client_id").toString();//客户端ID
            data<<it.value().property("clean_start").toString();//清除会话
            data<<it.value().property("subscriptions_count").toString();//订阅数
            data<<it.value().property("expiry_interval").toString();//ExpiryInterval
            data<<it.value().property("max_inflight").toString();//最大拥塞
            data<<it.value().property("inflight_len").toString();//当前拥塞
            data<<it.value().property("mqueue_len").toString();//当前缓存信息
            data<<it.value().property("mqueue_dropped").toString();//丢弃信息
            data<<it.value().property("awaiting_rel_len").toString();//等待释放
            data<<it.value().property("deliver_msg").toString();//投递信息
            data<<it.value().property("enqueue_msg").toString();//入队信息
            data<<it.value().property("created_at").toString();//创建时间
        }
        break;
    }
    //订阅列表
    case 4:{
        if(source.property("code").toInt32() != 0)
            break;
        QScriptValue subsc = source.property("data");
        //QScriptValue subsc1 = subsc.property("objects");
        QScriptValueIterator it(subsc);
        while(it.hasNext())
        {
            it.next();
            if(it.flags() && QScriptValue::SkipInEnumeration)
                continue;
            data<<it.value().property("node").toString();
            data<<it.value().property("client_id").toString();//客户端ID
            data<<it.value().property("topic").toString();//主题
            data<<it.value().property("qos").toString();//安全质量
        }
        break;
    }
    default:
        break;
    }
  //qDebug()<<"data";
    return data;
}
