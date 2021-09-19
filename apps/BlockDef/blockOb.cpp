/* MIT License
 *
 * Copyright (c) 2021 Assign Onward
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
#include "blockOb.h"
#include <QTextCodec>
#include <string.h>

size_t strnlength (const char* s, size_t n)
{ const char *found = (char *)memchr(s, '\0', n);
  return found ? (size_t)(found-s) : n;
}

    Dictionary  dict;        // Single dictionary for the application
BlockValueNull  glob_null;   // Returned when BlockValueObject::valueAt() calls an invalid index

/**
 * @brief ValueBase::newValue
 * @param key - tells the type of value object to return
 * @param parent - optional, parent for the new value object
 * @param vtc - optional, value to copy - when not nullptr, copy the value from this
 * @return pointer to the new value object
 */
ValueBase *ValueBase::newValue( RiceyInt key, QObject *parent, ValueBase *vtc )
{ if (( key & RDT_ARRAY ) == RDT_ARRAY )
    { KeyValueArray *kva = new KeyValueArray( key, parent );
      if ( vtc )
        kva->set( ((KeyValueArray *)vtc)->value() );
      return kva;
    }
  qint32 t = key & RDT_TYPEMASK;
  if ( t == RDT_OBJECT    ) { BlockValueObject    *vbo = new BlockValueObject( parent );    if ( vtc ) vbo->set( ((BlockValueObject    *)vtc)->value() ); return vbo; }
  if ( t == RDT_INT64     ) { BlockValueInt64     *vbo = new BlockValueInt64( parent );     if ( vtc ) vbo->set( ((BlockValueInt64     *)vtc)->value() ); return vbo; }
  if ( t == RDT_INT32     ) { BlockValueInt32     *vbo = new BlockValueInt32( parent );     if ( vtc ) vbo->set( ((BlockValueInt32     *)vtc)->value() ); return vbo; }
  if ( t == RDT_MPZ       ) { BlockValueMPZ       *vbo = new BlockValueMPZ( parent );       if ( vtc ) vbo->set( ((BlockValueMPZ       *)vtc)->value() ); return vbo; }
  if ( t == RDT_MPQ       ) { BlockValueMPQ       *vbo = new BlockValueMPQ( parent );       if ( vtc ) vbo->set( ((BlockValueMPQ       *)vtc)->value() ); return vbo; }
  if ( t == RDT_RCODE     ) { BlockValueRiceyCode *vbo = new BlockValueRiceyCode( parent ); if ( vtc ) vbo->set( ((BlockValueRiceyCode *)vtc)->value() ); return vbo; }
  if ( t == RDT_STRING    ) { BlockValueString    *vbo = new BlockValueString( parent );    if ( vtc ) vbo->set( ((BlockValueString    *)vtc)->value() ); return vbo; }
  if ( t == RDT_BYTEARRAY ) { BlockValueByteArray *vbo = new BlockValueByteArray( parent ); if ( vtc ) vbo->set( ((BlockValueByteArray *)vtc)->value() ); return vbo; }
  qWarning( "unhandled type in ValueBase::newValue" );
  return nullptr;
}

/**
 * @brief KeyValueBase::setKey
 * @param key - recognized dictonary entry
 * @return number of bytes the key occupied, or -1 if unsuccessful setting key
 */
qint32 KeyValueBase::setKey( const RiceyCode &key )
{ qint32 len = 0;
  bool ok = false;
  RiceyInt k = riceToInt( key, &len, &ok );
  if ( !ok )
    { qWarning( "problem converting key rice code %s to int", key.toHex().data() ); return -1; }
  if ( !dict.codesContainCode( k ) )
    { qWarning( "dictionary does not contain key %llu", k ); return -1; }
  m_key = k;
  return len;
}

/**
 * @brief KeyValuePair::setBsonish
 * @param b - key followed by a value of type matching the key type
 * @return length of all data read from b to get key and value, or -1 if there was a problem
 */
