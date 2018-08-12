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
#include "datampz.h"
#include <QByteArray>

/**
 * @brief DataMpz::DataMpz
 * @param di - data item byte array to construct this value from
 * @param p - parent, if any
 */
DataMpz::DataMpz( const DataItemBA &di, QObject *p ) : DataItem( AO_UNDEFINED_DATAITEM, p )
{
  qint32 tcSz = 0;
  typeCode = bytesToCode( di, tcSz );
  if ( di.size() < tcSz+4 )
    { // TODO: log an exception
      return;
    }
  qint32 lenSz = 0;
  qint64 len = bytesToCode( di.mid( tcSz ), lenSz );
  if ( di.size() != tcSz+lenSz+len )
    { qDebug( "size mismatch %d != %d + %d + %lld", di.size(), tcSz, lenSz, len );
      // TODO: log an exception
      return;
    }
  v = byteArrayToMpz( di.mid( tcSz+lenSz ) );
}

/**
 * @brief DataMpz::toDataItem
 * @param cf - chain/compact form?  ignored.
 * @return serialized rational number
 */
DataItemBA DataMpz::toDataItem( bool cf ) const
{ QByteArray di; (void)cf;
  di.append( codeToBytes( typeCode ) );
  QByteArray ba = mpzToByteArray( v );
  di.append( codeToBytes( ba.size() ) );
  di.append( ba );
  return di;
}

/**
 * @brief DataMpz::mpzToByteArray - apologies for the inefficiency
 *   spend time optimizing for speed later.
 * @param n - GMP integer to store in a byte array
 * @return QByteArray encoding n, sign bit encoded in MSb
 */
QByteArray DataMpz::mpzToByteArray( const mpz_class &n )
{ if ( n == 0 )
    { QByteArray z;
      z.append( '\0' );
      return z;
    }
  std::string txt = n.get_str( 16 );
  if ( txt.at(0) == '-' )
    txt.replace(0,1,"");
  int len = static_cast<int>(txt.length());
  // Ensure an even number of hex characters
  if ( len % 2 )
    { txt.insert(0, 1, '0'); // prepend a 0 to make an even number of characters
      len++;
    }
  // Negative numbers expressed by most significant bit == 1
  // When the most signficant bit == 1, it is ignored in the value
  if ( n >= 0 )
    { if ( txt.at(0) >= '8' )
        txt.insert(0, 2, '0'); // Prepend a 0x00 to get the correct sign bit
      return QByteArray::fromHex( QString::fromStdString( txt ).toLatin1() );
    }
  if ( txt.at(0) >= '8' )
    { txt.insert(0, 0, '0'); // Prepend a 0x80 to get the correct sign bit
      txt.insert(0, 0, '8'); // Because when the MSb is 1, it is ignored in the value.
    }
   else
    { txt.at(0) += 8; // Set the hexadecimal MSb
    }
  return QByteArray::fromHex( QString::fromStdString( txt ).toLatin1() );
}

/**
 * @brief DataMpz::byteArrayToMpz - reverse of mpzToByteArray
 * @param ba - byte array to recover into mpz_class
 * @return mpz integer representation of the byte array
 */
mpz_class DataMpz::byteArrayToMpz( const QByteArray &ba )
{ std::string txt = QString( ba.toHex() ).toStdString();
  mpz_class n;
  if ( txt.at(0) < '8' )
    n.set_str( txt, 16 );
   else
    { txt.at(0) -= 8;
      n.set_str( txt, 16 );
      n = -n;
    }
  return n;
}
