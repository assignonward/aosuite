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
#include "dataitem.h"

DataItem::DataItem( typeCode_t tc, QObject *p ) : QObject(p), typeCode( tc )
{}

/**
 * @brief DataItem::typeSize
 * @param tc - type code to interpret, or 0xFF (default) to use this object's typeCode
 *  current typeCode,
 *   or -1 if the typeCode doesn't have a defined fixed length.
 */
qint32 DataItem::typeSize( typeCode_t tc ) const
{ if ( tc == AO_UNDEFINED_DATAITEM )
    tc = typeCode;
  switch ( tc & AO_SIZE_MASK )
    { case AO_SIZE_4BYTES  : return   4;
      case AO_SIZE_18BYTES : return  18;
      case AO_SIZE_34BYTES : return  34;
      case AO_SIZE_66BYTES : return  66;
      case AO_SIZE_388BYTES: return 388;
    }
  return -1;
}

/**
 * @brief DataItem::typeSize
 * @param di - byte array to interpret size of the first item in
 */
qint32 DataItem::typeSize( const QByteArray &di ) const
{ if ( di.size() < 1 )
    return -1;
  typeCode_t tc = di.at(0);
  switch ( tc & AO_VARSIZE_MASK )
    { case AO_SIZE_VARLENLONG:
        if ( di.size() < 4 )
          return -1;
        return (((qint32)di.at(1)) * 256 + ((qint32)di.at(2))) * 256 + ((qint32)di.at(3));

      case AO_SIZE_VARLENSHORT:
        if ( di.size() < 2 )
          return -1;
        return (qint32)di.at(1);
    }
  return typeSize( tc );
}

/**
 * @brief DataItem::typeCodeOf
 * @param di - data item to interpret
 * @return typecode of the passed data item
 */
typeCode_t DataItem::typeCodeOf( const QByteArray &di ) const
{ if ( di.size() < 1 )
    return AO_UNDEFINED_DATAITEM;
  char tc = di.at(0);
  return reinterpret_cast<typeCode_t &>( tc );
}
