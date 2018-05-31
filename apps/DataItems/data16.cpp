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
#include "data16.h"

/**
 * @brief Data16::Data16
 * @param di - data item for initialization
 * @param p - object parent, if any.
 */
Data16::Data16( const QByteArray &di, QObject *p ) : DataItem( AO_UNDEFINED_DATAITEM, p )
{ v = 0;
  union _16_as_8
    { qint16 i;
      unsigned char d[2];
    } u;
  qint32 tcSz = 0;
  typeCode = bytesToCode( di, tcSz );
  if ( di.size() < tcSz+2 )
    { // TODO: log an exception
      return;
    }
  u.d[ 0] = di.at( tcSz   );
  u.d[ 1] = di.at( tcSz+1 );
  v = u.i;
}

/**
 * @brief Data16::operator =
 * @param di - data item to assign
 */
void Data16::operator = ( const QByteArray &di )
{ Data16 temp( di );
  v        = temp.v;
  typeCode = temp.typeCode;
  csVal    = temp.csVal;
  return;
}

/**
 * @brief Data16::toDataItem
 * @param cf - compact (or chain) form, no difference at this level - unused
 * @return byte array starting with type code, followed by 16 bit data and 8 bit checksum.
 */
QByteArray Data16::toDataItem( bool cf ) const
{ QByteArray di; (void)cf;
  union _16_in_8s
    {        qint16 i;
      unsigned char d[2];
    } u;
  // if (( code & AO_SIZE_MASK ) != AO_SIZE_3BYTES )
  //   TODO: log a warning
  u.i = v;
  di.append( codeToBytes( typeCode ) );
  di.append( u.d[ 0] );
  di.append( u.d[ 1] );
  return di;
}
