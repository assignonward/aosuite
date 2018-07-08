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
#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include "datavbc64.h"
#include "genericcollection.h"
#include "note.h"
#include "pageref.h"
#include "pubkey.h"
#include "shares.h"

/**
 * @brief The Participant class - identifies a source or recipient of shares
 */
class Participant : public GenericCollection
{
    Q_OBJECT
public:
              Participant( QObject *p = NULL )
                : GenericCollection( AO_PARTICIPANT, p ) {}
              Participant( DataItemBA di, QObject *p = NULL );
              Participant( const Participant &r, QObject *p = NULL )
                : GenericCollection( AO_PARTICIPANT, p ? p : r.parent() ),
                  amount( r.amount ), key( r.key ), page( r.page ), note( r.note ), index( r.index ) {}
        void  operator = ( const DataItemBA &di );
  DataItemBA  toDataItem( bool cf );
  QByteArray  getId()     const { return key ? key->getId() : QByteArray(); }
      PubKey *getKey()    const { return key; }
    __int128  getAmount() const { return amount ? amount->value() : 0; }
  QByteArray  getNote()   const { return note ? note->get() : QByteArray();  }
        void  setId( QByteArray i )   { if ( key ) key->set( i ); }
        void  setKey( PubKey *k );
        void  setAmount( __int128 v ) { if ( amount ) *amount = v; }
        void  setNote( QByteArray n ) { note->set( n ); }
     quint64  getIndex() const { return index ? index->value() : -1; }
        void  setIndex( qint64 v );

private:
   QPointer<Shares> amount;  // Negative for givers, positive for receivers, 0 is invalid
   QPointer<PubKey> key;     // Full public key, suitable for checking signatures
  QPointer<PageRef> page;    // Reference for givers
     QPointer<Note> note;    // Arbitrary data to record with the transaction
QPointer<DataVbc64> index;   // When part of a participant list, this is the index number (starting with 0)
};

#endif // PARTICIPANT_H
