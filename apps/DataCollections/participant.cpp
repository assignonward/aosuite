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
// Assign Onward
//
// A participant in a share assignment, either giver (negative amount) or
//   receiver (positive amount).
//
// Must include a public key (participant ID)
// Must include non-zero amount.
// Negative amounts must:
//   - point to a previous share assignment in this chain
//     - promise no other assignment of those shares since their recording,
//       through the time this assignment expires, or permanently if this assignment is recorded
//   - exactly match the positive amount assigned to the public ID in that block-page-record
//

#include "participant.h"

Participant::Participant( DataItemBA di, QObject *p )
  : GenericCollection( typeCodeOf( di ), p )
{ // See if there's anything interesting in the data item
  setIndex( -1 );
  if ( di.size() > 0 )
    { if (( typeCodeOf( di ) != AO_PARTICIPANT ) &&
          ( typeCodeOf( di ) != AO_PARTICIPANT_CF ))
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
                { DataItemBA item = items.left( sz );
                  switch ( typeCodeOf( item ) ) // read valid items from the byte array, in any order
                    { case AO_AMT:
                        if ( !amount ) amount = new Shares( item );
                         else qDebug( "Participant doesn't expect to contain more than one AO_AMT" );
                        break;

                      case AO_ECDSA_PUB_KEY2:
                      case AO_ECDSA_PUB_KEY3:
                      case AO_RSA3072_PUB_KEY:
                      case AO_ID_SEQ_NUM:
                        if ( !key ) insert( key = new PubKey( item ) );
                         else qDebug( "Participant doesn't expect to contain more than one public key" );
                        break;

                      case AO_PAGE_REF:
                        if ( !page ) insert( page = new PageRef( item ) );
                         else qDebug( "Participant doesn't expect to contain more than one AO_PAGE_REF" );
                        break;

                      case AO_NOTE:
                        if ( !note ) insert( note = new Note( item ) );
                         else qDebug( "Participant doesn't expect to contain more than one AO_NOTE" );
                        break;


                      case AO_INDEX:
                        if ( !index ) insert( index = new DataVbc64( item ) );
                         else qDebug( "Participant doesn't expect to contain more than one AO_INDEX" );
                        break;

                      default:
                        qDebug( "Unrecognized data type %lld", typeCodeOf( item ) );
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
 * @brief Participant::operator =
 * @param di - data item to assign from
 */
void Participant::operator = ( const DataItemBA &di )
{ Participant temp( di );
  deleteItemsLater();
  amount   = temp.amount;
  key      = temp.key;
  page     = temp.page;
  note     = temp.note;
  index    = temp.index;
  DataItemMap tmap = temp.mmap();
  foreach ( DataItem *di, tmap )
    insert( di );
  typeCode = temp.typeCode;
}


/**
 * @brief Participant::toDataItem
 * @param cf - compact (or chain) form
 * @return data item in the requested form
 */
DataItemBA Participant::toDataItem( bool cf )
{ QByteArrayList dil;
  if ( cf )
    typeCode = AO_PARTICIPANT_CF;
   else if ( typeCode == AO_PARTICIPANT_CF )
    cf = true;
  switch ( typeCode )
    { case AO_PARTICIPANT:
        if ( amount )                       dil.append( amount->toDataItem(false) );
        if ( key ) if ( key->isValid() )    dil.append( key->   toDataItem(false) );
        if ( page )                         dil.append( page->  toDataItem(false) );
        if ( note ) if ( note->size() > 0 ) dil.append( note->  toDataItem(false) );
        if ( index ) if ( *index > -1 )     dil.append( index-> toDataItem(false) );
        break;

      case AO_PARTICIPANT_CF:
        if ( amount ) if ( amount != 0 )    dil.append( amount->toDataItem(true) );
        if ( key ) if ( key->isValid() )    dil.append( key->   toDataItem(true) );
        if ( note ) if ( note->size() > 0 ) dil.append( note->  toDataItem(true) );
        if ( index ) if ( *index > -1 )     dil.append( index-> toDataItem(true) );
        break;

      default:
        // TODO: log error
        return QByteArray();
    }
  std::sort( dil.begin(), dil.end() );
  QByteArray ba = dil.join();
  DataItemBA db;
  db.append( codeToBytes( typeCode  ) );
  db.append( codeToBytes( ba.size() ) );
  db.append( ba );
  return db;
}


/**
 * @brief Participant::setIndex - if it does not exist, create the index data item
 *   and insert it in the collection.  Either way, set the index data item value to v.
 * @param v - value to set index to
 */
void Participant::setIndex( qint64 v )
{ if ( !index )
    if ( contains( AO_INDEX ) )
      { qDebug( "weird, we've got an AO_INDEX in the collection, but not in the index pointer" );
        index = qobject_cast<DataVbc64 *>( value( AO_INDEX ) );
        if ( !index )
          qDebug( "weirder, AO_INDEX object would not cast to DataVbc64*" );
      }
  if ( index )
    { index->set( v );
      return;
    }
  index = new DataVbc64( v, AO_INDEX, this );
  if ( !index )
    qDebug( "problem creating index" );
   else
    insert( index );
}

void Participant::setKey( PubKey *k )
{ if ( key )
    { key->deleteLater();
      qDebug( "odd, overwriting existing key" );
    }
  insert( key = k );
}
