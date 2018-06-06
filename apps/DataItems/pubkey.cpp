/* MIT License
 *
 * Copyright (c) 2018 Assign Onward
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "pubkey.h"
#include "hash.h"

/**
 * @brief PubKey::PubKey
 * @param tc - type code defaults to ECDSA 2
 * @param p - object parent, if any
 */
PubKey::PubKey( typeCode_t tc, QObject *p ) : DataItem( tc, p )
{ switch ( tc )
    { case AO_ECDSA_PUB_KEY2: // valid type codes for PubKey
      case AO_ECDSA_PUB_KEY3:
      case AO_RSA3072_PUB_KEY:
      case AO_ID_SEQ_NUM:
        typeCode = tc;
        break;
      default:
        // TODO: log error
        typeCode = AO_UNDEFINED_DATAITEM;
    }
}

/**
 * @brief PubKey::PubKey - copy constructor
 * @param pk - source key
 * @param p - alternate parentage
 */
PubKey::PubKey( const PubKey &pk, QObject *p )
  : DataItem( pk.typeCode, p ? p : pk.parent() )
{ publicKeyEcdsa   = pk.publicKeyEcdsa;
  publicKeyRsa3072 = pk.publicKeyRsa3072;
  publicKeyIndex   = pk.publicKeyIndex;
}

/**
 * @brief PubKey::PubKey - constructor from data item
 * @param di - data item
 * @param p - object parent, if any
 */
PubKey::PubKey( const DataItemBA &di, QObject *p ) : DataItem( AO_UNDEFINED_DATAITEM, p )
{ if ( di.size() < 34 )
    { // TODO: log error
      return;
    }
  switch ( typeCodeOf( di ) )
    { case AO_ECDSA_PUB_KEY2:
      case AO_ECDSA_PUB_KEY3:
        typeCode = di.at(0);
        publicKeyEcdsa = PublicKeyEcdsa( di, this );
        break;

      case AO_RSA3072_PUB_KEY:
        typeCode = AO_RSA3072_PUB_KEY;
        publicKeyRsa3072 = PublicKeyRsa3072( di, this );
        break;

      case AO_ID_SEQ_NUM:
        typeCode = AO_ID_SEQ_NUM;
        publicKeyIndex = Data64( di, this );

      default:
        // TODO: log error
        typeCode = AO_UNDEFINED_DATAITEM;
    }
}

/**
 * @brief PubKey::get
 * @return the public key in ready to use form
 */
QByteArray  PubKey::get() const
{ switch ( typeCode )
    { case AO_ECDSA_PUB_KEY2:
      case AO_ECDSA_PUB_KEY3:
        return publicKeyEcdsa.get();

      case AO_RSA3072_PUB_KEY:
        return publicKeyRsa3072.get();

      case AO_ID_SEQ_NUM:
        return QByteArray(); // TODO: database lookup required
    }
  // TODO: log error
  return QByteArray();
}

bool  PubKey::isValid() const
{ switch ( typeCode )
    { case AO_ECDSA_PUB_KEY2:
      case AO_ECDSA_PUB_KEY3:
        return publicKeyEcdsa.isValid();

      case AO_RSA3072_PUB_KEY:
        return publicKeyRsa3072.isValid();

      case AO_ID_SEQ_NUM:
        return false;  // TODO: database lookup required
    }
  // TODO: log error
  return false;
}

/**
 * @brief PubKey::toDataItem
 * @param cf - compact (or chain) form, passed on to children
 * @return the key encapsulated as a data item
 */
DataItemBA  PubKey::toDataItem( bool cf ) const
{ switch ( typeCode )
    { case AO_ECDSA_PUB_KEY2:
      case AO_ECDSA_PUB_KEY3:
        return publicKeyEcdsa.toDataItem(cf);

      case AO_RSA3072_PUB_KEY:
        return publicKeyRsa3072.toDataItem(cf);

      case AO_ID_SEQ_NUM:
        return publicKeyIndex.toDataItem(cf);
    }
  // TODO: log error
  return DataItemBA();
}

/**
 * @brief PubKey::operator =
 * @param di - data item to assign
 */
void PubKey::operator = ( const DataItemBA &di )
{ PubKey temp( di );
  publicKeyEcdsa   = temp.publicKeyEcdsa;
  publicKeyRsa3072 = temp.publicKeyRsa3072;
  publicKeyIndex   = temp.publicKeyIndex;
  typeCode         = temp.typeCode;
}

/**
 * @brief PubKey::getId
 * @return a data item containing either the key itself for short keys like ECDSA,
 *   or a hash of the key for longer keys.
 */
QByteArray  PubKey::getId( bool cf ) const
{ if (( typeCode == AO_ECDSA_PUB_KEY2 ) ||
      ( typeCode == AO_ECDSA_PUB_KEY3 ))
    return publicKeyEcdsa.toDataItem(cf);

  if ( typeCode == AO_RSA3072_PUB_KEY )
    { Hash256 hid( publicKeyRsa3072.get() );
      DataFixedLength did( AO_PUB_RSA3072_ID, hid.get() );
      return did.toDataItem(cf);
    }

  if ( typeCode == AO_ID_SEQ_NUM )
    return publicKeyIndex.toDataItem(cf);

  // TODO: log error
  return QByteArray();
}

/**
 * @brief PubKey::set - set the key
 * @param k - ready to use key
 */
void  PubKey::set( const QByteArray k )
{ DataItemBA diba = codeToBytes( AO_ID_SEQ_NUM );
  switch ( k.size() )
    { case 33:
        publicKeyEcdsa.set( k );
        publicKeyRsa3072.clear();
        publicKeyIndex = -1;
        break;

      case 384:
        publicKeyRsa3072.set( k );
        publicKeyEcdsa.clear();
        publicKeyIndex = -1;
        break;

      case 8:
        publicKeyRsa3072.clear();
        publicKeyEcdsa.clear();
        diba.append( k );
        publicKeyIndex = diba;
        break;

      default:
        publicKeyRsa3072.clear();
        publicKeyEcdsa.clear();
        publicKeyIndex = -1;
        // TODO: log error
        break;
    }
}