qint32  KeyValuePair::setBsonish( const BsonSerial &b )
{ qint32 len = setKey( b );
  if ( len < 1 )
    { qWarning( "problem reading key" ); return -1; }
  qint32 i = len;
  ValueBase *vbo = bsonishValueByKey( key(), b.mid(i), &len, this );
  if ( vbo == nullptr )
    { qWarning( "problem reading value" ); return -1; }
  m_value = vbo;
  return i+len;
}

/**
 * @brief BlockValueInt64::setBsonish
 * @param b - byte array which starts with 8 bytes of LittleEndian encoded integer
 * @return number of bytes converted from the BsonSerial stream, -1 if there was a problem
 */
qint32 BlockValueInt64::setBsonish( const BsonSerial &b )
{ if ( b.size() < 8 )
    { qWarning( "undersized array pased to BlockValueInt64::setBsonish" ); return -1; }
  QDataStream s(b);
  s.setByteOrder(QDataStream::LittleEndian);
  s >> m_value;
  return 8;
}


/**
 * @brief BlockValueInt32::setBsonish
 * @param b - byte array which starts with 8 bytes of LittleEndian encoded integer
 * @return number of bytes conve2rted from the BsonSerial stream, -1 if there was a problem
 */
qint32 BlockValueInt32::setBsonish( const BsonSerial &b )
{ if ( b.size() < 4 )
    { qWarning( "undersized array passed to BlockValueInt32::setBsonish" ); return -1; }
  QDataStream s(b);
  s.setByteOrder(QDataStream::LittleEndian);
  s >> m_value;
  return 4;
}

/**
 * @brief BlockValueMPZ::valueEqual
 * @param v - generic block value
 * @return true if v.value() is equal to this.value()
 */
bool BlockValueMPZ::valueEqual( const ValueBase &v ) const
{ if ( v.type() != type() ) return false;
  MP_INT v2 = (((BlockValueMPZ *)&v)->value());
  return ( mpz_cmp( &m_value, &v2 ) == 0);
}

/**
 * @brief BlockValueMPZ::toStr
 * @param in - signed integer to convert
 * @return base 10 string, with negative sign prepended when needed
 */
Utf8String  BlockValueMPZ::toStr( const MP_INT &in )
{ size_t sz = mpz_sizeinbase( &in, 10 );
  if ( sz > MAX_MPINT_LEN )
    { qWarning( "oversized MP_INT %d digits", (int)sz ); return Utf8String( "" ); }
  char str[sz+2];
  mpz_get_str( str, 10, &in );
  return Utf8String( str );
}

  /**
 * @brief BlockValueMPZ::toBCD
 * @param in - signed integer to convert
 * @return BCD string, terminated with an xE/ED if positive or xF/FD if negative, EC/FC if there is a problem
 */
BsonSerial  BlockValueMPZ::toBCD( const MP_INT &in )
{ BsonSerial bcd;
  qint32 sz = mpz_sizeinbase( &in, 10 );
  if ( sz > MAX_MPINT_LEN )
    { qWarning( "oversized MP_INT %d digits", (int)sz ); return Utf8String( "" ); }
  char str[sz+2];
  mpz_get_str( str, 10, &in );
  int len = strnlength( str, MAX_MPINT_LEN );
  if ( len < 1 )
    { bcd.append( (quint8)0xEC ); // empty result, different from 0 which would be 0x0E
      qWarning( "empty string returned from mpz_get_str" );
      return bcd;
    }
  int i = 0;
  bool neg = false;
  if ( str[0] == '-' )
    { neg = true;
      i = 1;
      if ( len == 1 )
        { bcd.append( (quint8)0xFC ); // negative sign only result, different from -0 which would be 0x0F
          qWarning( "lonely negative sign returned from mpz_get_str" );
          return bcd;
        }
    }
  while ( i < len )
    { if ( (i+1) < len )
        { if (( str[i] < '0' ) || ( str[i] > '9' ) || ( str[i+1] < '0' ) || ( str[i+1] > '9' ))
            { qWarning( "unexpected character in mpz_get_str output" );
              bcd.append( neg ? (quint8)0xFC : (quint8)0xEC );
              return bcd;
            }
          bcd.append( ((str[i]-'0') << 4) + (str[i+1]-'0') );
          i += 2;
        }
       else
        { if (( str[i] < '0' ) || ( str[i] > '9' ))
            { qWarning( "unexpected character in mpz_get_str output" );
              bcd.append( neg ? (quint8)0xFC : (quint8)0xEC );
              return bcd;
            }
          bcd.append( ((str[i]-'0') << 4) + ( neg ? 0xF : 0xE ) );
          return bcd;
        }
    }
  bcd.append( ( neg ? 0xFD : 0xED ) );
  return bcd;
}

