#ifndef SIGNATUREWITHTIME_H
#define SIGNATUREWITHTIME_H

#include <QObject>

class SignatureWithTime : public QObject
{
    Q_OBJECT
public:
    explicit SignatureWithTime(QObject *parent = nullptr);

signals:

public slots:
};

#endif // SIGNATUREWITHTIME_H