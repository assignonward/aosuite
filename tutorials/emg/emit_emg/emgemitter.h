/*************************************************
*       Copyright © 2016, Medtronic, Inc.        *
* Confidential, Proprietary, All Rights Reserved *
*************************************************/

#ifndef EMGEMITTER_H
#define EMGEMITTER_H

#include <QObject>
#include <QStringList>
#include <QDebug>

#include "qamqpclient.h"
#include "qamqpexchange.h"
#include "qamqpqueue.h"

class EmgEmitter : public QObject
{
    Q_OBJECT

public:
    EmgEmitter(QObject *parent = 0) : QObject(parent) {}

signals:
    void exchangeDeclared();

public slots:
    void connectHost();
    void createExchange();
    void sendMessage();
    void disconnectDebug();
    void error(QAMQP::Error);
    void socketError(QAbstractSocket::SocketError);
    void setBytes( int b )  { bytes  = b; }
    void setOffset( int o ) { offset = o; }
    void setEcho( bool e )  { echo   = e; }

public:
    QAmqpClient  m_client;
  QAmqpExchange *topic_logs;
            int  bytes;
            int  offset;
           bool  echo;
};

#endif // EMGEMITTER_H