/**
 * @brief BlockValueMPZ::fromBCD
 * @param bcd - octet stream that starts with a BCD coded integer, as above in toBCD()
 * @param in - (initialized) MP_INT to set the
 * @return number of bytes read from bcd, or -1 if an error was encountered
 */
qint32 BlockValueMPZ::fromBCD( const BsonSerial &bcd, mpz_t in )
{ if ( bcd.size() < 1 )
    { qWarning( "fromBCD was passed an empty array" );
      return 0;
    }
  quint8 secondNybble;
  quint8 firstNybble = (bcd.at(0) & 0xF0) >> 4;
  if ( firstNybble > 9 )
    { qWarning( "fromBCD was passed a non-bcd first nybble %x", firstNybble );
      return 0;
    }
  Utf8String iStr;
  qint32 i = 0;
  while ( i < MAX_MPINT_LEN )
    {  firstNybble = (bcd.at(i) & 0xF0) >> 4;
      secondNybble = (bcd.at(i) & 0x0F);
      i++;
      if ( firstNybble < 10 )
        iStr.append( firstNybble + '0' );
       else
        { if (( firstNybble == 15 ) && ( secondNybble == 13 ))
            { iStr.prepend( "-" );
              mpz_set_str( in, iStr.data(), 10 );
              return i;
            }
           else if (( firstNybble == 14 ) && ( secondNybble == 13 ))
            { mpz_set_str( in, iStr.data(), 10 );
              return i;
            }
           else
            { qWarning( "Invalid first nybble passed to fromBCD %x", firstNybble );
              return -1;
            }
        }
      if ( secondNybble < 10 )
        iStr.append( secondNybble + '0' );
       else
        { if ( secondNybble == 15 )
            { iStr.prepend( "-" );
              mpz_set_str( in, iStr.data(), 10 );
              return i;
            }
           else if ( secondNybble == 14 )
            { mpz_set_str( in, iStr.data(), 10 );
              return i;
            }
           else
            { qWarning( "Invalid first nybble passed to fromBCD %x", firstNybble );
              return -1;
            }
        }
    } // while ( i < MAX_MPINT_LEN )
  qWarning( "fromBCD did not encounter a terminator within %d bytes", i );
  return -1;
}

/**
 * @brief BlockValueRiceyCode::setBsonish
 * @param b - ricey code in octets
 * @return number of bytes converted from the BsonSerial stream, -1 if there was a problem
 */
qint32  BlockValueRiceyCode::setBsonish( const BsonSerial &b )
{ qint32 len = 0;
  bool ok = false;
  riceToInt( b, &len, &ok );
  if ( !ok )
    { qWarning( "riceToInt conversion problem." ); return -1; }
  set( b.mid(0,len) );
  return len;
}

/**
 * @brief BlockValueByteArray::bsonish
 * @return the bsonish representation of this byte array
 */
BsonSerial  BlockValueByteArray::bsonish() const
{ RiceyCode b = intToRice( value().size() );
  b.append( value() );
  return b;
}


