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
#include "hash.h"
#include "varsizecode.h"

#include "dataitem-auto.cpp"

/**
 * @brief DataItem::typeCodeOf
 * @param di - data item to interpret
 * @return typecode of the passed data item
 */
typeCode_t DataItem::typeCodeOf( const DataItemBA &di )
{ if ( di.size() < 1 )
    return AO_UNDEFINED_DATAITEM;
  int i;
  return bytesToCode( di, i );
}

/**
 * @brief DataItem::typeSize
 * @param tc - type code to interpret, or 0xFF (default) to use this object's typeCode
 *  current typeCode,
 *   or -1 if the typeCode doesn't have a defined fixed length.
 */
qint32 DataItem::typeSize( typeCode_t tc ) const
{ if ( tc == AO_UNDEFINED_DATAITEM )
    tc = typeCode;
  return typeSizeTable( tc );
}


/**
 * @brief DataItem::typeSize
 * @param di - byte array to interpret size of the first item in,
 *   byte array starts with the variable length type code and if the
 *   type code is not for a fixed length data item, then the variable
 *   length value is decoded and returned.
 * @return total size of this data item, including typeCode and size data, if any
 */
qint32 DataItem::typeSize( const DataItemBA &di ) const
{ if ( di.size() < 1 )
    return -1;
  qint32 tcSz = 0;                   // Size of the typeCode
  typeCode_t tc = bytesToCode( di, tcSz );
  qint32 fdSz = typeSizeTable( tc ); // See if fixed size, or variable, based on type
  if ( fdSz >= 0 )
    return tcSz+fdSz;    // Total size of the data item's byte array, including type code and its fixed size data
  qint32 scSz = 0;       // Size of the size code
  qint32 vdSz = bytesToCode( di.mid( tcSz ), scSz ); // Size of the variable data
  if ( fdSz == -2 )      // -2 means that a single variable byte code value follows the type code
    return tcSz+scSz;    // with no additional data
  return tcSz+scSz+vdSz; // Total size of the data item's byte array, including type and size codes
}


DataItemBA  DataItem::toDataItem( bool cf ) const
{ qDebug( "DataItem:toDataItem ERROR!" );
  (void)cf; return DataItemBA();
} // Implemented in all child classes

/**
 * @brief DataItem::toHashData
 * @param cf - chain/compact form?
 * @return the data item as a type identified byte array, with separable items replaced
 *   with a salted hash.
 */
DataItemBA DataItem::toHashData( bool cf ) const
{ //qDebug( "DataItem:toHashData" );
  if ( typeCodeIsSeparable() )
    { Hash256 h;
      return h.calculate( toDataItem(cf) ).toDataItem(cf);
    }
  return toDataItem(cf);
}

bool DataItem::typeCodeIsSeparable( typeCode_t tc )
{ return ((( tc & AO_SEPARABLE_TYPE1 ) != 0 ) !=
          (( tc & AO_SEPARABLE_TYPE2 ) != 0 ) );
}

bool DataItem::typeCodeIsSeparable() const
{ return typeCodeIsSeparable( getTypeCode() ); }

void  DataItem::debugShow( qint32 level ) const
{ qDebug( "%sDataItem typeCode %lld", qPrintable(QString( level, QChar('.') )), typeCode );
}

/**
 * @brief DataItem::getHash - return the hash of this item
 * @param ht - type of hash to calculate
 * @return The hash value in a hash type identifying wrapper
 */
DataItemBA  DataItem::getHash( typeCode_t ht ) const
{ Hash hi( ht );
  hi.calculate( toHashData() );
  return hi.toDataItem();
}

/**
 * @brief DataItem::verifyHash
 * @param hdi - hash in a type identifying wrapper to verify against this item's current text
 * @return
 */
bool  DataItem::verifyHash( const DataItemBA &hdi ) const
{ Hash hi( hdi );
  return hi.verify( toHashData() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef INCLUDE_TESTS
/**
 * @brief DataItem::testHashVerify - create and verify all kinds of hashes, then modify the hash
 *   and verify that it does not successfully verify.
 * @return true if all tests pass
 */
bool DataItem::testHashVerify()
{ if ( !testHashVerifyType( AO_HASH256       ) ) return false;
  if ( !testHashVerifyType( AO_HASH512       ) ) return false;
  if ( !testHashVerifyType( AO_HASH224SALT32 ) ) return false;
  qDebug( "testHashVerify() PASS all types" );
  return true;
}

/**
 * @brief DataItem::testHashVerifyType - create and verify one kind of hash, then modify the hash
 *   and verify that it does not successfully verify.
 * @param ht - type of hash to test
 * @return true if all tests pass
 */
bool DataItem::testHashVerifyType( typeCode_t ht )
{ // qDebug( "testHashVerifyType( %x )", ht );
  // qDebug( "  data:%s",qPrintable( QString::fromUtf8( toDataItem().toHex() ) ) );
  // qDebug( "  sepr:%s",qPrintable( QString::fromUtf8( toHashData().toHex() ) ) );
  DataItemBA hdi = getHash( ht );
  // qDebug( "  hash:%s",qPrintable( QString::fromUtf8( hdi.toHex() ) ) );
  if ( verifyHash( hdi ) )
    { // qDebug( "  verified when hash is unaltered." );
    }
   else
    { qDebug( "  FAIL, did not verify." );
      return false;
    }
  // Single bit-flip tests
  for ( int i = 0 ; i < hdi.size() ; i++ )
    { for ( int j = 0; j < 8 ; j++ )
        { QByteArray hda = hdi;
          hda[i] = hda.at(i) ^ (0x01 << j);
          if ( verifyHash( hda ) )
            { qDebug( "  FAIL, verified with altered hash %d %d.",i,j );
              qDebug( "  altered hash:%s",qPrintable( QString::fromUtf8( hda.toHex() ) ) );
              return false;
            }
        }
    }
  // qDebug( "  no single bit flips verified." );
  if ( verifyHash( hdi ) )
    { // qDebug( "  re-verified unaltered hash." );
    }
   else
    { qDebug( "  FAIL, unaltered hash did not re-verify." );
      qDebug( "  hash:%s",qPrintable( QString::fromUtf8( hdi.toHex() ) ) );
      return false;
    }
  // qDebug( "  PASS type %x", ht );
  return true;
}

#endif // #ifdef INCLUDE_TESTS
