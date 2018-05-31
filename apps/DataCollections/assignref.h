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
#ifndef ASSIGNREF_H
#define ASSIGNREF_H

#include "data16.h"
#include "datavarlength.h"
#include "pageref.h"
#include "pubkey.h"
#include "shares.h"

/**
 * @brief The AssignRef class - refers to a record of shares assigned,
 *   on a page, in a block, in a chain.
 */
class AssignRef : public DataVarLength
{
    Q_OBJECT
public:
    explicit  AssignRef( const QByteArray &di = QByteArray(), QObject *p = NULL );
              AssignRef( const AssignRef &r, QObject *p = NULL )
                : DataVarLength( r.ba, r.typeCode, p ? p : r.parent() ),
                  page( r.page ), seqNum( r.seqNum ), key( r.key ), keyHash( r.keyHash ), amount( r.amount ) {}
        void  operator = ( const QByteArray &di );
  QByteArray  toDataItem( bool cf = false );
        bool  isValid() { return page.isValid() && (seqNum >= 0) && (amount > 0); }

private:
     PageRef  page;    // page these shares are recorded on
      Data16  seqNum;  // share assignment sequence number in the page
      PubKey  key;     // Id (public key) of shares
        Hash  keyHash; // Id (hashed public key) of shares
      Shares  amount;  // amount of shares assigned
};

#endif // ASSIGNREF_H
