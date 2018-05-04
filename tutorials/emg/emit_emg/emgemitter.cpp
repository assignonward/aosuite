/*************************************************
*       Copyright © 2016, Medtronic, Inc.        *
* Confidential, Proprietary, All Rights Reserved *
*************************************************/

#include "emgemitter.h"
#include <QApplication>

void EmgEmitter::connectHost()
{ qDebug("EmgEmitter::connectHost()");
  connect(&m_client, SIGNAL(connected()),         SLOT(createExchange()   ));
  connect(&m_client, SIGNAL(disconnected()),      SLOT(disconnectDebug()  ));
  connect(&m_client, SIGNAL(error(QAMQP::Error)), SLOT(error(QAMQP::Error)));
  connect(&m_client, SIGNAL(socketError(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
  m_client.setUsername( "nim4" );
  m_client.setPassword( "nim4" );
  m_client.connectToHost( QHostAddress( "10.74.113.178" ), 5672 );
}

void EmgEmitter::createExchange()
{ qDebug("EmgEmitter::createExchange()");
  topic_logs = m_client.createExchange("EMG");
  connect(topic_logs, SIGNAL(declared()), SIGNAL(exchangeDeclared()));
  topic_logs->declare(QAmqpExchange::Topic);
}

void EmgEmitter::sendMessage()
{ if (!topic_logs)
    { qDebug("topic_logs NULL");
      return;
    }

  QString routingKey = "raw";
  QByteArray message;
  for ( int i = 0; i < bytes; i++ )
    message[i] = ( i + offset ) % 256;

  topic_logs->publish(message, routingKey, "application/octet-stream" );
  if ( echo )
    qDebug(" [x] Sent %s:%d:%s", routingKey.toLatin1().constData(), message.size(), message.toHex().constData());
}

void EmgEmitter::disconnectDebug()
{ qDebug("EmgEmitter::disconnectDebug()");
}

void EmgEmitter::error(QAMQP::Error error)
{ qDebug() << "error: " << error;
  // qApp->quit();
}

void EmgEmitter::socketError(QAbstractSocket::SocketError error)
{ qDebug() << "socket error: " << error;
  // qApp->quit();
}
