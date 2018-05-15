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
#include "signature.h"

/**
 * @brief Signature::Signature
 * @param tc - type code
 * @param p - object parent, if any
 */
Signature::Signature( typeCode_t tc, QObject *p )
  : DataItem( tc, p )
{ switch ( tc )
    { case AO_ECDSA_SIG: // valid type codes for Signature
      case AO_RSA3072_SIG:
        typeCode = tc;
        break;
      default:
        // TODO: log error
        typeCode = AO_UNDEFINED_DATAITEM;
    }
}

/**
 * @brief Signature::Signature - copy constructor
 * @param pk - source key
 * @param p - alternate parentage
 */
Signature::Signature( const Signature &s, QObject *p )
  : DataItem( s.typeCode, p ? p : s.parent() )
{ sigEcdsa   = s.sigEcdsa;
  sigRsa3072 = s.sigRsa3072;
}

/**
 * @brief Signature::Signature - constructor from data item
 * @param di - data item
 * @param p - object parent, if any
 */
Signature::Signature( const QByteArray &di, QObject *p )
  : DataItem( AO_UNDEFINED_DATAITEM, p )
{ if ( di.size() < 34 )
    { // TODO: log error
      return;
    }
  switch ( typeCodeOf( di ) )
    { case AO_ECDSA_SIG:
        typeCode = AO_ECDSA_SIG;
        sigEcdsa = SigEcdsa( di, this );
        break;

      case AO_RSA3072_SIG:
        typeCode   = AO_RSA3072_SIG;
        sigRsa3072 = SigRsa3072( di, this );
        break;

      default:
        // TODO: log error
        typeCode = AO_UNDEFINED_DATAITEM;
    }
}

/**
 * @brief Signature::get
 * @return the public key in ready to use form
 */
QByteArray  Signature::get() const
{ switch ( typeCode )
    { case AO_ECDSA_SIG:
        return sigEcdsa.get();

      case AO_RSA3072_SIG:
        return sigRsa3072.get();
    }
  // TODO: log error
  return QByteArray();
}

/**
 * @brief Signature::toDataItem
 * @return the key encapsulated as a data item
 */
QByteArray  Signature::toDataItem() const
{ switch ( typeCode )
    { case AO_ECDSA_SIG:
        return sigEcdsa.toDataItem();

      case AO_RSA3072_SIG:
        return sigRsa3072.toDataItem();
    }
  // TODO: log error
  return QByteArray();
}

/**
 * @brief Signature::operator =
 * @param di - data item to assign
 */
void Signature::operator = ( const QByteArray &di )
{ Signature temp( di );
  sigEcdsa   = temp.sigEcdsa;
  sigRsa3072 = temp.sigRsa3072;
  typeCode   = temp.typeCode;
}
