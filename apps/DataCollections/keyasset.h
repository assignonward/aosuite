#ifndef KEYASSET_H
#define KEYASSET_H

#include "genericcollection.h"

/**
 * @brief The KeyAsset class - nothing special, just a GenericCollection of a specific type
 */
class KeyAsset : public GenericCollection
{
    Q_OBJECT
public:
                KeyAsset( QObject *p = nullptr )
                  : GenericCollection( AO_KEY_ASSET, p ) {}
                KeyAsset( const DataItemBA &di, QObject *p = nullptr )
                  : GenericCollection( di, p ) {}
                KeyAsset( const KeyAsset &ka, QObject *p = nullptr )
                  : GenericCollection( ka, p ? p : ka.parent() ) {}
          void  operator = ( const DataItemBA &di )
                  { GenericCollection::operator = ( di ); }
};

#endif // KEYASSET_H
