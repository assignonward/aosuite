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
#include "hash.h"

/**
 * @brief Hash::Hash - default constructor
 * @param tc - type code defaults to SHA2-256
 * @param p - object parent, if any
 */
Hash::Hash( typeCode_t tc, QObject *p ) : DataItem( tc, p )
{ switch ( tc )
    { case AO_HASH256: // valid type codes for Hash
      case AO_HASH512:
        // Nothing to do here...
        break;
      default:
        // TODO: log error
        typeCode = AO_UNDEFINED_DATAITEM;
    }
}

/**
 * @brief Hash::Hash - constructor from data item
 * @param di - data item to construct from
 * @param p - object parent, if any
 */
Hash::Hash( const QByteArray &di, QObject *p ) : DataItem( AO_UNDEFINED_DATAITEM, p )
{ if ( di.size() < 4 )
    { // TODO: log error
      return;
    }
  switch ( typeCodeOf( di ) )
    { case AO_HASH256:
        typeCode = AO_HASH256;
        hash256 = Hash256( di, this );
        break;

      case AO_HASH512:
        typeCode = AO_HASH512;
        hash512 = Hash512( di, this );
        break;

      default:
        // TODO: log error
        typeCode = AO_UNDEFINED_DATAITEM;
    }
}

/**
 * @brief Hash::toDataItem
 * @return Data Item representing this hash
 */
QByteArray Hash::toDataItem()
{ switch ( typeCode )
    { case AO_HASH256:
        return hash256.toDataItem();
      case AO_HASH512:
        return hash512.toDataItem();
    }
  // TODO: log warning
  return QByteArray();
}

/**
 * @brief Hash::calculate
 * @param text - to compute the hash from
 * @return reference to this Hash object
 */
Hash &Hash::calculate( QByteArray text )
{ switch ( typeCode )
    { case AO_HASH256:
        hash256.calculate( text );
        return *this;
      case AO_HASH512:
        hash512.calculate( text );
        return *this;
    }
  // TODO: log error
  return *this;
}

/**
 * @brief Hash::verify - does the passed text's hash match this hash?
 * @param text - text to verify against this object's current hash value
 * @return true if matching, false if not
 */
bool Hash::verify( QByteArray text )
{ switch ( typeCode )
    { case AO_HASH256:
        return hash256.verify( text );

      case AO_HASH512:
        return hash512.verify( text );
    }
  // TODO: log error
  return false;
}

/**
 * @brief Hash::isValid
 * @return true if the hash has the correct length of data
 */
bool Hash::isValid()
{ switch ( typeCode )
    { case AO_HASH256:
        return hash256.isValid();
      case AO_HASH512:
        return hash512.isValid();
    }
  // TODO: log warning
  return false;
}

/**
 * @brief Hash::isVerified
 * @return true if the hash has been verified to match a text, or calculated
 */
bool Hash::isVerified()
{ switch ( typeCode )
    { case AO_HASH256:
        return hash256.isVerified();
      case AO_HASH512:
        return hash512.isVerified();
    }
  // TODO: log warning
  return false;
}
