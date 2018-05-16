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
#include "assignment.h"
#include <QCryptographicHash>
#include <QSettings>

/**
 * @brief Assignment::Assignment - describes the exchange of value
 * @param di - optional data item
 * @param p - object parent
 */
Assignment::Assignment(const QByteArray &di, QObject *p) : DataVarLenLong( AO_ASSIGNMENT, p )
{ // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if ( typeCodeOf( di ) != AO_ASSIGNMENT )
        { // TODO: log an error
          return;
        }
       else
        { randomizeSalt();                    // Incase it is not yet created
          DataVarLenLong temp( di );          // It's our type
          if ( temp.checksumValidated() )
            { QByteArray items = temp.get();  // typeCode and checksum have been stripped off
              while ( items.size() > 0 )
                { int sz = typeSize( items );
                  if ( sz <= 0 )
                    { // TODO: log error
                      return;
                    }
                   else
                    { switch ( typeCodeOf( items ) ) // read valid items from the byte array, in any order
                        { case AO_SALT256:
                            salt = items;
                            break;

                          case AO_PAGE_REF:
                            proposedChain = items;
                            break;

                          case AO_RECORDING_DEADLINE:
                            recordingDeadline = items;
                            break;

                          case AO_RECORDING_BID:
                            recordingBid = items;
                            break;

                          case AO_NOTE:
                            note = items;
                            break;

                          case AO_PARTICIPANT_LIST:
                          case AO_PARTICIPANT_LIST_CF:
                            participants = items;
                            break;

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

void Assignment::operator = ( const QByteArray &di )
{ Assignment temp( di );
  salt              = temp.salt;
  proposedChain     = temp.proposedChain;
  recordingDeadline = temp.recordingDeadline;
  recordingBid      = temp.recordingBid;
  note              = temp.note;
  participants      = temp.participants;
  typeCode          = temp.typeCode;
  return;
}

QByteArray  Assignment::toDataItem()
{ QList<QByteArray> dil;
  if ( salt.isValid() )
    dil.append( salt.toDataItem() );
  if ( proposedChain.isValid() )
    dil.append( proposedChain.toDataItem() );
  if ( recordingDeadline.future() )
    dil.append( recordingDeadline.toDataItem() );
  if ( recordingBid >= 0 )
    dil.append( recordingBid.toDataItem() );
  if ( note.size() > 0 )
    dil.append( note.toDataItem() );
  if ( participants.listSize() > 0 )
    dil.append( participants.toDataItem() );
  // TODO: randomize order of dil
  ba.clear();
  foreach( QByteArray a, dil )
    ba.append( a );
  return DataVarLenLong::toDataItem();
}

AOTime Assignment::proposalTime()
{ return proposedChain.publicationTime(); }

/**
 * @brief Assignment::randomizeSalt - using the last salt and
 *   the current time, come up with a new salt.
 */
void Assignment::randomizeSalt()
{ QSettings setting;
  QByteArray ls = setting.value( "lastSalt" ).toByteArray();
  rng.seed( *((__int128 *)ls.data()) ^ AOTime::now() ); // LAME, but good enough for now.
  QByteArray a;
  while ( a.length() < 32 )
    a.append( rng.rnd_uint64() & 0xFF );
  // salt = QCryptographicHash::hash( a, QCryptographicHash::Sha3_256 );
  // setting.setValue( "lastSalt", salt );
}

/**
 * @brief Assignment::valid
 * @return true if the Assignment internal checks are all valid
 */
bool Assignment::valid()
{ return validSum() & validTimeline(); }

/**
 * @brief Assignment::validTimeline
 * @return true if the timeline is in-order
 *   Will want to add some additional sanity checks
 */
bool Assignment::validTimeline()
{ return ( recordingDeadline.future() && proposalTime().past() ); }

/**
 * @brief Assignment::validSum
 * @return true if the give, take and bid sum correctly
 */
bool Assignment::validSum()
{ Shares total(0);
  foreach( Participant p, participants.list )
    total += p.getAmount();
  return ( recordingBid == total );
}

//////////////////////////////////////////////////////////////////////////

Authorization::Authorization(const QByteArray &di, QObject *p) : DataVarLenLong( AO_ASSIGNMENT, p )
{ // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if ( typeCodeOf( di ) != AO_AUTHORIZATION )
        { // TODO: log an error
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
                    { Signature sig;
                      switch ( typeCodeOf( items ) ) // read valid items from the byte array, in any order
                        { case AO_ASSIGNMENT:
                            assignment = items;
                            break;

                          case AO_SIG_WITH_TIME:
                            sig = items;
                            sigs.append( sig );
                            break;

                          case AO_LISTSIZE:
                            listSize = items;
                            break;

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

void Authorization::operator = ( const QByteArray &di )
{ Authorization temp( di );
  assignment        = temp.assignment;
  sigs              = temp.sigs;
  listSize          = temp.listSize;
  typeCode          = temp.typeCode;
  return;
}

QByteArray  Authorization::toDataItem()
{ QList<QByteArray> dil;
  dil.append( assignment.toDataItem() );
  if ( sigs.size() > 0 )
    foreach( Signature s, sigs )
      dil.append( s.toDataItem() );
  dil.append( listSize.toDataItem() );
  // TODO: randomize order of dil
  ba.clear();
  foreach( QByteArray a, dil )
    ba.append( a );
  return DataVarLenLong::toDataItem();
}
