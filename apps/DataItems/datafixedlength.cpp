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
DataFixedLength::DataFixedLength( const DataItemBA &di, QObject *p )
                   : DataItem( AO_UNDEFINED_DATAITEM, p )
{ qint32 tcSz = 0;
  typeCode = bytesToCode( di, tcSz );
  if ( di.size() < typeSize()+tcSz )
    { // TODO: log an exception
      return;
    }
  ba.append( di.mid( tcSz ) );
}

/**
 * @brief DataFixedLength::operator =  Assign value from a serialized bytearray
 * @param di - serialized bytearray with typeCode and checksum
 */
void DataFixedLength::operator = ( const DataItemBA &di )
{ DataFixedLength temp( di );
  ba       = temp.ba;
  typeCode = temp.typeCode;
  return;
}

/**
 * @brief DataFixedLength::toDataItem
 * @param cf - compact (or chain) form, no difference at this level - unused
 * @return serialized byte array with type, data and checksum
 */
DataItemBA DataFixedLength::toDataItem( bool cf ) const
{ QByteArray di; (void)cf;
  di.append( codeToBytes( typeCode ) );
  di.append( ba );
  // qDebug( "DataFixedLength::toDataItem(size=%d)%s", ba.size(), qPrintable( QString::fromUtf8( di.toHex() ) ) );
  return di;
}

/**
 * @brief DataFixedLength::set - if sba is the right size for typeCode, set ba = sba.
 * @param sba - byte array to set
 */
void DataFixedLength::set( QByteArray sba )
{ if ( sba.size() == (typeSize() - 2) )
    { ba    = sba;
    }
  // else
  // TODO: log warning
}
