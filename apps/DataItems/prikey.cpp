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
        qDebug( "PriKey::PriKey( typeCode_t tc, QObject *p ) unrecognized type code 0x%x", tc );
        // TODO: log error
        typeCode = AO_UNDEFINED_DATAITEM;
    }
}

/**
 * @brief PriKey::PriKey - copy constructor, takes over parentage of member pointers
 * @param pk - source key
 * @param p - alternate parentage
 */
PriKey::PriKey( const PriKey &pk, QObject *p )
  : DataItem( pk.typeCode, p ? p : pk.parent() )
{ priKeyEcdsa   = pk.priKeyEcdsa;   if ( priKeyEcdsa   ) priKeyEcdsa  ->setParent( this );
  priKeyRsa3072 = pk.priKeyRsa3072; if ( priKeyRsa3072 ) priKeyRsa3072->setParent( this );
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
        priKeyEcdsa = new PrivateKeyEcdsa( di, this );
        break;

      case AO_RSA3072_PRI_KEY:
        typeCode = AO_RSA3072_PRI_KEY;
        priKeyRsa3072 = new PrivateKeyRsa3072( di, this );
        break;

      default:
        qDebug( "PriKey::PriKey( const DataItemBA &di, QObject *p ) unrecognized type code 0x%x", typeCodeOf( di ) );
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
        if ( !priKeyEcdsa )
          return QByteArray();
        return priKeyEcdsa->get();

      case AO_RSA3072_PRI_KEY:
        if ( !priKeyRsa3072 )
          return QByteArray();
        return priKeyRsa3072->get();
    }
  qDebug( "PriKey::get() invalid type code 0x%x", typeCode );
  // TODO: log error
  return QByteArray();
}

bool  PriKey::isValid() const
{ switch ( typeCode )
    { case AO_ECDSA_PRI_KEY:
        if ( !priKeyEcdsa )
          return false;
        return priKeyEcdsa->isValid();

      case AO_RSA3072_PRI_KEY:
        if ( !priKeyRsa3072 )
          return false;
        return priKeyRsa3072->isValid();
    }
  qDebug( "PriKey::isValid() unrecognized type code 0x%x", typeCode );
  // TODO: log error
  return false;
}

/**
 * @brief PriKey::toDataItem
 * @param cf - compact (or chain) form, passed on to children
 * @return the key encapsulated as a data item
 */
DataItemBA  PriKey::toDataItem( bool cf ) const
{ // qDebug( "PriKey::toDataItem() type code 0x%x", typeCode );
  switch ( typeCode )
    { case AO_ECDSA_PRI_KEY:
        if ( !priKeyEcdsa )
          return DataItemBA();
        return priKeyEcdsa->toDataItem(cf);

      case AO_RSA3072_PRI_KEY:
        if ( !priKeyRsa3072 )
          return DataItemBA();
        return priKeyRsa3072->toDataItem(cf);
    }
  qDebug( "PriKey::toDataItem() unrecognized type code 0x%x", typeCode );
  // TODO: log error
  return DataItemBA();
}

/**
 * @brief PriKey::operator =
 * @param di - data item to assign
 */
void PriKey::operator = ( const DataItemBA &di )
{ PriKey temp( di );
  priKeyEcdsa   = temp.priKeyEcdsa;   if ( priKeyEcdsa   ) priKeyEcdsa  ->setParent( this );
  priKeyRsa3072 = temp.priKeyRsa3072; if ( priKeyRsa3072 ) priKeyRsa3072->setParent( this );
  typeCode      = temp.typeCode;
}

/**
 * @brief PriKey::set - set the key
 * @param k - ready to use key
 */
void  PriKey::set( const QByteArray &k )
{ switch ( typeCode )
    { case AO_ECDSA_PRI_KEY:
        if ( !priKeyEcdsa )
          priKeyEcdsa = new PrivateKeyEcdsa( this );
        priKeyEcdsa->set( k );
        if ( priKeyRsa3072 )
          priKeyRsa3072->clear();
        break;

      case AO_RSA3072_PRI_KEY:
        if ( !priKeyRsa3072 )
          priKeyRsa3072 = new PrivateKeyRsa3072( this );
        priKeyRsa3072->set( k );
        if ( priKeyEcdsa )
          priKeyEcdsa->clear();
        break;

      default:
        qDebug( "PriKey::set( const QByteArray &k ) invalid type code 0x%x", typeCode );
        // TODO: log error
        break;
    }
}


