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


Participant::Participant(QByteArray i, Shares a, QObject *p) : DataVarLenLong( AO_PARTICIPANT, QByteArray(), p )
{ setId( i );
  setAmount( a );
  setMinUAmt( a ); // Default, can be adjusted - usually higher
  // note to be set later if desired, usually empty.
}

Participant::Participant( const Participant &p ) : DataVarLenLong( p.typeCode, p.ba, p.parent() )
{ id.setPublicKey( p.getId() );
  amount  = p.getAmount();
  minUAmt = p.getMinUAmt();
  note    = p.getNote();
}
