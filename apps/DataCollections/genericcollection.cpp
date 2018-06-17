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
#include "genericcollection.h"
#include "stdio.h"

/**
 * @brief GenericCollection::GenericCollection - constructor
 * @param di - optional data item
 * @param p - optional parent object
 */
GenericCollection::GenericCollection( DataItemBA di, QObject *p )
                     : DataItem( typeCodeOf(di), p )
{ // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if (( typeCodeOf( di ) != GB_GENESIS_BLOCK ) &&
          ( typeCodeOf( di ) != AO_ASSETS        ) &&
          ( typeCodeOf( di ) != AO_ASSIGN_REF    ) &&
          ( typeCodeOf( di ) != AO_KEY_ASSET     ) &&
          ( typeCodeOf( di ) != CB_CHAIN_BLOCK   )) // TODO: add more as they are defined
        { typeCode = AO_UNDEFINED_DATAITEM;
          // TODO: log an error
          return;
        }
       else
        { qint32 tcs, scs;
          typeCode_t itc = bytesToCode( di         , tcs );          // It's our type
          qint32     sz  = bytesToCode( di.mid(tcs), scs );
          printf( "GenericCollection() type 0x%x, size %d\n", itc, sz );
          DataItemBA items = di.mid( tcs+scs );              // strip off typeCode and size code
          if ( items.size() != sz )
            qDebug( "size disagreement between items %d and container %d for type 0x%x", items.size(), sz, itc );
          while ( items.size() > 0 )
            { int isz = typeSize( items );
              if ( isz <= 0 )
                { // TODO: log error
                  return;
                }
               else
                { printf( "inserting type 0x%x:%s\n", typeCodeOf( items ), qPrintable( QString::fromUtf8( items.left(isz).toHex() ) ) );
                  itemMM.insert( typeCodeOf( items ), fromDataItem( items.left(isz), this ) );
                  items = items.mid( isz ); // move on to the next
                  printf( "%d bytes remaining in type 0x%x, size %d\n", items.size(), itc, sz );
                }
            }
        }
    }
}

/**
 * @brief GenericCollection::operator =
 * @param di - data item to assign
 */
void GenericCollection::operator = ( const DataItemBA &di )
{ GenericCollection temp( di );
  itemMM   = temp.itemMM;
  typeCode = temp.typeCode;
  return;
}

/**
 * @brief GenericCollection::toDataItem
 * @param cf - compact (or chain) form?  Pass along to children.
 * @return data item with the BlockRef contents
 */
#include "stdio.h"
DataItemBA  GenericCollection::toDataItem( bool cf ) const
{ printf( "GenericCollection::toDataItem\n" );
  QByteArrayList dil;
  QMapIterator DataItemMap_t it( itemMM );
  printf( "%d items\n", itemMM.size() );
  while ( it.hasNext() )
    { it.next();
      printf( "appending key 0x%x\n", it.value()->getTypeCode() );
      fflush(stdout);
      dil.append( it.value()->toDataItem(cf) );
      printf( "appended key 0x%x:%s\n", typeCodeOf( dil.last() ), qPrintable( QString::fromUtf8( dil.last().toHex() ) ) );
    }
  std::sort( dil.begin(), dil.end() );
  QByteArray dba = dil.join();
  DataItemBA di;
  di.append( codeToBytes( typeCode   ) );
  di.append( codeToBytes( dba.size() ) ); // variable length form...
  di.append( dba );
  return di;
}

DataItemBA  GenericCollection::toHashData( bool cf ) const
{ // qDebug( "GenericCollection::toHashData" );
  // Build a list of data items, and include paired hashes if the item is separable
  QList<QPair<DataItemBA,QByteArray>> dil;
  QMapIterator DataItemMap_t it( itemMM );
  while ( it.hasNext() )
    { it.next();
      DataItemBA di = it.value()->toDataItem(cf);
      QByteArray ba;
      if ( typeCodeOf( di ) & AO_SEPARABLE_TYPE )
        ba = it.value()->toHashData(cf);
      dil.append( QPair<DataItemBA,QByteArray>( di, ba ) );
    }
  std::sort( dil.begin(), dil.end() );
  // Now, make the final list, substituting hashes for data items when the item is separable
  QByteArrayList hdl;
  QPair<DataItemBA,QByteArray> pr;
  foreach ( pr, dil )
    { if ( typeCodeOf( pr.first ) & AO_SEPARABLE_TYPE )
        hdl.append( pr.second );
       else
        hdl.append( pr.first );
    }
  QByteArray dba = hdl.join();
  QByteArray hd;
  hd.append( codeToBytes( typeCode   ) );
  hd.append( codeToBytes( dba.size() ) ); // variable length form...
  hd.append( dba );

  // If this item itself is separable, then return its hash
  if ( typeCode & AO_SEPARABLE_TYPE )
    { Hash256 h;
      return h.calculate( hd ).toDataItem(cf);
    }
  // qDebug( "  hdat:%s",qPrintable( QString::fromUtf8( hd.toHex() ) ) );
  return hd;
}

