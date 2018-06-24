#ifndef ASSETSENGINE_H
#define ASSETSENGINE_H

#include <QObject>
#include <QPointer>
#include "cryptoEngine.h"
#include "genericcollection.h"

/**
 * @brief The AssetsEngine class - wraps an AO_ASSETS collection and provides
 *   subset retrieval functions, change notifier signals, etc.
 */
class AssetsEngine : public QObject
{
    Q_OBJECT
public:
         explicit  AssetsEngine( CryptoEngine *ice = NULL, QObject *parent = NULL);
             void  restoreConfig();
             void  saveConfig();
             void  insert( DataItem *it ) { assets.insert( it ); }
      DataItemMap  itemMM() { return assets.itemMM; }
GenericCollection *getUnusedKeyPair( QByteArray pkp );
GenericCollection *getNewKeyPair( typeCode_t keyType = AO_ECDSA_PRI_KEY );

signals:

public slots:

private:
      GenericCollection  assets;
  QPointer<CryptoEngine> ce;
};

#endif // ASSETSENGINE_H
