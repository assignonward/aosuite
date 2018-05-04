/*************************************************
*       Copyright © 2016, Medtronic, Inc.        *
* Confidential, Proprietary, All Rights Reserved *
*************************************************/

#include "emgreceiver.h"

void EmgReceiver::start(const QStringList &bindingKeys)
{ qDebug( "EmgReceiver::start()" );
  m_bindingKeys = bindingKeys;
  connect(&m_client, SIGNAL(connected()), this, SLOT(clientConnected()));
  m_client.setUsername( "nim4" );
  m_client.setPassword( "nim4" );
  m_client.connectToHost( QHostAddress( "10.74.113.178" ), 5672 );
}

void EmgReceiver::clientConnected()
{ qDebug( "EmgReceiver::clientConnected()" );
  QAmqpExchange *topic_logs = m_client.createExchange("EMG");
  connect(topic_logs, SIGNAL(declared()), this, SLOT(exchangeDeclared()));
  topic_logs->declare(QAmqpExchange::Topic);
}

void EmgReceiver::exchangeDeclared()
{ qDebug( "EmgReceiver::exchangeDeclared()" );
  temporaryQueue = m_client.createQueue();
  connect(temporaryQueue, SIGNAL(declared()),        SLOT(queueDeclared())  );
  connect(temporaryQueue, SIGNAL(bound()),           SLOT(queueBound())     );
  connect(temporaryQueue, SIGNAL(messageReceived()), SLOT(messageReceived()));
  temporaryQueue->declare(QAmqpQueue::Exclusive);
}

void EmgReceiver::queueDeclared()
{ qDebug( "EmgReceiver::queueDeclared()" );
  if (!temporaryQueue)
    { qDebug( "EmgReceiver::queueDeclared() temporaryQueue NULL" );
      return;
    }

  foreach (QString bindingKey, m_bindingKeys)
    temporaryQueue->bind("EMG", bindingKey);
  qDebug() << " [*] Waiting for EMG. To exit press CTRL+C";
}

void EmgReceiver::queueBound()
{ qDebug( "EmgReceiver::queueBound()" );
  if (!temporaryQueue)
    { qDebug( "EmgReceiver::queueBound() temporaryQueue NULL" );
      return;
    }
  temporaryQueue->consume(QAmqpQueue::coNoAck);
}

void EmgReceiver::messageReceived()
{ if (!temporaryQueue)
    { qDebug( "EmgReceiver::messageReceived() temporaryQueue NULL" );
      return;
    }

  QAmqpMessage message = temporaryQueue->dequeue();
  if ( echo )
    qDebug() << " [x] " << message.routingKey()     << ":"
                        << message.payload().size() << ":"
                        << message.payload().toHex();

  qint32 s = message.payload().size();
  if ( s != lastPacketSize )
    { lastPacketSize = s;
      emit newPacketSize( s );
    }
  emit newPacketTotal( ++packetCount );
  byteCount += s;
  emit newByteTotal( byteCount );
}

