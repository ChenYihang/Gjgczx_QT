﻿#ifndef COMMON_H
#define COMMON_H
#include <QUrl>

const QUrl url1("http://139.199.208.33:8080/api/v3/brokers/emqx@127.0.0.1");  //获取指定节点的基本信息
const QUrl url2("http://139.199.208.33:8080/api/v3/nodes/emqx@127.0.0.1");  //获取指定节点的监控数据
const QUrl url3("http://139.199.208.33:8080/api/v3/nodes/emqx@127.0.0.1/connections");  //获取指定节点的客户端连接列表
const QUrl url4("http://139.199.208.33:8080/api/v3/nodes/emqx@127.0.0.1/sessions");  //获取指定节点的客户端会话列表
const QUrl url5("http://139.199.208.33:8080/api/v3/nodes/emqx@127.0.0.1/subscriptions");  //获取指定节点的客户端订阅列表

enum dataType
{
    sensorData = 1,
    collectData,
    historyData,
    armFileData
};
#endif
