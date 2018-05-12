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
#include "participantlist.h"

/**
 * @brief ParticipantList::ParticipantList
 * @param di - Data Item filled with a participant list
 * @param p - object parent, if any
 */
ParticipantList::ParticipantList( const QByteArray &di, QObject *p )
                   : DataVarLenLong( AO_PARTICIPANT_LIST, p )
{ size = 0;
  // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if (( typeCodeOf( di ) != AO_PARTICIPANT_LIST ) &&
          ( typeCodeOf( di ) != AO_PARTICIPANT_LIST_CF ))
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
                    { Participant part;
                      switch ( typeCodeOf( items ) ) // read valid items from the byte array, in any order
                        { case AO_PARTICIPANT:
                          case AO_PARTICIPANT_CF:
                            part = items;
                            list.append( part );
                            break;

                          case AO_LISTSIZE:
                            size = items;
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

/**
 * @brief ParticipantList::clear - empty the list and zero the item count
 */
void ParticipantList::clear()
{ list.clear();
  size = 0;
}

/**
 * @brief ParticipantList::append
 * @param part - participant to add to list
 * @return size of the list after appending
 */
ListSize ParticipantList::append( const Participant &part )
{ list.append( part );
  size = list.size();
  return size;
}

/**
 * @brief ParticipantList::toDataItem - serialize the list
 * @param tc - type of data item to return, either long form for contract negotiation,
 *   or compact form for recording in the blockchain
 * @return the serialized list, including a list size data item
 */
QByteArray ParticipantList::toDataItem( typeCode_t tc )
{ QList<QByteArray> dil;
  size = list.size();
  dil.append( size.toDataItem() );
  switch ( typeCode )
    { case AO_PARTICIPANT_LIST:
        foreach( Participant p, list )
          dil.append( p.toDataItem( AO_PARTICIPANT ) );
        break;

      case AO_PARTICIPANT_LIST_CF:
        foreach( Participant p, list )
          dil.append( p.toDataItem( AO_PARTICIPANT_CF ) );
        break;

      default:
        // TODO: log error
        return QByteArray();
    }
  typeCode = tc;
  ba.clear();
  foreach( QByteArray a, dil )
    ba.append( a );
  return DataVarLenLong::toDataItem();
}

