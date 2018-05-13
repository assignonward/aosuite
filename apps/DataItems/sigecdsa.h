#ifndef SIGECDSA_H
#define SIGECDSA_H

#include <QObject>

class SigEcdsa : public QObject
{
    Q_OBJECT
public:
    explicit SigEcdsa(QObject *parent = nullptr);

signals:

public slots:
};

#endif // SIGECDSA_H