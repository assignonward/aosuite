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
#include "data8.h"

/**
 * @brief Data8::Data8
 * @param di - data item for initialization
 * @param p - object parent, if any.
 */
Data8::Data8( const QByteArray &di, QObject *p ) : DataItem( AO_UNDEFINED_DATAITEM, p )
{ csVal = false;
  v = 0;
  if ( di.size() < 3 )
    { // TODO: log an exception
      return;
    }
  union _8_as_8
    { qint8 i;
      unsigned char d[1];
    } u;
  typeCode = di.at(0);
  unsigned char chk = typeCode;
  // if (( chk & AO_SIZE_MASK ) != AO_SIZE_3BYTES )
  //   TODO: log a warning
  chk ^= u.d[ 0] = di.at( 1);
  if ( chk == di.at(2) )
    csVal = true;
  // else
  // TODO: log a warning
  v = u.i;
}

/**
 * @brief Data8::operator =
 * @param di - data item to assign
 */
void Data8::operator = ( const QByteArray &di )
{ Data8 temp( di );
  v        = temp.v;
  typeCode = temp.typeCode;
  csVal    = temp.csVal;
  return;
}

/**
 * @brief Data8::toDataItem
 * @param cf - compact (or chain) form, no difference at this level - unused
 * @return byte array starting with type code, followed by 8 bit data and 8 bit checksum.
 */
QByteArray Data8::toDataItem( bool cf ) const
{ QByteArray di; (void)cf;
  union _8_in_8s
    {         qint8 i;
      unsigned char d[1];
    } u;
  // if (( code & AO_SIZE_MASK ) != AO_SIZE_3BYTES )
  //   TODO: log a warning
  unsigned char chk = typeCode;
  u.i = v;
  di.append( typeCode );
  di.append( u.d[0] ); chk ^= u.d[0];
  di.append( chk );
  return di;
}