/**
 * @brief BlockValueByteArray::setBsonish
 * @param b - Rice Code length plus byte array data
 * @return number of bytes converted from the BsonSerial stream, -1 if there was a problem
 */
qint32  BlockValueByteArray::setBsonish( const BsonSerial &b )
{ if ( b.size() < 1 )
    { qWarning( "undersized array passed to BlockValueByteArray::setBsonish" ); return -1; }
  qint32 sz;
  bool ok;
  qint64 length = (qint64)riceToInt( b, &sz, &ok );
  if ( !ok || ( b.size() < (sz + length) ) || (length > MAX_LENGTH) || ( sz < 0 ) || ( length < 0 ) )
    { qWarning( "riceConversion %d size %d length %lld problem in BlockValueByteArray::setBsonish",ok,sz,length );
      return -1;
    }
  if ( length == 0 )
    set( QByteArray() );
   else
    set( b.mid(sz,length) );
  return length+sz;
}

/**
 * @brief BlockValueString::bsonish
 * @return rice code length (in bytes, not characters) followed by UTF-8 encoded string
 */
BsonSerial  BlockValueString::bsonish() const
{ RiceyCode b = intToRice( value().size() );
  b.append( value() );
  return b;
}

/**
 * @brief BlockValueString::setBsonish
 * @param b - rice coded length in bytes followed by UTF-8 encoded string
 * @return number of bytes converted from the BsonSerial stream, -1 if there was a problem
 */
qint32  BlockValueString::setBsonish( const BsonSerial &b )
{ if ( b.size() < 1 )
    { qWarning( "empty array passed to BlockValueString::setBsonish" ); return -1; }
  qint32 sz;
  bool ok;
  qint64 length = (qint64)riceToInt( b, &sz, &ok );
  if ( !ok || ( b.size() < (sz + length) ) || (length > MAX_LENGTH) )
    { qWarning( "riceConversion %d size %d problem in BlockValueString::setBsonish",ok,sz ); return -1; }
  if ( length == 0 )  // empty string is a valid construct
    { set( Utf8String() );
      return sz;
    }
  QByteArray string = b.mid(sz, length);
  QTextCodec::ConverterState state;
  QTextCodec *codec = QTextCodec::codecForName("UTF-8");
  codec->toUnicode( string.constData(), string.size(), &state );
  if (state.invalidChars > 0) // Checking if string is valid UTF-8?
    { qWarning( "invalid UTF8" ); return -1; }
  set( string );
  return sz+length;
}

/**
 * @brief bsonishNull - used to prevent bson parse errors when a value is missing
 * @param keyType - key type to make the value for
 * @return a null bson value for the type
 */
BsonSerial ValueBase::bsonishNull( qint8 keyType ) const
{ QByteArray b;
  QDataStream s(b);
  s.setByteOrder(QDataStream::LittleEndian);
  switch ( keyType )
    { case RDT_OBJECT:    s << intToRice( 0 );            break;
      case RDT_INT64:     s << (qint64)0;                 break;
      case RDT_INT32:     s << (qint32)0;                 break;
      case RDT_MPZ:                                       break;
      case RDT_MPQ:                                       break;
      case RDT_RCODE:     s << intToRice( RCD_ObTerm_o ); break;
      case RDT_STRING:    s << intToRice( 0 );            break;
      case RDT_BYTEARRAY: s << (qint32)0;                 break;
      default:
      if (( keyType & RDT_ARRAY ) == RDT_ARRAY )
        { s << intToRice( RCD_ObTerm_o ) << intToRice( 0 ); }
       else
        qWarning( "unrecognized keyType in ValueBase::bsonishNull" );
    }
  return b;
}

/**
 * @brief KeyValuePair::bsonish
 * @return the bsonish representation of a single key-value pair
 */
