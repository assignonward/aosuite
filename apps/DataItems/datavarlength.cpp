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
#include "datavarlength.h"

/**
 * @brief DataVarLength::DataVarLength - constructor from serialized form
 * @param di - typecode, extensible size, data
 * @param p - parent object
 */
DataVarLength::DataVarLength( const DataItemBA &di, QObject *p )
  : DataItem( typeCodeOf(di), p )
{ if ( di.size() < 2 ) // Shortest valid varlenlong serialized data (1 type + 1 length + 0 data)
    { // TODO: log an exception
      return;
    }
  qint32 i,j;
  typeCode   = bytesToCode( di       , i );
  quint32 sz = bytesToCode( di.mid(i), j );
  if ( (unsigned int)di.size() < (sz+i+j) )
    { qDebug( "problem with size of DataItemBA: %d vs encoded size: %d for type: 0x%x", di.size(),sz,typeCode );
      // TODO: log an exception
      return;
    }
  ba = di.mid(i+j,sz);
}

/**
 * @brief DataVarLength::operator =  Assign values from a serialized bytearray
 * @param di - serialized bytearray with typeCode, size, data and checksum
 */
void DataVarLength::operator = ( const DataItemBA &di )
{ DataVarLength temp( di );
  ba       = temp.ba;
  typeCode = temp.typeCode;
  return;
}

/**
 * @brief DataVarLength::toDataItem
 * @param cf - compact (or chain) form, no difference at this level - unused
 * @return serialized bytearray with typeCode, size, data and checksum
 */
DataItemBA DataVarLength::toDataItem( bool cf ) const
{ qDebug( "DataVarLength::toDataItem() for tc 0x%x", typeCode  );
  DataItemBA di; (void)cf;
  di.append( codeToBytes( typeCode  ) );
  di.append( codeToBytes( ba.size() ) );
  di.append( ba );
  return di;
}
