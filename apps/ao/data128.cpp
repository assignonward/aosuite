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
#include "data128.h"

/**
 * @brief Data128::Data128
 * @param di - data item for initialization
 * @param p - object parent, if any.
 */
Data128::Data128( const QByteArray &di, QObject *p ) : QObject(p)
{ v = 0;
  if ( di.size() < 18 )
    { // TODO: log an exception
      return;
    }
  union _128_as_8
    { __int128 i;
      unsigned char d[16];
    } u;
  typeCode = di.at(0);
  unsigned char chk = typeCode;
  // if (( chk & AO_SIZE_MASK ) != AO_SIZE_18BYTES )
  //   TODO: log a warning
  chk ^= u.d[ 0] = di.at( 1);
  chk ^= u.d[ 1] = di.at( 2);
  chk ^= u.d[ 2] = di.at( 3);
  chk ^= u.d[ 3] = di.at( 4);
  chk ^= u.d[ 4] = di.at( 5);
  chk ^= u.d[ 5] = di.at( 6);
  chk ^= u.d[ 6] = di.at( 7);
  chk ^= u.d[ 7] = di.at( 8);
  chk ^= u.d[ 8] = di.at( 9);
  chk ^= u.d[ 9] = di.at(10);
  chk ^= u.d[10] = di.at(11);
  chk ^= u.d[11] = di.at(12);
  chk ^= u.d[12] = di.at(13);
  chk ^= u.d[13] = di.at(14);
  chk ^= u.d[14] = di.at(15);
  chk ^= u.d[15] = di.at(16);
  // if ( chk != ba.at(17) )
  //   TODO: log a warning
  v = u.i;
}

/**
 * @brief Data128::operator =
 * @param di - data item to assign
 */
void Data128::operator = ( const QByteArray &di )
{ Data128 temp( di );
  v        = temp.v;
  typeCode = temp.typeCode;
  return;
}

/**
 * @brief Data128::toDataItem
 * @return byte array starting with type code, followed by 128 bit data and 8 bit checksum.
 */
QByteArray Data128::toDataItem()
{ QByteArray di;
  union _128_in_8s
    {      __int128 i;
      unsigned char d[16];
    } u;
  // if (( code & AO_SIZE_MASK ) != AO_SIZE_18BYTES )
  //   TODO: log a warning
  unsigned char chk = typeCode;
  u.i = v;
  di.append( typeCode );
  di.append( u.d[ 0] ); chk ^= u.d[ 0];
  di.append( u.d[ 1] ); chk ^= u.d[ 1];
  di.append( u.d[ 2] ); chk ^= u.d[ 2];
  di.append( u.d[ 3] ); chk ^= u.d[ 3];
  di.append( u.d[ 4] ); chk ^= u.d[ 4];
  di.append( u.d[ 5] ); chk ^= u.d[ 5];
  di.append( u.d[ 6] ); chk ^= u.d[ 6];
  di.append( u.d[ 7] ); chk ^= u.d[ 7];
  di.append( u.d[ 8] ); chk ^= u.d[ 8];
  di.append( u.d[ 9] ); chk ^= u.d[ 9];
  di.append( u.d[10] ); chk ^= u.d[10];
  di.append( u.d[11] ); chk ^= u.d[11];
  di.append( u.d[12] ); chk ^= u.d[12];
  di.append( u.d[13] ); chk ^= u.d[13];
  di.append( u.d[14] ); chk ^= u.d[14];
  di.append( u.d[15] ); chk ^= u.d[15];
  di.append( chk );
  return di;
}
