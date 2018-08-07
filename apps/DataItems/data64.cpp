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
#include "data64.h"

/**
 * @brief Data64::Data64
 * @param di - data item for initialization
 * @param p - object parent, if any.
 */
Data64::Data64( const DataItemBA &di, QObject *p ) : DataItem( AO_UNDEFINED_DATAITEM, p )
{ v = 0;
  union _64_as_8
    { qint64 i;
      char d[8];
    } u;
  qint32 tcSz = 0;
  typeCode = bytesToCode( di, tcSz );
  if ( di.size() < tcSz+8 )
    { // TODO: log an exception
      return;
    }
  u.d[ 0] = di.at( tcSz    ); // Surely there is a clever memory mapping to do this more efficiently, later.
  u.d[ 1] = di.at( tcSz+ 1 );
  u.d[ 2] = di.at( tcSz+ 2 );
  u.d[ 3] = di.at( tcSz+ 3 );
  u.d[ 4] = di.at( tcSz+ 4 );
  u.d[ 5] = di.at( tcSz+ 5 );
  u.d[ 6] = di.at( tcSz+ 6 );
  u.d[ 7] = di.at( tcSz+ 7 );
  v = u.i;
}

/**
 * @brief Data64::operator =
 * @param di - data item to assign
 */
void Data64::operator = ( const DataItemBA &di )
{ Data64 temp( di );
  v        = temp.v;
  typeCode = temp.typeCode;
  return;
}

/**
 * @brief Data64::toDataItem
 * @param cf - compact (or chain) form, no difference at this level - unused
 * @return byte array starting with type code, followed by 64 bit data
 */
DataItemBA Data64::toDataItem( bool cf ) const
{ QByteArray di; (void)cf;
  union _64_in_8s
    { qint64 i;
      char d[8];
    } u;
  u.i = v;
  di.append( codeToBytes( typeCode ) );
  di.append( u.d[ 0] );
  di.append( u.d[ 1] );
  di.append( u.d[ 2] );
  di.append( u.d[ 3] );
  di.append( u.d[ 4] );
  di.append( u.d[ 5] );
  di.append( u.d[ 6] );
  di.append( u.d[ 7] );
  return di;
}
