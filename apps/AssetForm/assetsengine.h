#ifndef ASSETSENGINE_H
#define ASSETSENGINE_H

#include <QObject>
#include <QPointer>
#include "cryptoEngine.h"
#include "keyasset.h"
#include "genericcollection.h"

/**
 * @brief The AssetsEngine class - wraps an AO_ASSETS collection and provides
 *   subset retrieval functions, change notifier signals, etc.
 */
class AssetsEngine : public QObject
{
    Q_OBJECT
public:
         explicit  AssetsEngine( CryptoEngine *ice = NULL, QObject *parent = NULL );
             void  restoreConfig();
             void  saveConfig();
             void  insert( DataItem *it )
                     { if ( assets )
                         assets->insert( it );
                        else
                         qDebug( "assets NULL" );
                     }
      DataItemMap  mmap()
                     { if ( !assets )
                         return DataItemMap();
                       return assets->mmap();
                     }
GenericCollection *getUnusedKeyPair( QByteArray pkp );
GenericCollection *getNewKeyPair( typeCode_t keyType = AO_ECDSA_PRI_KEY );
             bool  isKeyPairUnused( KeyAsset *ka );
   QByteArrayList  getUnusedKeyPairIDs();

signals:
             void  newKeyAdded();

public slots:

private:
      // Simple "bag'o stuff" for early applications with small asset collections to organize
      // anticipate multiple maps into the assets and specialized collections for the heavier
      // load applications like recorders.
QPointer<GenericCollection> assets;
     QPointer<CryptoEngine> ce;
};

#endif // ASSETSENGINE_H
