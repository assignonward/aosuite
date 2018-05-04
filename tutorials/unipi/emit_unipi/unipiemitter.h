#ifndef UNIPIEMITTER_H
#define UNIPIEMITTER_H

#include <QObject>
#include <QStringList>
#include <QDebug>

#include "qamqpclient.h"
#include "qamqpexchange.h"
#include "qamqpqueue.h"

class UniPiEmitter : public QObject
{
    Q_OBJECT

public:
    UniPiEmitter(QObject *parent = 0) : QObject(parent) {}

signals:
    void exchangeDeclared();

public slots:
    void connectHost();
    void createExchange();
    void sendMessage( QByteArray msg );
    void disconnectDebug();
    void error(QAMQP::Error);
    void socketError(QAbstractSocket::SocketError);

public:
    QAmqpClient  m_client;
  QAmqpExchange *topic_logs;
};

#endif // UNIPIEMITTER_H
