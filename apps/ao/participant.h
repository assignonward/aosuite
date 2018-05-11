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

#include "datavarlenlong.h"
#include "pageref.h"
#include "pubkey.h"
#include "shares.h"

/**
 * @brief The Participant class - describes a participant's role in an Assignment contract
 */
class Participant : public DataVarLenLong
{
    Q_OBJECT
public:
     explicit  Participant( QByteArray i, Shares a, QObject *parent = nullptr);
               Participant( const Participant &p );
   QByteArray  getId()      const { return id.get(); }
       Shares  getAmount()  const { return amount;  }
       Shares  getMinUAmt() const { return minUAmt; }
   QByteArray  getNote()    const { return note;    }
         void  setId( QByteArray i )   { id.set( i ); }
         void  setAmount( Shares v )   { amount  = v; }
         void  setMinUAmt( Shares v )  { minUAmt = v; }
         void  setNote( QByteArray n ) { note    = n; }
   QByteArray  toByteArray() { return QByteArray(); }

private:
     PageRef  page;    // Reference to a page in a block in the chain, for givers in the transaction
      PubKey  id;      // Public Key, corresponding to the private key used for signing
      Shares  amount;  // Positive for receivers, negative for givers
      Shares  minUAmt; // Positive, minimum acceptable amount for underwriting
  QByteArray  note;    // Arbitrary data to record with the transaction
};

#endif // PARTICIPANT_H
