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
#include "authorization.h"

/**
 * @brief Authorization::Authorization - signatures on an assignment
 * @param di - data item to populate this object from, optional
 * @param p - parent, if any
 */
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
                            nSigs = items;
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
  assignment     = temp.assignment;
  sigs           = temp.sigs;
  nSigs          = temp.nSigs;
  typeCode       = temp.typeCode;
  return;
}

QByteArray  Authorization::toDataItem( bool cf )
{ QList<QByteArray> dil;
  dil.append( assignment.toDataItem(cf) );
  if ( sigs.size() > 0 )
    foreach( Signature s, sigs )
      dil.append( s.toDataItem(cf) );
  nSigs = sigs.size();
  dil.append( nSigs.toDataItem(cf) );
  // TODO: randomize order of dil
  ba.clear();
  foreach( QByteArray a, dil )
    ba.append( a );
  return DataVarLenLong::toDataItem(cf);
}
