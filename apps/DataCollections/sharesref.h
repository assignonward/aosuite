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

#include "assignref.h"
#include "data8.h"
#include "data16.h"
#include "datavarlength.h"
#include "pageref.h"
#include "pubkey.h"
#include "shares.h"

// AO_SHARE_STATE values, stored in the shareState member
#define KEYS_UNUSED             0x00
#define KEYS_CONTROL_SHARES     0x01
#define KEYS_SHARES_ASSIGNED    0x02
#define KEYS_ASSIGNMENT_PENDING 0x03
#define KEYS_SHARES_ESCROWED    0x05  // For underwriting, similar to assignment pending but not expected to result in assignment

/**
 * @brief The SharesRef class - refers to a record of shares received,
 *   on a page, in a block, in a chain.
 */
class SharesRef : public DataVarLength
{
    Q_OBJECT
public:
    explicit  SharesRef( const DataItemBA &di = QByteArray(), QObject *p = NULL );
              SharesRef( const SharesRef &r, QObject *p = NULL )
                : DataVarLength( r.ba, r.typeCode, p ? p : r.parent() ),
                  amount( r.amount ), key( r.key ), page( r.page ), seqNum( r.seqNum ),
                  shareState( r.shareState ), lockExp( r.lockExp ), assignRef( r.assignRef ) {}
        void  operator = ( const DataItemBA &di );
  DataItemBA  toDataItem( bool cf = false );
        bool  isValid() { return page.isValid() && (seqNum >= 0) && (amount > 0); }

      Shares  amount;     // amount of shares recorded
      PubKey  key;        // Id (public key) of shares

     PageRef  page;       // page these shares are recorded on
      Data16  seqNum;     // shares sequence number in the page
       Data8  shareState; // available, or other?
      AOTime  lockExp;    // if the state is locked, when does the lock expire?
   AssignRef  assignRef;  // if these shares have been assigned away, this is the optional record of when/where
};

#endif // SHARESREF_H
