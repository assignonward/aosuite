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
#include "datavarint32.h"

/**
 * @brief DataVarInt32::DataVarInt32
 * @param di - data item for initialization
 * @param p - object parent, if any.
 */
DataVarInt32::DataVarInt32( const DataItemBA &di, QObject *p )
  : Data32( 0, AO_UNDEFINED_DATAITEM, p )
{ qint32 tcSz = 0;
  qint32  vSz = 0;
  typeCode = bytesToCode( di          , tcSz );
  v        = bytesToCode( di.mid(tcSz),  vSz );
}

/**
 * @brief DataVarInt32::operator =
 * @param di - data item to assign
 */
void DataVarInt32::operator = ( const DataItemBA &di )
{ DataVarInt32 temp( di );
  v        = temp.v;
  typeCode = temp.typeCode;
  return;
}

/**
 * @brief DataVarInt32::toDataItem
 * @param cf - compact (or chain) form, no difference at this level - unused
 * @return byte array starting with type code, followed by 32 bit data
 */
DataItemBA DataVarInt32::toDataItem( bool cf ) const
{ QByteArray di; (void)cf;
  di.append( codeToBytes( typeCode ) );
  di.append( codeToBytes( v )        );
  return di;
}
