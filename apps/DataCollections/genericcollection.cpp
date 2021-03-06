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
 * @brief GenericCollection::deleteItemsLater - clean out the DataItemMap
 */
void GenericCollection::deleteItemsLater()
{ QList<DataItem *>dipl = itemMM.values();
  foreach( DataItem *di, dipl )
    di->deleteLater();
}

/**
 * @brief GenericCollection::GenericCollection - constructor
 * @param di - optional data item
 * @param p - optional parent object
 */
GenericCollection::GenericCollection( const DataItemBA &di, QObject *p )
                     : DataItem( typeCodeOf(di), p )
{ // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if (( typeCodeOf( di ) != GB_GENESIS_BLOCK ) &&
          ( typeCodeOf( di ) != AO_ASSETS        ) &&
          ( typeCodeOf( di ) != AO_ASSIGN_REF    ) &&
          ( typeCodeOf( di ) != AO_KEY_ASSET     ) &&
          ( typeCodeOf( di ) != AO_KEYPAIR       ) &&
          ( typeCodeOf( di ) != AO_KEYPAIR       ) &&
          ( typeCodeOf( di ) != PG_CONTENTS      ) &&
          ( typeCodeOf( di ) != CB_CONTENTS      ) &&
          ( typeCodeOf( di ) != CB_SIGNED        ) &&
          ( typeCodeOf( di ) != CB_COMPLETE      )) // TODO: add more as they are defined
        { typeCode = AO_UNDEFINED_DATAITEM;
          qDebug( "%lld is not a GenericCollection", typeCodeOf( di ) );
          // TODO: log an error
          return;
        }
       else
        { qint32 tcs, scs;
          typeCode_t itc = bytesToCode( di         , tcs ); // It's our type
          qint32     sz  = bytesToCode( di.mid(tcs), scs );
          DataItemBA dib = di.mid( tcs+scs );               // strip off typeCode and size code
          if ( dib.size() != sz )
            { qDebug( "size disagreement between items %d and container %d for type %lld", dib.size(), sz, itc );
              setTypeCode( AO_UNDEFINED_DATAITEM );
            }
           else
            { while ( dib.size() > 0 )
                { int isz = typeSize( dib );
                  if ( isz <= 0 )
                    { qDebug( "typeSize() %d makes no sense.", isz );
                      // TODO: log error
                      return;
                    }
                   else
                    { itemMM.insert( typeCodeOf( dib ), fromDataItem( dib.left(isz), p ) );
                      dib = dib.mid(isz); // move on to the next
                    }
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
  deleteItemsLater(); // Clean out old items before assigning new ones.
  itemMM   = temp.itemMM;
  typeCode = temp.typeCode;
  return;
}

/**
 * @brief GenericCollection::toDataItem
 * @param cf - compact (or chain) form?  Pass along to children.
 * @return data item with the BlockRef contents
 */
DataItemBA  GenericCollection::toDataItem( bool cf ) const
{ // qDebug( "GenericCollection::toDataItem" );
  QByteArrayList dil;
  QMapIterator DataItemMap_t it( itemMM );
  while ( it.hasNext() )
    { it.next();
      if ( it.key() != it.value()->getTypeCode() )
        { qDebug( "ERROR: key %lld does not match typeCode %lld, skipping.", it.key(), it.value()->getTypeCode() ); }
       else
        { dil.append( it.value()->toDataItem(cf) );
        }
    }
  std::sort( dil.begin(), dil.end() );
  QByteArray dba = dil.join();
  DataItemBA di;
  di.append( codeToBytes( typeCode   ) );
  di.append( codeToBytes( dba.size() ) ); // variable length form...
  di.append( dba );
  // qDebug( "GenericCollection:%s", qPrintable( QString::fromUtf8(di.toHex())));
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
      if ( typeCodeIsSeparable( typeCodeOf( di ) ) )
        ba = it.value()->toHashData(cf);
      dil.append( QPair<DataItemBA,QByteArray>( di, ba ) );
    }
  std::sort( dil.begin(), dil.end() );
  // Now, make the final list, substituting hashes for data items when the item is separable
  QByteArrayList hdl;
  QPair<DataItemBA,QByteArray> pr;
  foreach ( pr, dil )
    { if ( typeCodeIsSeparable( typeCodeOf( pr.first ) ) )
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
  if ( typeCodeIsSeparable() )
    { Hash256 h;
      return h.calculate( hd ).toDataItem(cf);
    }
  // qDebug( "  hdat:%s",qPrintable( QString::fromUtf8( hd.toHex() ) ) );
  return hd;
}

void  GenericCollection::debugShow( qint32 level ) const
{ DataItem::debugShow( level );
  qDebug( "%sGenericCollection itemMM.size() %d", qPrintable(QString( level, QChar('.') )), itemMM.size() );
  int i = 0;
  QMapIterator DataItemMap_t it( itemMM );
  while ( it.hasNext() )
    { it.next();
      qDebug( "%sGenericCollection %d itemMM.key: %lld", qPrintable(QString( level, QChar('.') )), i, it.key() );
      qDebug( "%sGenericCollection %d itemMM.contents:", qPrintable(QString( level, QChar('.') )), i );
      i++;
      it.value()->debugShow( level+1 );
    }
}


