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
  : DataVarLenLong( tc, p )
{ typeCode = AO_SIG_WITH_TIME;
  switch ( tc )
    { case AO_ECDSA_SIG: // valid type codes for Signature
      case AO_RSA3072_SIG:
        sigType = tc;
        break;
      default:
        // TODO: log error
        sigType = AO_UNDEFINED_DATAITEM;
    }
}

/**
 * @brief Signature::Signature - copy constructor
 * @param pk - source key
 * @param p - alternate parentage
 */
Signature::Signature( const Signature &s, QObject *p )
  : DataVarLenLong( s.typeCode, p ? p : s.parent() )
{ sigTime    = s.sigTime;
  sigType    = s.sigType;
  sigEcdsa   = s.sigEcdsa;
  sigRsa3072 = s.sigRsa3072;
}

/**
 * @brief Signature::Signature - constructor from data item
 * @param di - data item
 * @param p - object parent, if any
 */
Signature::Signature( const QByteArray &di, QObject *p )
  : DataVarLenLong( AO_UNDEFINED_DATAITEM, p )
{ sigType = AO_UNDEFINED_DATAITEM;
  // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if ( typeCodeOf( di ) != AO_SIG_WITH_TIME )
        { // TODO: log an error
          return;
        }
       else
        { DataVarLenLong temp( di );          // It's our type
          if ( temp.checksumValidated() )
            { QByteArray items = temp.get();  // typeCode and checksum have been stripped off
              while ( items.size() > 0 )
                { int sz = typeSize( items );
                  if ( sz <= 0 )
                    { // TODO: log error
                      return;
                    }
                   else
                    { switch ( typeCodeOf( items ) ) // read valid items from the byte array, in any order
                        { case AO_TIME_OF_SIG:
                            sigTime = items;
                            break;

                          case AO_ECDSA_SIG:
                            sigEcdsa = items;
                            sigType  = AO_ECDSA_SIG;
                            break;

                          case AO_RSA3072_SIG:
                            sigRsa3072 = items;
                            sigType    = AO_RSA3072_SIG;
                            break;

                          default:
                            // TODO: log anomaly - unrecognized data type
                            break;
                        }
                      items = items.mid( sz ); // move on to the next
                    }
                }
            }
        }
    }
}

/**
 * @brief Signature::operator =
 * @param di - data item to assign
 */
void Signature::operator = ( const QByteArray &di )
{ Signature temp( di );
  sigTime    = temp.sigTime;
  typeCode   = temp.typeCode;
  sigEcdsa   = temp.sigEcdsa;
  sigRsa3072 = temp.sigRsa3072;
}

/**
 * @brief Signature::toDataItem
 * @return the signature and time encapsulated as a data item
 */
QByteArray  Signature::toDataItem()
{ QList<QByteArray> dil;
  dil.append( sigTime.toDataItem() );
  switch ( sigType )
      { case AO_ECDSA_SIG:
          dil.append( sigEcdsa.toDataItem() );
          break;

        case AO_RSA3072_SIG:
          dil.append( sigRsa3072.toDataItem() );
          break;
      }
  // TODO: randomize order of dil
  ba.clear();
  foreach( QByteArray a, dil )
    ba.append( a );
  return DataVarLenLong::toDataItem();
}

/**
 * @brief Signature::get
 * @return the signature in ready to use form
 */
QByteArray  Signature::getSig() const
{ switch ( sigType )
    { case AO_ECDSA_SIG:
        return sigEcdsa.get();

      case AO_RSA3072_SIG:
        return sigRsa3072.get();
    }
  // TODO: log error
  return QByteArray();
}

void  Signature::setSig( const QByteArray &s, typeCode_t tc )
{ if ( tc != AO_UNDEFINED_DATAITEM )
    sigType = tc;
  switch ( sigType )
    { case AO_ECDSA_SIG:
        sigEcdsa.set( s );
        break;

      case AO_RSA3072_SIG:
        sigRsa3072.set( s );
        break;

      // default:
      //   TODO: log error
    }
}
