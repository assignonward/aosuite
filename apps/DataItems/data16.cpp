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
{ csVal = false;
  v = 0;
  if ( di.size() < 4 )
    { // TODO: log an exception
      return;
    }
  union _16_as_8
    { qint16 i;
      unsigned char d[2];
    } u;
  typeCode = di.at(0);
  unsigned char chk = typeCode;
  // if (( chk & AO_SIZE_MASK ) != AO_SIZE_4BYTES )
  //   TODO: log a warning
  chk ^= u.d[ 0] = di.at( 1);
  chk ^= u.d[ 1] = di.at( 2);
  if ( chk == di.at(3) )
    csVal = true;
  // else
  // TODO: log a warning
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
 * @return byte array starting with type code, followed by 16 bit data and 8 bit checksum.
 */
QByteArray Data16::toDataItem() const
{ QByteArray di;
  union _16_in_8s
    {        qint16 i;
      unsigned char d[2];
    } u;
  // if (( code & AO_SIZE_MASK ) != AO_SIZE_4BYTES )
  //   TODO: log a warning
  unsigned char chk = typeCode;
  u.i = v;
  di.append( typeCode );
  di.append( u.d[ 0] ); chk ^= u.d[ 0];
  di.append( u.d[ 1] ); chk ^= u.d[ 1];
  di.append( chk );
  return di;
}
