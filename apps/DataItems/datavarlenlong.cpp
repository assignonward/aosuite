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
#include "datavarlenlong.h"

/**
 * @brief DataVarLenLong::DataVarLenLong - constructor from serialized form
 * @param di - typecode, 3 bytes size, data, 4 bytes checksum
 * @param p - parent object
 */
DataVarLenLong::DataVarLenLong( const QByteArray &di, QObject *p ) : DataItem( AO_UNDEFINED_DATAITEM, p )
{ csVal = false;
  if ( di.size() < 8 ) // Shortest valid varlenlong serialized data (1 type + 3 length + 0 data + 4 checksum)
    { // TODO: log an exception
      return;
    }
  typeCode = di.at(0);
  qint32 size = (((qint32)di.at(1)) * 256 + ((qint32)di.at(2))) * 256 + ((qint32)di.at(3));
  if ( di.size() < (size+8) )
    { // TODO: log an exception
      return;
    }
  QByteArray chk;
  chk[0] = di.at(0);
  chk[1] = di.at(1);
  chk[2] = di.at(2);
  chk[3] = di.at(3);
  int i;
  int j = 0;
  for ( i = 4; i < size+4 ; i++ )
    { ba.append( di.at(i) );
      chk[j] = chk.at(j) ^ di.at(i);
      if ( ++j > 3 )
        j = 0;
    }
  for ( j = 0; j < 4 ; j++ )
    if ( chk.at(j) != di.at(i++) )
      { // TODO: log an exception
        return;
      }
  csVal = true;
}

/**
 * @brief DataVarLenLong::operator =  Assign values from a serialized bytearray
 * @param di - serialized bytearray with typeCode, size, data and checksum
 */
void DataVarLenLong::operator = ( const QByteArray &di )
{ DataVarLenLong temp( di );
  ba       = temp.ba;
  typeCode = temp.typeCode;
  csVal    = temp.csVal;
  return;
}

/**
 * @brief DataVarLenLong::toDataItem
 * @return serialized bytearray with typeCode, size, data and checksum
 */
QByteArray DataVarLenLong::toDataItem() const
{ QByteArray di;
  di.append( typeCode );
  qint32 size = ba.size();
  di.append(( size & 0xFF0000 ) >> 16 );
  di.append(( size & 0xFF00   ) >>  8 );
  di.append(( size & 0xFF     )       );

  QByteArray chk;
  chk[0] = di.at(0);
  chk[1] = di.at(1);
  chk[2] = di.at(2);
  chk[3] = di.at(3);
  int j = 0;
  for ( int i = 0; i < ba.size(); i++ )
    { di.append( ba.at(i) );
      chk[j] = chk.at(j) ^ ba.at(i);
      if ( ++j > 3 )
        j = 0;
    }
  di.append( chk );
  return di;
}