BsonSerial  KeyValuePair::bsonish() const
{ BsonSerial b = keyCode();
  if ( !dict.codesContainCode(m_key) )
    { qWarning( "unknown key 0x%s", keyHexd() );
      return BsonSerial();
    }
  if ( m_value )
    b.append( m_value->bsonish() );
   else
    { qWarning( "null value with 0x%s key", keyHexd() );
      b.append( bsonishNull( type() & RDT_TYPEMASK ) );  // Mask off the array bit too
    }
  return b;
}

/**
 * @brief KeyValueArray::bsonish
 * @return The entire array as a key plus list of values (matching the type of the key)
 */
BsonSerial KeyValueArray::bsonish() const
{ BsonSerial b = keyCode();
  if ( !dict.codesContainCode(b) )
    { qWarning( "unknown key 0x%s", keyHexd() );
      return BsonSerial();
    }
  quint64 elementCount = 0;
  foreach ( ValueBase *vp, m_values )
    if ( vp != nullptr )
      elementCount++;
  b.append( intToRice( elementCount ) );
  foreach ( ValueBase *vp, m_values )
    if ( vp != nullptr )
      b.append( vp->bsonish() );
  return b;
}

/**
 * @brief KeyValueArray::setBsonish
 * @param b - bsonish code for a KeyValueArray to be set into this object
 * @return number of bytes converted from the BsonSerial stream, -1 if there was a problem
 */
qint32  KeyValueArray::setBsonish( const BsonSerial &b )
{ if ( b.size() < 1 )
    { qWarning( "empty BsonSerial" ); return -1; }
  qint32 len = 0;
  bool ok = false;
  RiceyInt k = riceToInt( b, &len, &ok );
  if ( !ok )
    { qWarning( "bad ricey code key" ); return -1; }
  if ( !dict.codesContainCode( k ) )
    { qWarning( "unrecognized key" ); return -1; }
  if ( b.size() <= len )
    { qWarning( "no element count" ); return -1; }
  qint32 i = len;
  quint64 elementCount = riceToInt( b.mid(i), &len, &ok );
  if ( !ok )
    { qWarning( "bad ricey code element count" ); return -1; }
  i += len;
  clear();
  setKey( k ); // Now we also know our data type
  while ( elementCount > 0 )
    { if ( b.size() <= i )
        { qWarning( "value data missing" ); return -1; }
      ValueBase *vbo = newValue( key() & RDT_TYPEMASK, this );
      len = vbo->setBsonish( b.mid(i) );
      if ( len < 1 )
        { delete vbo; qWarning( "problem reading element" ); return -1; }
      append( vbo );
      i += len;
      elementCount--;
    }
  return i;
}

/**
 * @brief KeyValueArray::append
 * @param value - pointer to a generic value, must be type matched to the array
 * @return true if successful
 */
bool  KeyValueArray::append( ValueBase *value )
{ if ( value == nullptr )
    { qWarning( "will not append null" );
      return false;
    }
  if ( value->type() != ( type() & RDT_TYPEMASK ) )
    { qWarning( "type mismatch in KeyValueArray::append( ValueBase *value ) %d %d",(int)value->type(),(int)type() );
      return false;
    }
  m_values.append( value );
  return true;
}

/**
 * @brief KeyValueArray::append
 * @param v - contents ( value() ) of a BlockValueObject
 * @return true if successful
 */
bool  KeyValueArray::append( const BlockObjectMap &v )
{ if ( type() == RDT_OBJECT_ARRAY )
    { return append( new BlockValueObject(v, this) ); }
  qWarning( "type mismatch O %d",(int)type() );
  return false;
}

/**
 * @brief BlockValueObject::bsonish
 * @return The entire object as a list of key-values
 */
BsonSerial BlockValueObject::bsonish() const
{ QList<RiceyInt> keys = m_obMap.keys();
  BsonSerial b = intToRice( m_obMap.size() );
  foreach ( RiceyInt key, keys )
    { b.append( intToRice(key) );
      if ( m_obMap[key] != nullptr )
        b.append( m_obMap[key]->bsonish() );
       else
        b.append( bsonishNull( m_obMap[key]->type() ) );
    }
  return b;
}

