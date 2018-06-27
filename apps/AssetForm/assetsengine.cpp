#include "assetsengine.h"
#include "keypair.h"
#include "sharesref.h"
#include <QSettings>

AssetsEngine::AssetsEngine(CryptoEngine *ice, QObject *parent)
  : QObject(parent)
{ assets.setTypeCode( AO_ASSETS );
  if ( ice ) ce = ice; else
    { ce = new CryptoEngine( this );
      qDebug( "AssetsEngine() creating local CryptoEngine" );
    }
}

void  AssetsEngine::restoreConfig()
{ QSettings s;
  if ( s.contains( "assets" ) )
    { assets = s.value( "assets" ).toByteArray();
      emit newKeyAdded();
    }
  // assets.debugShow();
}

void  AssetsEngine::saveConfig()
{ QSettings s;
  s.setValue( "assets", assets.toDataItem() );
  // assets.debugShow();
}

/**
 * @brief getUnusedKeyPair - return a pointer to an AO_KEY_ASSET that is unused,
 *   if pkp is specified look for a matching unused public key pattern,
 *   if none is found make a new default key pair and return that
 *     key pattern will not likely match in the new key pair case.
 * @param pkp - Public Key Pattern, if empty return first available,
 *   otherwise return the first key asset with a matching pattern
 *   in the front of the public key.
 * @return pointer to an AO_KEY_ASSET in the assets map that is currently
 *   unused, one will be made if necessary, NULL if making fails.
 */
GenericCollection *AssetsEngine::getUnusedKeyPair( QByteArray pkp )
{ QMapIterator DataItemMap_t it( itemMM() );
  while ( it.hasNext() )
    { it.next();
      DataItem *di = it.value();
      if ( di->getTypeCode() == AO_KEY_ASSET )
        { GenericCollection *ka = qobject_cast<GenericCollection *>(di);
          if ( !ka ) { qDebug( "AO_KEY_ASSET did not qobject_cast to a GenericCollection" ); } else
            { if ( isKeyPairUnused( ka ) )
                { KeyPair *kp = qobject_cast<KeyPair *>(DataItem::fromDataItem( ka->value(AO_KEYPAIR) ));
                  if ( !kp ) { qDebug( "AO_KEYPAIR did not qobject_cast to a KeyPair" ); } else
                    { PubKey *pubKey = kp->getPubKey();
                      if ( !pubKey )
                        { qDebug( "pubKey NULL" ); }
                       else
                        { if ( pkp.size() < 1 )
                            return ka;
                          QByteArray akp = pubKey->get(); // Asset Key Pattern
                          if ( akp.mid( 0, pkp.size() ) == pkp )
                            return ka; // found a match
                        } // if ( !pubKey ) else
                    } // if ( !kp ) else
                } // if ( isKeyPairUnused( ka ) )
            } // if ( !ka ) else
        } // if ( di->getTypeCode() == AO_KEY_ASSET )
    } //  while ( it.hasNext() )

  // No match found in assets, make a new key pair and return it
  return getNewKeyPair();
}

/**
 * @brief AssetsEngine::getNewKeyPair - make a new key pair, store it in
 *   the assets map, and return the AO_KEY_ASSET that contains it
 * @return a newly made key pair, NULL if there is a problem.
 */
GenericCollection *AssetsEngine::getNewKeyPair( typeCode_t keyType )
{ if ( !ce ) return NULL; // can't make a pair without a CryptoEngine
  KeyPair *kp = ce->makeNewGCryPair( keyType, this );                  if ( !kp ) return NULL;
  GenericCollection *ka = new GenericCollection( AO_KEY_ASSET, this ); if ( !ka ) return NULL;
  ka->insert( kp );
  insert( ka );
  emit newKeyAdded();
  return ka;
}


/**
 * @brief AssetsEngine::isKeyPairUnused
 * @param ka - pointer to an AO_KEY_ASSET
 * @return true if the passed asset contains an unused key pair
 */
bool AssetsEngine::isKeyPairUnused( GenericCollection *ka )
{ if ( !ka )                                return false;
  if (  ka->getTypeCode() != AO_KEY_ASSET ) return false;
  if ( !ka->contains( AO_KEYPAIR ) )        return false;
  if ( !ka->contains( AO_SHARES_REF ) )     return true;
  SharesRef *sr = qobject_cast<SharesRef *>(DataItem::fromDataItem( ka->value(AO_SHARES_REF) ));
  if ( !sr ) { qDebug( "AO_SHARES_REF did not qobject_cast to a SharesRef" ); return false; }
  if ( sr->shareState.value() == KEYS_UNUSED )
    return true;
  return false;
}

/**
 * @brief getUnusedKeyPairIDs
 * @return a list of the public key values for all unused key pairs in key asset items the assets
 */
QByteArrayList  AssetsEngine::getUnusedKeyPairIDs()
{ QByteArrayList ukpids;
  QList<DataItem *>keyAssetList = itemMM().values( AO_KEY_ASSET );
  foreach( DataItem *di, keyAssetList )
    { if ( di->getTypeCode() != AO_KEY_ASSET ) { qDebug( "expected an AO_KEY_ASSET not type code %lld", di->getTypeCode() ); } else
        { GenericCollection *ka = qobject_cast<GenericCollection *>(di);
          if ( !ka ) { qDebug( "AO_KEY_ASSET did not qobject_cast to a GenericCollection" ); } else
            { if ( isKeyPairUnused( ka ) )
                { KeyPair *kp = qobject_cast<KeyPair *>(DataItem::fromDataItem( ka->value(AO_KEYPAIR) ));
                  if ( !kp ) { qDebug( "AO_KEYPAIR did not qobject_cast to a KeyPair" ); } else
                    { PubKey *pubKey = kp->getPubKey();
                      if ( !pubKey )
                        { qDebug( "pubKey NULL" ); }
                       else
                        { ukpids.append( pubKey->get() ); // Asset Key Pattern
                        } // if ( !pubKey ) else
                    } // if ( !kp ) else
                } // if ( isKeyPairUnused( ka ) )
            } // if ( !ka ) else
        } // if ( di->getTypeCode() != AO_KEY_ASSET ) else
    } // foreach( DataItem *di, keyAssetList )
  return ukpids;
}
