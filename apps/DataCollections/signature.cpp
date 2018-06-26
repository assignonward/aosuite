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
  : DataVarLength( tc, p )
{ typeCode = AO_SIG_WITH_TIME;
  index.setTypeCode( AO_INDEXV );
  index = -1;
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
  : DataVarLength( s.typeCode, p ? p : s.parent() )
{ sigTime    = s.sigTime;
  sigType    = s.sigType;
  sigEcdsa   = s.sigEcdsa;
  sigRsa3072 = s.sigRsa3072;
  index      = s.index;
}

/**
 * @brief Signature::Signature - constructor from data item
 * @param di - data item
 * @param p - object parent, if any
 */
Signature::Signature( const DataItemBA &di, QObject *p )
  : DataVarLength( AO_UNDEFINED_DATAITEM, p ), index( -1 )
{ sigType = AO_UNDEFINED_DATAITEM;
  index.setTypeCode( AO_INDEXV );
  index = -1;
  // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if ( typeCodeOf( di ) != AO_SIG_WITH_TIME )
        { // TODO: log an error
          return;
        }
       else
        { DataVarLength temp( di );        // It's our type
          DataItemBA items = temp.get();  // typeCode has been stripped off
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

                      case AO_INDEXV:
                        index = items;
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

/**
 * @brief Signature::operator =
 * @param di - data item to assign
 */
void Signature::operator = ( const DataItemBA &di )
{ Signature temp( di );
  sigTime    = temp.sigTime;
  typeCode   = temp.typeCode;
  sigEcdsa   = temp.sigEcdsa;
  sigRsa3072 = temp.sigRsa3072;
  index      = temp.index;
}

/**
 * @brief Signature::toDataItem
 * @param cf - compact (or chain) form?  Pass along to children.
 * @return the signature and time encapsulated as a data item
 */
DataItemBA  Signature::toDataItem( bool cf )
{ QByteArrayList dil;
  dil.append( sigTime.toDataItem(cf) );
  switch ( sigType )
      { case AO_ECDSA_SIG:
          dil.append( sigEcdsa.toDataItem(cf) );
          break;

        case AO_RSA3072_SIG:
          dil.append( sigRsa3072.toDataItem(cf) );
          break;
      }
  if ( index >= 0 )
    dil.append( index.toDataItem(cf) );
  std::sort( dil.begin(), dil.end() );
  ba = dil.join();
  return DataVarLength::toDataItem(cf);
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
