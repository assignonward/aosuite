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
#include "keypair.h"

/**
 * @brief KeyPair::KeyPair - constructor
 * @param di - optional data item
 * @param p - optional parent object
 */
KeyPair::KeyPair( DataItemBA di, QObject *p )
  : DataVarLength( AO_KEYPAIR, p )
{ // See if there's anything interesting in the data item
  priKey = NULL;
  pubKey = NULL;
  if ( di.size() > 0 )
    { if ( typeCodeOf( di ) != AO_KEYPAIR )
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
                    { case AO_ECDSA_PUB_KEY2:
                      case AO_ECDSA_PUB_KEY3:
                      case AO_ECDSA_PUB_KEY4:
                      case AO_RSA3072_PUB_KEY:
                      // case AO_ID_SEQ_NUM: // TODO: this one needs a database lookup
                        if ( !pubKey )
                          pubKey = new PubKey( this );
                        *pubKey = items;
                        break;

                      case AO_ECDSA_PRI_KEY:
                      case AO_RSA3072_PRI_KEY:
                        if ( !priKey )
                          priKey = new PriKey( this );
                        *priKey = items;
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
 * @brief KeyPair::KeyPair - construct from pointers to the keys, make copies
 * @param priKp - private key pointer
 * @param pubKp - public key pointer
 * @param p - parent
 */
KeyPair::KeyPair( PriKey *priKp, PubKey *pubKp, QObject *p )
  : DataVarLength( AO_KEYPAIR, p ? p : (priKp->parent() ? priKp->parent() : pubKp->parent()) )
{ pubKey = pubKp; pubKey->setParent( this );
  priKey = priKp; priKey->setParent( this );
}

/**
 * @brief KeyPair::operator =
 * @param di - data item to assign
 */
void KeyPair::operator = ( const DataItemBA &di )
{ KeyPair temp( di );
  pubKey   = temp.pubKey; temp.pubKey = NULL;
  priKey   = temp.priKey; temp.priKey = NULL;
  typeCode = temp.typeCode;
  return;
}

/**
 * @brief KeyPair::toDataItem
 * @param cf - compact (or chain) form?  Pass along to children.
 * @return data item with the BlockRef contents
 */
DataItemBA  KeyPair::toDataItem( bool cf ) const
{ // qDebug( "KeyPair::toDataItem() %lld", typeCode );
  QByteArrayList dil;
  if ( pubKey )
    if ( pubKey->isValid() )
      { dil.append( pubKey->toDataItem(cf) );
        // pubKey->debugShow();
      }
  if ( priKey )
    if ( priKey->isValid() )
      { dil.append( priKey->toDataItem(cf) );
        // priKey->debugShow();
      }
  std::sort( dil.begin(), dil.end() );
  DataItemBA diba = dil.join();
  DataItemBA di; (void)cf;
  di.append( codeToBytes( typeCode  ) );
  di.append( codeToBytes( diba.size() ) );
  di.append( diba );
  // debugShow();
  // qDebug( "KeyPair:%s", qPrintable( QString::fromUtf8(di.toHex())));
  return di;
}
