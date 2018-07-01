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
#ifndef GENERICCOLLECTION_H
#define GENERICCOLLECTION_H

#include "dataitem.h"
#include "hash.h"
#include <QMultiMap>

#define DataItemMap_t <typeCode_t,DataItem *>
#define DataItemMap QMultiMap DataItemMap_t

/**
 * @brief The GenericCollection class - the anchorpoint of a blockchain
 */
class GenericCollection : public DataItem
{
    Q_OBJECT
public:
                GenericCollection( QObject *p = NULL )
                  : DataItem( AO_UNDEFINED_DATAITEM, p ) {}
                GenericCollection( typeCode_t tc, QObject *p = NULL )
                  : DataItem( tc, p ) {} // Empty collection, of a specified type
                GenericCollection( const GenericCollection &r, QObject *p )
                  : DataItem( r.typeCode, p ? p : r.parent() ),
                    itemMM( r.itemMM ) {} // Copy constructor, with optional parent change
                GenericCollection( const DataItemBA &di, QObject *p = NULL );
          void  deleteItemsLater();
   DataItemMap  mmap() { return itemMM; }
           int  size() { return itemMM.size(); }
          bool  contains( const typeCode_t &tc ) { return itemMM.contains( tc ); }
      DataItem *value(    const typeCode_t &tc ) { return itemMM.   value( tc ); }
QList<DataItem *>values(  const typeCode_t &tc ) { return itemMM.  values( tc ); }
QList<DataItem *>values()                        { return itemMM.  values();     }
          void  insert( DataItem *di )
                  { if ( di )
                      { itemMM.insert( di->getTypeCode(), di );
                        di->setParent(this);
                  }   }
          void  insert( QList<DataItem *>dipl )
                  { foreach( DataItem *di, dipl )
                      { if ( di )
                          { itemMM.insert( di->getTypeCode(), di );
                            di->setParent(this);
                  }   }   }
          void  operator = ( const DataItemBA &di );
    DataItemBA  toDataItem( bool cf = false ) const;
    DataItemBA  toHashData( bool cf = false ) const;
          void  debugShow( qint32 level = 0 ) const;

private:
   DataItemMap  itemMM;  // Collection of pointers to data items of various types
};

#endif // GENERICCOLLECTION_H
