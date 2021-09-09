/* MIT License
 *
 * Copyright (c) 2021 Assign Onward
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
#include "riceyCodes.h"


/**
 * @brief intToRice -
 * @param v - unsigned integer value to convert
 * @return v converted to rice code, MSB first in the byte array
 */
QByteArray intToRice( quint64 v )
{ if ( v == 0 )
    return QByteArray( 0 );
  QByteArray ba;
  while ( v > 0 )
    { quint8 c = v & 0x7F;
      if ( ba.size() > 0 )
        c |= 0x80;
      ba.prepend( c );
      v = v >> 7;
    }
  return ba;
}

/**
 * @brief riceToInt
 * @param ba - rice code to convert, may contain additional data - which will be ignored if present.
 * @param sz - when not nullptr, returns number of bytes in the rice code
 * @param ok - when not nullptr, true if a valid rice code was found
 * @return integer equivalent of the rice code
 */
quint64 riceToInt( const QByteArray &ba, qint32 *sz, bool *ok )
{ quint64 v = 0;
  if ( sz ) *sz = 0;
  foreach ( quint8 c, ba )
    { v |= (c & 0x7F);
      if ( sz ) (*sz)++;
      if (( c & 0x80 ) == 0) // first zero msb terminates the code
        { if ( ok ) *ok = true;
          return v;
        }
      v = v << 7;
    }
  qWarning( "Unterminated rice code %s", ba.toHex().data() );
  if ( ok ) *ok = false;
  return v;
}
