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
#include "pageref.h"

PageRef::PageRef( const DataItemBA &di, QObject *p )
  : DataVarLength( AO_PAGE_REF, p ), sequenceNumber( -1 )
{ // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if ( typeCodeOf( di ) != AO_PAGE_REF )
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
                    { case AO_BLOCK_REF:
                        block = items;
                        break;

                      case AO_INDEX:
                        sequenceNumber = items;
                        break;

                      case AO_HASH256:
                      case AO_HASH512:
                        hash = items;

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
 * @brief PageRef::operator =
 * @param di - data item to assign
 */
void PageRef::operator = ( const DataItemBA &di )
{ PageRef temp( di );
  block          = temp.block;
  sequenceNumber = temp.sequenceNumber;
  hash           = temp.hash;
  typeCode       = temp.typeCode;
  return;
}

DataItemBA  PageRef::toDataItem( bool cf )
{ QByteArrayList dil;
  dil.append( block.toDataItem(cf) );
  if ( sequenceNumber >= 0 )
    { sequenceNumber.setTypeCode( AO_INDEX );
      dil.append( sequenceNumber.toDataItem(cf) );
    }
  if ( hash.isValid() )
    dil.append( hash.toDataItem(cf) );
  std::sort( dil.begin(), dil.end() );
  ba = dil.join();
  return DataVarLength::toDataItem(cf);
}

