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
#include "datafixedlength.h"

/**
 * @brief DataFixedLength::DataFixedLength - constructor.
 * @param di - typeCode, data, and checksum
 * @param p - parent object
 */
DataFixedLength::DataFixedLength( const QByteArray &di, QObject *p ) : DataItem( AO_UNDEFINED_DATAITEM, p )
{ if ( di.size() < 4 ) // Shortest fixed length serialized data type
    { typeCode = AO_DATAFIXED_UNDEFINED;
      // TODO: log an exception
      return;
    }
  typeCode = di.at(0);
  if ( di.size() < typeSize() )
    { // TODO: log an exception
      return;
    }
  unsigned char chk = typeCode;
  qint32 end = typeSize() - 1;
  for ( int i = 1 ; i < end ; i++ )
    { ba.append( di.at(i) );
      chk ^= di.at(i);
    }
  if ( chk != di.at( di.size()-1 ) )
    { // TODO: log an exception
      return;
    }
}

/**
 * @brief DataFixedLength::operator =  Assign value from a serialized bytearray
 * @param di - serialized bytearray with typeCode and checksum
 */
void DataFixedLength::operator = ( const QByteArray &di )
{ DataFixedLength temp( di );
  ba       = temp.ba;
  typeCode = temp.typeCode;
  return;
}

/**
 * @brief DataFixedLength::toDataItem
 * @return serialized byte array with type, data and checksum
 */
QByteArray DataFixedLength::toDataItem() const
{ QByteArray di;
  // if (( code & AO_FIXED_MASK ) != 0x00 )
  //   TODO: log a warning, maybe check closer for defined types, too.
  unsigned char chk = typeCode;
  di.append( typeCode );
  for ( int i = 0; i < ba.size(); i++ )
    { di.append( ba.at(i) ); chk ^= ba.at(i); }
  di.append( chk );
  return di;
}

/**
 * @brief DataFixedLength::set - if sba is the right size for typeCode, set ba = sba.
 * @param sba - byte array to set
 */
void DataFixedLength::set( QByteArray sba )
{ if ( sba.size() == (typeSize() - 2) )
    ba = sba;
  // TODO: log warning
}
