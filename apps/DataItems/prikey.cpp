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
#include "prikey.h"
#include "hash.h"

/**
 * @brief PriKey::PriKey
 * @param tc - type code defaults to ECDSA
 * @param p - object parent, if any
 */
PriKey::PriKey( typeCode_t tc, QObject *p ) : DataItem( tc, p )
{ switch ( tc )
    { case AO_ECDSA_PRI_KEY: // valid type codes for PriKey
      case AO_RSA3072_PRI_KEY:
        typeCode = tc;
        break;
      default:
        // TODO: log error
        typeCode = AO_UNDEFINED_DATAITEM;
    }
}

/**
 * @brief PriKey::PriKey - copy constructor
 * @param pk - source key
 * @param p - alternate parentage
 */
PriKey::PriKey( const PriKey &pk, QObject *p )
  : DataItem( pk.typeCode, p ? p : pk.parent() )
{ privateKeyEcdsa   = pk.privateKeyEcdsa;
  privateKeyRsa3072 = pk.privateKeyRsa3072;
}

/**
 * @brief PriKey::PriKey - constructor from data item
 * @param di - data item
 * @param p - object parent, if any
 */
PriKey::PriKey( const DataItemBA &di, QObject *p ) : DataItem( AO_UNDEFINED_DATAITEM, p )
{ switch ( typeCodeOf( di ) )
    { case AO_ECDSA_PRI_KEY:
        typeCode = AO_ECDSA_PRI_KEY;
        privateKeyEcdsa = PrivateKeyEcdsa( di, this );
        break;

      case AO_RSA3072_PRI_KEY:
        typeCode = AO_RSA3072_PRI_KEY;
        privateKeyRsa3072 = PrivateKeyRsa3072( di, this );
        break;

      default:
        // TODO: log error
        typeCode = AO_UNDEFINED_DATAITEM;
    }
}

/**
 * @brief PriKey::get
 * @return the public key in ready to use form
 */
QByteArray  PriKey::get() const
{ switch ( typeCode )
    { case AO_ECDSA_PRI_KEY:
        return privateKeyEcdsa.get();

      case AO_RSA3072_PRI_KEY:
        return privateKeyRsa3072.get();
    }
  // TODO: log error
  return QByteArray();
}

bool  PriKey::isValid() const
{ switch ( typeCode )
    { case AO_ECDSA_PRI_KEY:
        return privateKeyEcdsa.isValid();

      case AO_RSA3072_PRI_KEY:
        return privateKeyRsa3072.isValid();
    }
  qDebug( "PriKey::isValid() unrecognized type code %d", typeCode );
  // TODO: log error
  return false;
}

/**
 * @brief PriKey::toDataItem
 * @param cf - compact (or chain) form, passed on to children
 * @return the key encapsulated as a data item
 */
DataItemBA  PriKey::toDataItem( bool cf ) const
{ switch ( typeCode )
    { case AO_ECDSA_PRI_KEY:
        return privateKeyEcdsa.toDataItem(cf);

      case AO_RSA3072_PRI_KEY:
        return privateKeyRsa3072.toDataItem(cf);
    }
  // TODO: log error
  return QByteArray();
}

/**
 * @brief PriKey::operator =
 * @param di - data item to assign
 */
void PriKey::operator = ( const DataItemBA &di )
{ PriKey temp( di );
  privateKeyEcdsa   = temp.privateKeyEcdsa;
  privateKeyRsa3072 = temp.privateKeyRsa3072;
  typeCode          = temp.typeCode;
}

/**
 * @brief PriKey::set - set the key
 * @param k - ready to use key
 */
void  PriKey::set( const QByteArray &k )
{ switch ( typeCode )
    { case AO_ECDSA_PRI_KEY:
        privateKeyEcdsa.set( k );
        privateKeyRsa3072.clear();
        break;

      case AO_RSA3072_PRI_KEY:
        privateKeyRsa3072.set( k );
        privateKeyEcdsa.clear();
        break;

      default:
        // TODO: log error
        break;
    }
}


