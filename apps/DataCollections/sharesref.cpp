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
#include "sharesref.h"
#include "keypair.h"

SharesRef::SharesRef( const DataItemBA &di, QObject *p )
  : DataVarLength( AO_SHARES_REF, p ), seqNum( -1 )
{ // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if ( typeCodeOf( di ) != AO_SHARES_REF )
        { // TODO: log error
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

                      case AO_SHARE_STATE:
                        shareState = items;
                        break;

                      case AO_TIME_RECORDED:
                      case AO_RECORDING_DEADLINE:
                      case AO_UNDERWRITING_EXPIRATION:
                        lockExp = items;
                        break;

                      case AO_ASSIGN_REF:
                        assignRef = items;
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
 * @brief SharesRef::operator =
 * @param di - data item to assign
 */
void SharesRef::operator = ( const DataItemBA &di )
{ SharesRef temp( di );
  page       = temp.page;
  seqNum     = temp.seqNum;
  key        = temp.key;
  keyHash    = temp.keyHash;
  amount     = temp.amount;
  shareState = temp.shareState;
  lockExp    = temp.lockExp;
  assignRef  = temp.assignRef;
  typeCode   = temp.typeCode;
  return;
}

DataItemBA  SharesRef::toDataItem( bool cf )
{ QByteArrayList dil;
  if ( !cf )
    { if ( page.isValid() )
        dil.append( page.toDataItem(false) );
      if ( seqNum >= 0 )
        { seqNum.setTypeCode( AO_INDEX );
          dil.append( seqNum.toDataItem(false) );
        }
      if ( key.isValid() )
        dil.append( key.toDataItem(false) );
      if ( keyHash.isValid() )
        dil.append( keyHash.toDataItem(false) );
      if ( amount > 0 )
        dil.append( amount.toDataItem(false) );
      if (( shareState == KEYS_ASSIGNMENT_PENDING ) ||
          ( shareState == KEYS_SHARES_ESCROWED ))
        { if ( lockExp.past() )
            { shareState == KEYS_CONTROL_SHARES; }
           else
            { dil.append( lockExp.toDataItem(false) ); }
        }
      if ( shareState != KEYS_UNUSED )
        dil.append( shareState.toDataItem(false) );
      if ( assignRef.isValid() )
        dil.append( assignRef.toDataItem(false) );
    }
   else // Compact/chain form only needs the keyId and amount
    { if ( keyHash.isValid() )
        dil.append( keyHash.toDataItem(true) );
       else if ( key.isValid() )
        dil.append( key.getId(true) );
      if ( amount > 0 )
        dil.append( amount.toDataItem(true) );
    }
  std::sort( dil.begin(), dil.end() );
  ba = dil.join();
  return DataVarLength::toDataItem(cf);
}

