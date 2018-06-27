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
#include "varsizecode.h"

/**
 * @brief VarSizeCode::bytesToCode - decodes a sequence of bytes according to the variable length scheme
 * @param ca - code array
 * @param i - returns the length of the code array in bytes
 * @return code value
 */
qint64  VarSizeCode::bytesToCode( const QByteArray &ca, qint32 &i )
{ qint64 code = 0;
  qint64 m    = 1;
  i = 0;
  while (( i < ca.size() ) && ( i < 10 ))
    { code += m * ( ca.at( i ) & 0x7F );
      if ( (ca.at( i ) & 0x80) == 0 )
        { i++;
          if ( code & 1 )
            return -(code-1)/2;
          return code/2;
        }
      m = m << 7;
      i++;
    }
  // TODO: if ( i >= 10 ) log error
  if ( code & 1 )
    return -(code-1)/2;
  return code/2;
}

QByteArray  VarSizeCode::codeToBytes( const qint64 &c )
{ QByteArray ca;
  qint64 code;
  if ( c < 0 )
    code = -c * 2 + 1;
   else
    code = c * 2;
  if ( code == 0 )
    ca.append( (char)0 );
   else
    while ( code > 0 )
      { ca.append( (unsigned char)( (code & 0x7F) | ( (code>127) ? 0x80 : 0 ) ) );
        code = code >> 7;
      }
  return ca;
}
