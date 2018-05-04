#include "unipireceiver.h"

void UniPiReceiver::start(const QStringList &bindingKeys)
{ qDebug( "UniPiReceiver::start()" );
  m_bindingKeys = bindingKeys;
  connect(&m_client, SIGNAL(connected()), this, SLOT(clientConnected()));
  m_client.setUsername( "nim4" );
  m_client.setPassword( "nim4" );
  m_client.connectToHost( QHostAddress( "10.74.113.178" ), 5672 );
}

void UniPiReceiver::clientConnected()
{ qDebug( "UniPiReceiver::clientConnected()" );
  QAmqpExchange *topic_logs = m_client.createExchange("UniPi");
  connect(topic_logs, SIGNAL(declared()), this, SLOT(exchangeDeclared()));
  topic_logs->declare(QAmqpExchange::Topic);
}

void UniPiReceiver::exchangeDeclared()
{ qDebug( "UniPiReceiver::exchangeDeclared()" );
  temporaryQueue = m_client.createQueue();
  connect(temporaryQueue, SIGNAL(declared()),        SLOT(queueDeclared())  );
  connect(temporaryQueue, SIGNAL(bound()),           SLOT(queueBound())     );
  connect(temporaryQueue, SIGNAL(messageReceived()), SLOT(messageReceived()));
  temporaryQueue->declare(QAmqpQueue::Exclusive);
}

void UniPiReceiver::queueDeclared()
{ qDebug( "UniPiReceiver::queueDeclared()" );
  if (!temporaryQueue)
    { qDebug( "UniPiReceiver::queueDeclared() temporaryQueue NULL" );
      return;
    }

  foreach (QString bindingKey, m_bindingKeys)
    temporaryQueue->bind("UniPi", bindingKey);
  qDebug() << " [*] Waiting for UniPi. To exit press CTRL+C";
}

void UniPiReceiver::queueBound()
{ qDebug( "UniPiReceiver::queueBound()" );
  if (!temporaryQueue)
    { qDebug( "UniPiReceiver::queueBound() temporaryQueue NULL" );
      return;
    }
  temporaryQueue->consume(QAmqpQueue::coNoAck);
}

void UniPiReceiver::messageReceived()
{ if (!temporaryQueue)
    { qDebug( "UniPiReceiver::messageReceived() temporaryQueue NULL" );
      return;
    }

  QAmqpMessage message = temporaryQueue->dequeue();
  if ( echo )
    qDebug() << " [x] " << message.routingKey()     << ":"
                        << message.payload().size() << ":"
                        << message.payload().toHex();

  emit newMessageReceived( message.payload() );

  qint32 s = message.payload().size();
  if ( s != lastPacketSize )
    { lastPacketSize = s;
      emit newPacketSize( s );
    }
  emit newPacketTotal( ++packetCount );
  byteCount += s;
  emit newByteTotal( byteCount );
}

