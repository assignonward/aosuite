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

Data128::Data128( const QByteArray &ba, QObject *p ) : QObject(p)
{ v = 0;
  if ( ba.size() < 18 )
    { // TODO: log an exception
      return;
    }
  union _128_as_8
    { __int128 i;
      unsigned char d[16];
    } u;
  typeCode = ba.at(0);
  unsigned char chk = typeCode;
  // if (( chk & AO_SIZE_MASK ) != AO_SIZE_18BYTES )
  //   TODO: log a warning
  chk ^= u.d[ 0] = ba.at( 1);
  chk ^= u.d[ 1] = ba.at( 2);
  chk ^= u.d[ 2] = ba.at( 3);
  chk ^= u.d[ 3] = ba.at( 4);
  chk ^= u.d[ 4] = ba.at( 5);
  chk ^= u.d[ 5] = ba.at( 6);
  chk ^= u.d[ 6] = ba.at( 7);
  chk ^= u.d[ 7] = ba.at( 8);
  chk ^= u.d[ 8] = ba.at( 9);
  chk ^= u.d[ 9] = ba.at(10);
  chk ^= u.d[10] = ba.at(11);
  chk ^= u.d[11] = ba.at(12);
  chk ^= u.d[12] = ba.at(13);
  chk ^= u.d[13] = ba.at(14);
  chk ^= u.d[14] = ba.at(15);
  chk ^= u.d[15] = ba.at(16);
  // if ( chk != ba.at(17) )
  //   TODO: log a warning
  v = u.i;
}

void Data128::operator = ( const QByteArray &ba )
{ Data128 temp( ba );
  v        = temp.v;
  typeCode = temp.typeCode;
  return;
}

QByteArray Data128::toByteArray()
{ QByteArray ba;
  union _128_in_8s
    {      __int128 i;
      unsigned char d[16];
    } u;
  // if (( code & AO_SIZE_MASK ) != AO_SIZE_18BYTES )
  //   TODO: log a warning
  unsigned char chk = typeCode;
  u.i = v;
  ba.append( typeCode );
  ba.append( u.d[ 0] ); chk ^= u.d[ 0];
  ba.append( u.d[ 1] ); chk ^= u.d[ 1];
  ba.append( u.d[ 2] ); chk ^= u.d[ 2];
  ba.append( u.d[ 3] ); chk ^= u.d[ 3];
  ba.append( u.d[ 4] ); chk ^= u.d[ 4];
  ba.append( u.d[ 5] ); chk ^= u.d[ 5];
  ba.append( u.d[ 6] ); chk ^= u.d[ 6];
  ba.append( u.d[ 7] ); chk ^= u.d[ 7];
  ba.append( u.d[ 8] ); chk ^= u.d[ 8];
  ba.append( u.d[ 9] ); chk ^= u.d[ 9];
  ba.append( u.d[10] ); chk ^= u.d[10];
  ba.append( u.d[11] ); chk ^= u.d[11];
  ba.append( u.d[12] ); chk ^= u.d[12];
  ba.append( u.d[13] ); chk ^= u.d[13];
  ba.append( u.d[14] ); chk ^= u.d[14];
  ba.append( u.d[15] ); chk ^= u.d[15];
  ba.append( chk );
  return ba;
}
