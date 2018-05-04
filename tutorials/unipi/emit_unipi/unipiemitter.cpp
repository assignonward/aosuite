#include "unipiemitter.h"
#include <QApplication>

void UniPiEmitter::connectHost()
{ qDebug("UniPiEmitter::connectHost()");
  connect(&m_client, SIGNAL(connected()),         SLOT(createExchange()   ));
  connect(&m_client, SIGNAL(disconnected()),      SLOT(disconnectDebug()  ));
  connect(&m_client, SIGNAL(error(QAMQP::Error)), SLOT(error(QAMQP::Error)));
  connect(&m_client, SIGNAL(socketError(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
  m_client.setUsername( "nim4" );
  m_client.setPassword( "nim4" );
  m_client.connectToHost( QHostAddress( "10.74.113.178" ), 5672 );
}

void UniPiEmitter::createExchange()
{ qDebug("UniPiEmitter::createExchange()");
  topic_logs = m_client.createExchange("UniPi");
  connect(topic_logs, SIGNAL(declared()), SIGNAL(exchangeDeclared()));
  topic_logs->declare(QAmqpExchange::Topic);
}

void UniPiEmitter::sendMessage( QByteArray message )
{ if (!topic_logs)
    { qDebug("topic_logs NULL");
      return;
    }
  // routing key is currently fixed as: prop
  topic_logs->publish( message, "prop", "application/octet-stream" );
}

void UniPiEmitter::disconnectDebug()
{ qDebug("UniPiEmitter::disconnectDebug()");
}

void UniPiEmitter::error(QAMQP::Error error)
{ qDebug() << "error: " << error;
  // qApp->quit();
}

void UniPiEmitter::socketError(QAbstractSocket::SocketError error)
{ qDebug() << "socket error: " << error;
  // qApp->quit();
}
