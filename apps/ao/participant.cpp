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

Participant::Participant( QByteArray di, QObject *p )
  : DataVarLenLong( AO_PARTICIPANT, QByteArray(), p )
{ // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { char tc = di.at(0);
      if (( reinterpret_cast<typeCode_t &>( tc ) != AO_PARTICIPANT ) &&
          ( reinterpret_cast<typeCode_t &>( tc ) != AO_PARTICIPANT_CF ))
        { // TODO: log error
          return;
        }
       else
        { DataVarLenLong temp( di );          // It's our type
          if ( temp.checksumValidated() )
            { QByteArray items = temp.get();  // typeCode and checksum have been stripped off
              while ( items.size() > 0 )
                { int sz = typeSize( items );
                  if ( sz <= 0 )
                    { // TODO: log error
                      return;
                    }
                   else
                    { char tc = items.at(0);
                      switch ( reinterpret_cast<typeCode_t &>( tc ) ) // read valid items from the byte array, in any order
                        { case AO_ASSIGNMENT_AMT:
                            amount = items;
                            break;

                          case AO_PUB_ECDSA_KEY2:
                          case AO_PUB_ECDSA_KEY3:
                          case AO_PUB_RSA3072_KEY:
                            key = items;
                            break;

                          case AO_PUB_RSA3072_ID:
                            keyHash = items;

                          case AO_PAGE_REF:
                            page = items;

                          case AO_NOTE:
                            note = items;

                          default:
                            // TODO: log anomaly - unrecognized data type
                            break;
                        }
                      items = items.mid( sz ); // move on to the next
                    }
                }
            }
        }
    }
}

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
        if ( amount != 0 )
          dil.append( amount.toDataItem() );
        if ( key.isValid() )
          dil.append( key.toDataItem() );
        if ( amount < 0 )
          dil.append( page.toDataItem() );
        if ( note.size() > 0 )
          dil.append( note.toDataItem() );
        break;

      case AO_PARTICIPANT_CF:
        if ( amount != 0 )
          dil.append( amount.toDataItem() );
        if ( key.isValid() )
          dil.append( key.getId() );
         else if ( keyHash.isValid() )
          dil.append( keyHash.toDataItem() );
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


