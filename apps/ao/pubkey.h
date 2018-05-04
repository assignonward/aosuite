#ifndef PUBKEY_H
#define PUBKEY_H

#include <QObject>

/**
 * @brief The PubKey class - unnecessary? QString wrapper.
 */
class PubKey : public QObject
{
    Q_OBJECT
public:
    explicit  PubKey( QObject *parent = nullptr ) : QObject( parent ) {}
              PubKey( const PubKey &p ) : QObject(p.parent()) { publicKey = p.getPublicKey(); }
  QByteArray  getPublicKey() const { return publicKey; }
        void  setPublicKey( const QByteArray k ) { publicKey = k; }

private:
  QByteArray  publicKey;
};

#endif // PUBKEY_H
