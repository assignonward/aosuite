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
#include "assets.h"

/**
 * @brief Assets::Assets - collections of shares and contact info
 * @param di - optional data item
 * @param p - object parent
 */
Assets::Assets(const DataItemBA &di, QObject *p)
          : DataVarLength( AO_ASSETS, p )
{ // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if ( typeCodeOf( di ) != AO_ASSETS )
        { // TODO: log an error
          return;
        }
       else
        { DataVarLength temp( di );        // It's our type
          DataItemBA items = temp.get();  // typeCode has been stripped off
          while ( items.size() > 0 )
            { int sz = typeSize( items );
              if ( sz <= 0 )
                { // TODO: log error
                  return;
                }
               else
                { Organizer org;
                  Recorder rec;
                  SharesRef shr;
                  GenericCollection gc;
                  switch ( typeCodeOf( items ) ) // read valid items from the byte array, in any order
                    { case AO_ORGANIZER:
                        org = items;
                        organizers.append( org );
                        break;

                      case AO_RECORDER:
                        rec = items;
                        recorders.append( rec );
                        break;

                      case AO_SHARES_REF:
                        shr = items;
                        sharesRefs.append( shr );
                        break;

                      case AO_KEY_ASSET:
                        gc = items;
                        keyAssets.append( gc );
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

void Assets::operator = ( const DataItemBA &di )
{ Assets temp( di );
  organizers = temp.organizers;
  recorders  = temp.recorders;
  sharesRefs = temp.sharesRefs;
  keyAssets  = temp.keyAssets;
  typeCode   = temp.typeCode;
  return;
}

DataItemBA  Assets::toDataItem( bool cf )
{ QList<QByteArray> dil; (void)cf;  // unused in this context, always false
  if ( organizers.size() > 0 )
    foreach( Organizer o, organizers )
      dil.append( o.toDataItem(false) );
  if ( recorders.size() > 0 )
    foreach( Recorder r, recorders )
      dil.append( r.toDataItem(false) );
  if ( sharesRefs.size() > 0 )
    foreach( SharesRef s, sharesRefs )
      dil.append( s.toDataItem(false) );
  if ( keyAssets.size() > 0 )
    foreach( GenericCollection k, keyAssets )
      dil.append( k.toDataItem(false) );
  // TODO: randomize order of dil
  ba.clear();
  foreach( QByteArray a, dil )
    ba.append( a );
  return DataVarLength::toDataItem(false);
}
