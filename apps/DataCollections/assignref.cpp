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
#include "assignref.h"

AssignRef::AssignRef( const QByteArray &di, QObject *p )
  : DataVarLength( AO_ASSIGN_REF, p )
{ // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if ( typeCodeOf( di ) != AO_ASSIGN_REF )
        { // TODO: log error
          return;
        }
       else
        { DataVarLength temp( di );          // It's our type
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
                        { case AO_PAGE_REF:
                            page = items;
                            break;

                          case AO_INDEX:
                            seqNum = items;
                            break;

                          case AO_ECDSA_PUB_KEY2:
                          case AO_ECDSA_PUB_KEY3:
                          case AO_RSA3072_PUB_KEY:
                            key = items;
                            break;

                          case AO_PUB_RSA3072_ID:
                            keyHash = items;
                            break;

                          case AO_ASSIGNMENT_AMT:
                            amount = items;
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
 * @brief AssignRef::operator =
 * @param di - data item to assign
 */
void AssignRef::operator = ( const QByteArray &di )
{ AssignRef temp( di );
  page     = temp.page;
  seqNum   = temp.seqNum;
  key      = temp.key;
  keyHash  = temp.keyHash;
  amount   = temp.amount;
  typeCode = temp.typeCode;
  return;
}

QByteArray  AssignRef::toDataItem( bool cf )
{ QByteArrayList dil;
  if ( !cf )
    { if ( page.isValid() )
        dil.append( page.toDataItem(false) );
      if ( seqNum >= 0 )
        dil.append( seqNum.toDataItem(false) );
      if ( key.isValid() )
        dil.append( key.toDataItem(false) );
      if ( keyHash.isValid() )
        dil.append( keyHash.toDataItem(false) );
      if ( amount > 0 )
        dil.append( amount.toDataItem(false) );
    }
   else // Compact/chain form only needs the keyId
    { if ( keyHash.isValid() )
        dil.append( keyHash.toDataItem(true) );
       else if ( key.isValid() )
        dil.append( key.getId(true) );
    }
  // TODO: randomize order of dil
  ba = dil.join();
  return DataVarLength::toDataItem(cf);
}