/**
 * @brief BlockValueObject::setBsonish
 * @param b - bsonish code for a BlockValueObject to be set into this object
 * @return number of bytes converted from the BsonSerial stream, -1 if there was a problem
 */
qint32  BlockValueObject::setBsonish( const BsonSerial &b )
{ if ( b.size() < 1 )
    { qWarning( "empty BsonSerial" ); return -1; }
  qint32 len = 0;
  bool ok = false;
  qint32 obCount = riceToInt( b, &len, &ok );
  if ( !ok )
    { qWarning( "bad count rice code" ); return -1; }
  clear();
  if ( obCount == 0 )
    return len; // empty object, we're done
  qint32 i = len;
  while ( obCount > 0 )
    { if ( b.size() <= i )
        { qWarning( "object key data missing" ); return -1; }
      RiceyInt k = riceToInt( b.mid(i), &len, &ok );
      if ( !ok )
        { qWarning( "bad key rice code" ); return -1; }
      i += len;
      if ( b.size() <= i )
        { qWarning( "object key data missing" ); return -1; }
      ValueBase *vbo = bsonishValueByKey( k, b.mid(i), &len, this );
      if ( vbo == nullptr )
        { qWarning( "problem reading bsonish value" ); return -1; }
      if ( !insert( k, vbo ) )
        { delete vbo; qWarning( "problem inserting value" ); return -1; }
      i += len;
      obCount--;
    }
  return i;
}

/**
 * @brief ValueBase::bsonishValueByKey
 * @param k - key as a RiceyInt which tells the type of the value to be read
 * @param b - bytestream ready to read a value from
 * @param l - when not null, return length of stream read here, -1 if there is a problem
 * @param parent - parent for the value object
 * @return length of stream read to get the value, or -1 if there was a problem
 */
ValueBase *ValueBase::bsonishValueByKey( RiceyInt k, const BsonSerial &b, qint32 *l, QObject *parent )
{ if ( l != nullptr )
    *l = -1;
  ValueBase *vbo = newValue( k, parent );
  qint32 len = vbo->setBsonish( b );
  if ( len < 1 )
    { delete vbo; qWarning( "problem reading value" ); return nullptr; }
  if ( l != nullptr )
    *l = len;
  return vbo;
}

/**
 * @brief BlockValueObject::insert
 * @param k - key
 * @param v - value
 * @return true if insertion was successful
 */
bool  BlockValueObject::insert( RiceyInt k, ValueBase *v )
{ if ( v == nullptr )
    { qWarning( "will not insert null value" ); return false; }
  if (( k & RDT_OBTYPEMASK ) != v->type() )
    { qWarning( "will not insert mismatched type and value" ); return false; }
  if ( !dict.codesContainCode( k ) )
    { qWarning( "key not recognized, will not insert." ); return false; }
  if ( m_obMap.contains( k ) )
    { qWarning( "type collision, insertion blocked." ); return false; }
  m_obMap.insert( k, v );
  return true;
}

/**
 * @brief BlockValueObject::insert
 * @param vl - list of items to insert
 * @return number of successfully inserted objects
 */
qint32  BlockValueObject::insert( const BlockObjectMap &vl )
{ qint32 count = 0;
  QList<RiceyInt> keys = vl.keys();
  foreach( RiceyInt k, keys )
    { if ( insert( k, newValue( k, this, vl[k] ) ) )
        count++;
       else
        qWarning( "insert Failed" );
    }
  return count;
}

/**
 * @brief BlockValueObject::operator ==
 * @param v - BlockObjectMap to compare
 * @return true if equal
 */
