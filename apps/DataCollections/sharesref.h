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
#ifndef SHARESREF_H
#define SHARESREF_H

#include "datavarlenlong.h"
#include "index.h"
#include "pageref.h"
#include "pubkey.h"
#include "shares.h"

/**
 * @brief The SharesRef class - refers to a record of shares received,
 *   on a page, in a block, in a chain.
 */
class SharesRef : public DataVarLenLong
{
    Q_OBJECT
public:
    explicit  SharesRef( QObject *p = nullptr )
                : DataVarLenLong( AO_SHARES_REF, QByteArray(), p ) {}
              SharesRef( const SharesRef &r )
                : DataVarLenLong( r.typeCode, r.ba, r.parent() ),
                  page( r.page ), seqNum( r.seqNum ), key( r.key ), keyHash( r.keyHash ), amount( r.amount ) {}
              SharesRef( const QByteArray &di, QObject *p = nullptr );
        void  operator = ( const QByteArray &di );
  QByteArray  toDataItem();
        bool  isValid() { return page.isValid() && (seqNum >= 0) && (amount > 0); }

private:
     PageRef  page;    // page these shares are recorded on
       Index  seqNum;  // shares sequence number in the page
      PubKey  key;     // Id (public key) of shares
        Hash  keyHash; // Id (hashed public key) of shares
      Shares  amount;  // amount of shares recorded
};

#endif // SHARESREF_H
