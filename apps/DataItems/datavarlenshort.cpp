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
#include "datavarlenshort.h"

/**
 * @brief DataVarLenShort::DataVarLenShort - constructor from serialized form
 * @param di - typecode, 1 byte size, data, 2 bytes checksum
 * @param p - parent object
 */
DataVarLenShort::DataVarLenShort( const QByteArray &di, QObject *p ) : DataItem( AO_UNDEFINED_DATAITEM, p )
{ csVal = false;
  if ( di.size() < 4 ) // Shortest valid varlenshort serialized data (1 type + 1 length + 0 data + 2 checksum)
    { // TODO: log an exception
      return;
    }
  typeCode = di.at(0);
  qint32 size = (qint32)di.at(1);
  if ( di.size() < (size+8) )
    { // TODO: log an exception
      return;
    }
  QByteArray chk;
  chk[0] = di.at(0);
  chk[1] = di.at(1);
  int i;
  int j = 0;
  for ( i = 2; i < size+2 ; i++ )
    { ba.append( di.at(i) );
      chk[j] = chk.at(j) ^ di.at(i);
      if ( ++j > 1 )
        j = 0;
    }
  for ( j = 0; j < 2 ; j++ )
    if ( chk.at(j) != di.at(i++) )
      { // TODO: log an exception
        return;
      }
  csVal = true;
}

/**
 * @brief DataVarLenShort::operator =  Assign values from a serialized bytearray
 * @param di - serialized bytearray with typeCode, size, data and checksum
 */
void DataVarLenShort::operator = ( const QByteArray &di )
{ DataVarLenShort temp( di );
  ba       = temp.ba;
  typeCode = temp.typeCode;
  csVal    = temp.csVal;
  return;
}

/**
 * @brief DataVarLenShort::toDataItem
 * @param cf - compact (or chain) form, no difference at this level - unused
 * @return serialized bytearray with typeCode, size, data and checksum
 */
QByteArray DataVarLenShort::toDataItem( bool cf ) const
{ QByteArray di; (void)cf;
  di.append( typeCode );
  qint32 size = ba.size();
  if ( size > 0xFF )
    { // TODO: log error
      size = 0xFF;
    }
  di.append( (unsigned char)size );

  QByteArray chk;
  chk[0] = di.at(0);
  chk[1] = di.at(1);
  int j = 0;
  for ( int i = 0; i < ba.size(); i++ )
    { di.append( ba.at(i) );
      chk[j] = chk.at(j) ^ ba.at(i);
      if ( ++j > 1 )
        j = 0;
    }
  di.append( chk );
  return di;
}

