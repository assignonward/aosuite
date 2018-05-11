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

/**
 * @brief PubKey::PubKey
 * @param tc - type code defaults to ECDSA 2
 * @param p - object parent, if any
 */
PubKey::PubKey( unsigned char tc, QObject *p ) : QObject( p )
{ switch ( tc )
    { case AO_PUB_ECDSA_KEY2: // valid type codes for PubKey
      case AO_PUB_ECDSA_KEY3:
      case AO_PUB_RSA3072_KEY:
        typeCode = tc;
        break;
      default:
        // TODO: log error
        typeCode = AO_DATAFIXED_UNDEFINED;
    }
}

/**
 * @brief PubKey::PubKey - constructor from data item
 * @param di - data item
 * @param p - object parent, if any
 */
PubKey::PubKey( const QByteArray &di, QObject *p ) : QObject( p )
{ if ( di.size() < 34 )
    { // TODO: log error
      typeCode = AO_DATAFIXED_UNDEFINED;
      return;
    }
  switch ( di.at(0) )
    { case AO_PUB_ECDSA_KEY2:
      case AO_PUB_ECDSA_KEY3:
        typeCode = di.at(0);
        publicKeyECDSA = PublicKeyECDSA( di, this );
        break;

      case AO_PUB_RSA3072_KEY:
        typeCode = AO_PUB_RSA3072_KEY;
        publicKeyRsa3072 = PublicKeyRsa3072( di, this );
        break;

      default:
        // TODO: log error
        typeCode = AO_DATAFIXED_UNDEFINED;
    }
}

/**
 * @brief PubKey::get
 * @return the public key in ready to use form
 */
QByteArray  PubKey::get() const
{ switch ( typeCode )
    { case AO_PUB_ECDSA_KEY2:
      case AO_PUB_ECDSA_KEY3:
        return publicKeyECDSA.get();

      case AO_PUB_RSA3072_KEY:
        return publicKeyRsa3072.get();
    }
  // TODO: log error
  return QByteArray();
}

/**
 * @brief PubKey::set - set the key
 * @param k - ready to use key
 */
void  PubKey::set( const QByteArray k )
{ switch ( k.size() )
    { case 33:
        publicKeyECDSA.set( k );
        publicKeyRsa3072.clear();
        break;

      case 384:
        publicKeyRsa3072.set( k );
        publicKeyECDSA.clear();
        break;

      default:
        // TODO: log error
        break;
    }
}


