/*************************************************
*       Copyright © 2016, Medtronic, Inc.        *
* Confidential, Proprietary, All Rights Reserved *
*************************************************/

#ifndef EMGRECEIVER_H
#define EMGRECEIVER_H

#include <QObject>
#include <QStringList>
#include <QDebug>

#include "qamqpclient.h"
#include "qamqpexchange.h"
#include "qamqpqueue.h"

class EmgReceiver : public QObject
{
    Q_OBJECT
public:
    EmgReceiver(QObject *parent = 0) : QObject(parent)
      { packetCount = byteCount = 0; lastPacketSize = -1; }

signals:
    void newPacketTotal( int );
    void newByteTotal( int );
    void newPacketSize( int );
    void packetContents( QString );

public slots:
    void start(const QStringList &bindingKeys);
    void clientConnected();
    void exchangeDeclared();
    void queueDeclared();
    void queueBound();
    void messageReceived();
    void setEcho( bool e ) { echo = e; }

private:
    QAmqpClient  m_client;
    QStringList  m_bindingKeys;
     QAmqpQueue *temporaryQueue;
           bool  echo;
         qint32  packetCount;
         qint32  byteCount;
         qint32  lastPacketSize;
};

#endif // EMGRECEIVER_H
