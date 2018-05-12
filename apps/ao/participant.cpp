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

/**
 * @brief Participant::toDataItem
 * @param tc - type of data item to return, either long form for contract negotiation,
 *   or compact form for recording in the blockchain
 * @return data item in the requested form
 */
QByteArray Participant::toDataItem( typeCode_t tc )
{ QList<QByteArray> dil;
  switch ( typeCode )
    { case AO_PARTICIPANT:
        dil.append( amount.toDataItem() );
        dil.append( key.toDataItem() );
        if ( amount < 0 )
          dil.append( page.toDataItem() );
        if ( note.size() > 0 )
          dil.append( note.toDataItem() );
        break;

      case AO_PARTICIPANT_CF:
        dil.append( amount.toDataItem() );
        dil.append( key.getId() );
        if ( note.size() > 0 )
          dil.append( note.toDataItem() );
        break;

      default:
        // TODO: log error
        return QByteArray();
    }
  // TODO: randomize order of dil
  typeCode = tc;
  ba.clear();
  foreach( QByteArray a, dil )
    ba.append( a );
  return DataVarLenLong::toDataItem();
}


