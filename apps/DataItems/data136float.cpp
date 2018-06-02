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
#include "data136float.h"

/**
 * @brief Data136Float::Data136Float
 * @param di - data item for initialization
 * @param p - object parent, if any.
 */
Data136Float::Data136Float( const DataItemBA &di, QObject *p )
                : DataItem( AO_UNDEFINED_DATAITEM, p )
{ v = 0;
  e = 0;
  union _128_as_8
    { __int128 i;
      unsigned char d[16];
    } u;
  union _8_as_8
    {         qint8 i;
      unsigned char d[1];
    } u8;
  qint32 tcSz = 0;
  typeCode = bytesToCode( di, tcSz );
  if ( di.size() < tcSz+17 )
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
  u.d[ 8] = di.at( tcSz+ 8 );
  u.d[ 9] = di.at( tcSz+ 9 );
  u.d[10] = di.at( tcSz+10 );
  u.d[11] = di.at( tcSz+11 );
  u.d[12] = di.at( tcSz+12 );
  u.d[13] = di.at( tcSz+13 );
  u.d[14] = di.at( tcSz+14 );
  u.d[15] = di.at( tcSz+15 );
  u8.d[0] = di.at( tcSz+16 );
  v = u.i;
  e = u8.i;
}

/**
 * @brief Data136Float::operator =
 * @param di - data item to assign
 */
void Data136Float::operator = ( const DataItemBA &di )
{ Data136Float temp( di );
  v        = temp.v;
  e        = temp.e;
  typeCode = temp.typeCode;
  csVal    = temp.csVal;
  return;
}

/**
 * @brief Data136Float::toDataItem
 * @param cf - compact (or chain) form, no difference at this level - unused
 * @return byte array starting with type code, followed by 128 bit data and 8 bit checksum.
 */
DataItemBA Data136Float::toDataItem( bool cf ) const
{ QByteArray di; (void)cf;
  union _128_in_8s
    {      __int128 i;
      unsigned char d[16];
    } u;
  union _8_as_8
    {         qint8 i;
      unsigned char d[1];
    } u8;
  u.i = v;
  u8.i = e;
  di.append( codeToBytes( typeCode ) );
  di.append( u.d[ 0] );
  di.append( u.d[ 1] );
  di.append( u.d[ 2] );
  di.append( u.d[ 3] );
  di.append( u.d[ 4] );
  di.append( u.d[ 5] );
  di.append( u.d[ 6] );
  di.append( u.d[ 7] );
  di.append( u.d[ 8] );
  di.append( u.d[ 9] );
  di.append( u.d[10] );
  di.append( u.d[11] );
  di.append( u.d[12] );
  di.append( u.d[13] );
  di.append( u.d[14] );
  di.append( u.d[15] );
  di.append( u8.d[0] );
  return di;
}
