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
 * @brief DataFixedLength::DataFixedLength
 * @param dba - typeCode, data, and checksum
 * @param p - parent object
 */
DataFixedLength::DataFixedLength( const QByteArray &dba, QObject *p ) : QObject( p )
{ if ( dba.size() < 4 ) // Shortest fixed length serialized data type
    { // TODO: log an exception
      return;
    }
  typeCode = dba.at(0);
  if (((( typeCode & AO_SIZE_MASK ) == AO_SIZE_34BYTES ) && ( ba.size() == 34 )) ||
      ((( typeCode & AO_SIZE_MASK ) == AO_SIZE_38BYTES ) && ( ba.size() == 38 )) )
    { unsigned char chk = typeCode;
      for ( int i = 1 ; i < dba.size()-1 ; i++ )
        { ba.append( dba.at(i) );
          chk ^= dba.at(i);
        }
      if ( chk != dba.at( dba.size()-1 ) )
        { // TODO: log an exception
          return;
        }
    }
   else
    { // TODO: log an exception
      return;
    }
}

/**
 * @brief DataFixedLength::operator =  Assign value from a serialized bytearray
 * @param dba - serialized bytearray with typeCode and checksum
 */
void DataFixedLength::operator = ( const QByteArray &dba )
{ DataFixedLength temp( dba );
  ba       = temp.ba;
  typeCode = temp.typeCode;
  return;
}

/**
 * @brief DataFixedLength::toByteArray
 * @return serialized byte array with type, data and checksum
 */
QByteArray DataFixedLength::toByteArray()
{ QByteArray sba;
  // if (( code & AO_FIXED_MASK ) != 0x00 )
  //   TODO: log a warning, maybe check closer for defined types, too.
  unsigned char chk = typeCode;
  sba.append( typeCode );
  for ( int i = 0; i < ba.size(); i++ )
    { sba.append( ba.at(i) ); chk ^= ba.at(i); }
  sba.append( chk );
  return sba;
}

