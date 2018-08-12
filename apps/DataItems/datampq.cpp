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
#include "datampq.h"
#include "datampz.h"
// TODO: store as bytes (base 256) instead of base 10 strings
#define MPQ_SERBASE 10
/**
 * @brief DataMpq::DataMpq
 * @param di - data item byte array to construct this value from
 * @param p - parent, if any
 */
DataMpq::DataMpq( const DataItemBA &di, QObject *p ) : DataItem( AO_UNDEFINED_DATAITEM, p )
{
  qint32 tcSz = 0;
  typeCode = bytesToCode( di, tcSz );
  if ( di.size() < tcSz+4 )
    { // TODO: log an exception
      return;
    }
  qint32 lenSz = 0;
  qint64 len = bytesToCode( di.mid( tcSz ), lenSz );
  if ( di.size() < tcSz+lenSz+len )
    { qDebug( "size mismatch %d != %d + %d + %lld", di.size(), tcSz, lenSz, len );
      // TODO: log an exception
      return;
    }
  qint32 lenNumSz = 0;
  qint64 lenNum = bytesToCode( di.mid( tcSz+lenSz ), lenNumSz );
  mpz_class num = DataMpz::byteArrayToMpz( di.mid( tcSz+lenSz+lenNumSz, static_cast<int>(lenNum) ) );
  mpz_class den = DataMpz::byteArrayToMpz( di.mid( tcSz+lenSz+lenNumSz+static_cast<int>(lenNum) ) );
  v = mpq_class( num, den );
}

/**
 * @brief DataMpq::toDataItem
 * @param cf - chain/compact form?  ignored.
 * @return serialized rational number
 */
DataItemBA DataMpq::toDataItem( bool cf ) const
{ QByteArray nba = DataMpz::mpzToByteArray( v.get_num() );
  QByteArray dba = DataMpz::mpzToByteArray( v.get_den() );
  QByteArray lns = codeToBytes( nba.size() );
  QByteArray di; (void)cf;
  di.append( codeToBytes( typeCode ) );
  di.append( codeToBytes( lns.size()+nba.size()+dba.size() ) );
  di.append( codeToBytes( lns.size() ) );
  di.append( nba );
  di.append( dba );
  return di;
}
