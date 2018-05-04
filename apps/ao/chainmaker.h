#ifndef CHAINMAKER_H
#define CHAINMAKER_H

#include <QObject>

class ChainMaker : public QObject
{
    Q_OBJECT
public:
    explicit ChainMaker(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CHAINMAKER_H