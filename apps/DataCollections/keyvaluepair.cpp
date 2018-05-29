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
#include "keyvaluepair.h"

/**
 * @brief KeyValuePair::KeyValuePair - constructor
 * @param di - optional data item
 * @param p - optional parent object
 */
KeyValuePair::KeyValuePair( QByteArray di, QObject *p )
  : DataVarLenLong( AO_KEYPAIR, p )
{ // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if ( typeCodeOf( di ) != AO_KEYVALUEPAIR )
        { // TODO: log an error
          return;
        }
       else
        { DataVarLenLong temp( di );          // It's our type
          if ( temp.checksumValidated() )
            { KeyValueKey k;
              QByteArray items = temp.get();  // typeCode and checksum have been stripped off
              while ( items.size() > 0 )
                { int sz = typeSize( items );
                  if ( sz <= 0 )
                    { // TODO: log error
                      return;
                    }
                   else
                    { typeCode_t tc = typeCodeOf( items ); // read valid items from the byte array, in any order
                      if ( tc == AO_KEYVALUEKEY )
                        { k = items;
                          key = k.value();
                        }
                       else
                        value = DataItem::fromDataItem( items );
                      items = items.mid( sz ); // move on to the next
                    }
                }
            }
        }
    }
}

/**
 * @brief KeyValuePair::operator =
 * @param di - data item to assign
 */
void KeyValuePair::operator = ( const QByteArray &di )
{ KeyValuePair temp( di );
  key      = temp.key;
  value    = temp.value;
  typeCode = temp.typeCode;
  return;
}

/**
 * @brief KeyValuePair::toDataItem
 * @param cf - compact (or chain) form?  Pass along to children.
 * @return data item with the BlockRef contents
 */
QByteArray  KeyValuePair::toDataItem( bool cf )
{ QByteArrayList dil;
  if ( key > 0 )
    { KeyValueKey k( key );
      dil.append( k.toDataItem(cf) );
    }
  if ( value.getTypeCode() != AO_UNDEFINED_DATAITEM )
    dil.append( value.toDataItem(cf) );
  // TODO: randomize order of dil
  ba = dil.join();
  return DataVarLenLong::toDataItem(cf);
}
