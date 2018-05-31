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

#include "datavarlength.h"
#include "note.h"
#include "pageref.h"
#include "pubkey.h"
#include "shares.h"

/**
 * @brief The Participant class - identifies a source or recipient of shares
 */
class Participant : public DataVarLength
{
    Q_OBJECT
public:
    explicit  Participant( QByteArray di = QByteArray(), QObject *p = NULL );
              Participant( const Participant &r, QObject *p = NULL )
                : DataVarLength( AO_PARTICIPANT, QByteArray(), p ? p : r.parent() ),
                  amount( r.amount ), key( r.key ), page( r.page ), note( r.note ), index( r.index ) {}
        void  operator = ( const QByteArray &di );
  QByteArray  toDataItem( bool cf = false );
  QByteArray  getId()     const { return key.getId(); }
      PubKey  getKey()    const { return key;         }
      Shares  getAmount() const { return amount;      }
  QByteArray  getNote()   const { return note.get();  }
        void  setId( QByteArray i )   { key.set( i ); }
        void  setAmount( Shares v )   { amount = v;  /* TODO: log error for 0 */ }
        void  setNote( QByteArray n ) { note.set( n ); }
       Index  getIndex() const { return index; }
        void  setIndex( const Index &i ) { index = i; }

private:
      Shares  amount;  // Negative for givers, positive for receivers, 0 is invalid
      PubKey  key;     // Full public key, suitable for checking signatures
        Hash  keyHash; // Hash of the public key, not always suitable to check signatures, but good enough for unique Id
     PageRef  page;    // Reference for givers
        Note  note;    // Arbitrary data to record with the transaction
       Index  index;   // When part of a participant list, this is the index number (starting with 0)
};

#endif // PARTICIPANT_H