bool BlockValueObject::operator==( const BlockObjectMap &v ) const
{ if ( v.size() != m_obMap.size() )
    return false;
  if ( v.size() == 0 )
    return true;
  QList<RiceyInt> keys = v.keys();
  foreach ( RiceyInt k, keys )
    { if ( !m_obMap.contains( k ) )
        return false;
      ValueBase *vv = v.value(k);
      ValueBase *vt = value(k);
      if ( vv->type() != vt->type() )
        { qWarning( "BlockValueObject::operator== type mismatch %d %d",vv->type(),vt->type() ); return false; }
      switch ( k & RDT_OBTYPEMASK )
        { case RDT_OBJECT:    if ( !(*((BlockValueObject    *)vt)          == *((BlockValueObject    *)vv)          ) ) return false; break;
          case RDT_INT64:     if (  ( ((BlockValueInt64     *)vt)->value() !=  ((BlockValueInt64     *)vv)->value() ) ) return false; break;
          case RDT_INT32:     if (  ( ((BlockValueInt32     *)vt)->value() !=  ((BlockValueInt32     *)vv)->value() ) ) return false; break;
//        case RDT_MPZ:       if (  ( ((BlockValueMPZ       *)vt)->value() !=  ((BlockValueMPZ       *)vv)->value() ) ) return false; break;
//        case RDT_MPQ:       if (  ( ((BlockValueMPQ       *)vt)->value() !=  ((BlockValueMPQ       *)vv)->value() ) ) return false; break;
          case RDT_RCODE:     if (  ( ((BlockValueRiceyCode *)vt)->value() !=  ((BlockValueRiceyCode *)vv)->value() ) ) return false; break;
          case RDT_STRING:    if (  ( ((BlockValueString    *)vt)->value() !=  ((BlockValueString    *)vv)->value() ) ) return false; break;
          case RDT_BYTEARRAY: if (  ( ((BlockValueByteArray *)vt)->value() !=  ((BlockValueByteArray *)vv)->value() ) ) return false; break;
          case RDT_OBJECT_ARRAY:

          case RDT_INT64_ARRAY:     if (  ( ((BlockArrayInt64     *)vt)->value() !=  ((BlockArrayInt64     *)vv)->value() ) ) return false; break;
          case RDT_INT32_ARRAY:     if (  ( ((BlockArrayInt32     *)vt)->value() !=  ((BlockArrayInt32     *)vv)->value() ) ) return false; break;
//        case RDT_MPZ_ARRAY:       if (  ( ((BlockArrayMPZ       *)vt)->value() !=  ((BlockArrayMPZ       *)vv)->value() ) ) return false; break;
//        case RDT_MPQ_ARRAY:       if (  ( ((BlockArrayMPQ       *)vt)->value() !=  ((BlockArrayMPQ       *)vv)->value() ) ) return false; break;
          case RDT_RCODE_ARRAY:     if (  ( ((BlockArrayRicey     *)vt)->value() !=  ((BlockArrayRicey     *)vv)->value() ) ) return false; break;
          case RDT_STRING_ARRAY:    if (  ( ((BlockArrayString    *)vt)->value() !=  ((BlockArrayString    *)vv)->value() ) ) return false; break;
          case RDT_BYTEARRAY_ARRAY: if (  ( ((BlockArrayByteArray *)vt)->value() !=  ((BlockArrayByteArray *)vv)->value() ) ) return false; break;
          default: qWarning( "unhandled type %s", dict.nameFromCode( k ).data() ); return false;
        }
    }
  return true;
}

/**
 * @brief BlockArrayObject::operator ==
 * @param l - list of object values
 * @return true if this list of object values is the same as l's
 */
bool  BlockArrayObject::operator==(const QList<BlockObjectMap>& l) const
{ if (l.size() != size())
    return false;
  for ( qint32 i=0; i<size(); i++ )
    { BlockObjectMap om = l.at(i);
      BlockObjectMap tm = at(i);
      if ( om.size() != tm.size() )
        return false;
      QList<RiceyInt> keys = om.keys();
      foreach ( RiceyInt k, keys )
        { if ( !tm.contains(k) )
            return false;
          if ( !om[k]->valueEqual( *(tm[k]) ) )
            return false;
        }
    }
  return true;
}

