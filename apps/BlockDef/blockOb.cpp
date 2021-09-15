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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTextCodec>
#include <QHash>
#include <QVariant>

    Dictionary  dict;        // Single dictionary for the application
BlockValueNull  glob_null;   // Returned when BlockValueObject::valueAt() calls an invalid index

/**
 * @brief ValueBase::newValue
 * @param key - tells the type of value object to return
 * @param parent - optional, parent for the new value object
 * @return pointer to the new value object
 */
ValueBase *ValueBase::newValue( RiceyInt key, QObject *parent )
{ if (( key & RDT_ARRAY ) == RDT_ARRAY )
    return new KeyValueArray( key, parent );
  switch ( key & RDT_TYPEMASK )
    { case RDT_OBJECT:    return new BlockValueObject( parent );
      case RDT_INT64:     return new BlockValueInt64( parent );
      case RDT_INT32:     return new BlockValueInt32( parent );
      case RDT_MPZ:       return new BlockValueMPZ( parent );
      case RDT_MPQ:       return new BlockValueMPQ( parent );
      case RDT_RCODE:     return new BlockValueRiceyCode( parent );
      case RDT_STRING:    return new BlockValueString( parent );
      case RDT_BYTEARRAY: return new BlockValueByteArray( parent );
    }
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
    return -1;
  if ( !dict.codesContainCode( k ) )
    return -1;
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
    return -1;
  QDataStream s(b);
  s.setByteOrder(QDataStream::LittleEndian);
  s >> m_value;
  return 8;
}

/**
 * @brief BlockValueInt64::setJson
 * @param j - byte array which should contain a UTF8 encoded integer
 * @return true if conversion was successful
 */
bool BlockValueInt64::setJson( const JsonSerial &j )
{ if ( j.size() < 1 )
    return false;
  bool ok;
  qint64 v = j.toLongLong(&ok);
  if ( ok )
    m_value = v;
  return ok;
}

/**
 * @brief BlockValueInt32::setBsonish
 * @param b - byte array which starts with 8 bytes of LittleEndian encoded integer
 * @return number of bytes converted from the BsonSerial stream, -1 if there was a problem
 */
qint32 BlockValueInt32::setBsonish( const BsonSerial &b )
{ if ( b.size() < 4 )
    return -1;
  QDataStream s(b);
  s.setByteOrder(QDataStream::LittleEndian);
  s >> m_value;
  return 4;
}

/**
 * @brief BlockValueInt32::setJson
 * @param j - byte array which should contain a UTF8 encoded integer
 * @return true if conversion was successful
 */
bool BlockValueInt32::setJson( const JsonSerial &j )
{ if ( j.size() < 1 )
    return false;
  bool ok;
  qint32 v = j.toLong(&ok);
  if ( ok )
    m_value = v;
  return ok;
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
    return -1;
  m_value = b.mid(0,len);
  return len;
}

/**
 * @brief BlockValueRiceyCode::setJson
 * @param j - ricey code in ASCII hexadecimal
 * @return true if conversion was successful
 */
bool  BlockValueRiceyCode::setJson( const JsonSerial &j )
{ RiceyCode r = j.fromHex( j );
  if ( !validRicey( r ) )
    return false;
  m_value = r;
  return true;
}

/**
 * @brief BlockValueByteArray::setBsonish
 * @param b - 4 byte LittleEndian length plus byte array data
 * @return number of bytes converted from the BsonSerial stream, -1 if there was a problem
 */
qint32  BlockValueByteArray::setBsonish( const BsonSerial &b )
{ if ( b.size() < 4 )
    return -1;
  qint32 sz;
  QDataStream s(b);
  s.setByteOrder(QDataStream::LittleEndian);
  s >> sz;
  if ( sz < 0 )
    return -1;
  if (( b.size() < 4+sz ) || ( sz > MAX_LENGTH ))
    return -1;
  if ( sz == 0 )
    m_value = QByteArray();
   else
    m_value = b.mid(4,sz);
  return 4+sz;
}

/**
 * @brief BlockValueByteArray::setJson
 * @param j - ASCII hexadecimal encoded list of octets
 * @return - true always, conversion can't fail, invalid hex is skipped
 */
bool  BlockValueByteArray::setJson( const JsonSerial &j )
{ // TODO: validate hex, or do we care?
  if ( j.size() == 0 )
    m_value = QByteArray();
   else
    m_value = j.fromHex( j );
  return true;
}

/**
 * @brief BlockValueString::bsonish
 * @return rice code length (in bytes, not characters) followed by UTF-8 encoded string
 */
BsonSerial  BlockValueString::bsonish()
{ RiceyCode b = intToRice( m_value.size() );
  b.append( m_value );
  return b;
}

/**
 * @brief BlockValueString::setBsonish
 * @param b - rice coded length in bytes followed by UTF-8 encoded string
 * @return number of bytes converted from the BsonSerial stream, -1 if there was a problem
 */
qint32  BlockValueString::setBsonish( const BsonSerial &b )
{ if ( b.size() < 1 )
    return -1;
  qint32 sz;
  bool ok;
  qint64 length = (qint64)riceToInt( b, &sz, &ok );
  if ( !ok || ( b.size() < (sz + length) ) || (length > MAX_LENGTH) )
    return -1;
  if ( length == 0 )  // empty string is a valid construct
    { m_value = Utf8String();
      return sz;
    }
  QByteArray string = b.mid(sz, length);
  QTextCodec::ConverterState state;
  QTextCodec *codec = QTextCodec::codecForName("UTF-8");
  codec->toUnicode( string.constData(), string.size(), &state );
  if (state.invalidChars > 0) // Checking if string is valid UTF-8?
    return -1;
  m_value = string;
  return sz+length;
}

/**
 * @brief BlockValueString::setJson
 * @param j - json escaped UTF-8 string in quotes
 * @return - true if conversion succeeds
 */
bool  BlockValueString::setJson( const JsonSerial &j )
{ JsonSerial d = "{ \"v\": ";
  d.append( j );
  d.append( " }" );
  QJsonDocument jd = QJsonDocument::fromJson(d);
  if ( !jd.isObject() )
    { qWarning( "document is not a JsonObject" ); return false; }
  QJsonObject o = jd.object();
  if ( !o.contains("v") )
    { qWarning( "object does not contain the v element" ); return false; }
  QJsonValue v = o.value("v");
  if ( !v.isString() )
    { qWarning( "value is not a string" ); return false; }
  m_value = v.toString().toUtf8();
  return true;
}

/**
 * @brief BlockValueString::json - m_value is assumed to be valid UTF-8
 * @return m_value UTF-8 string encoded as a json value field (escaped, in quotes)
 */
JsonSerial  BlockValueString::json()
{ QJsonValue v( QString::fromUtf8(m_value) );
  QJsonObject o;
  o.insert( "k", v );
  QJsonDocument d(o);
  JsonSerial js = d.toJson(QJsonDocument::Compact);
  if (( !js.contains( ':' ) ) || ( !js.contains( '"' ) ))
    return "\"\"";
  qint32 ci = js.indexOf( ':' );
  qint32 fqi = js.indexOf( '"', ci );
  qint32 lqi = js.lastIndexOf( '"' );
  if (( fqi >= lqi ) || ( fqi < 0 ) || ( lqi < 0 ))
    return "\"\"";
  return js.mid( fqi, lqi-fqi+1 );
}

/**
 * @brief bsonishNull - used to prevent bson parse errors when a value is missing
 * @param keyType - key type to make the value for
 * @return a null bson value for the type
 */
BsonSerial ValueBase::bsonishNull( qint8 keyType )
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
    }
  return b;
}

/**
 * @brief KeyValuePair::bsonish
 * @return the bsonish representation of a single key-value pair
 */
BsonSerial  KeyValuePair::bsonish()
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
 * @brief KeyValuePair::json
 * @return the json representation of a single key-value pair
 */
JsonSerial  KeyValuePair::json()
{ JsonSerial j = "{ \"";
  if ( !dict.codesContainCode(m_key) )
    { qWarning( "unknown key 0x%s", keyHexd() );
      return "{<!-- unknown key 0x"+keyHex()+" -->}";
    }
  j.append( dict.nameFromCode(m_key) );
  j.append( "\": " );
  if ( m_value )
    j.append( m_value->json() );
   else
    { qWarning( "null value with 0x%s key", keyHexd() );
      j.append( "0 <!-- null value -->" );
    }
  j.append( " }\n" );
  return j;
}

/**
 * @brief KeyValueArray::bsonish
 * @return The entire array as a key plus list of values (matching the type of the key)
 */
BsonSerial KeyValueArray::bsonish()
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
 * @brief KeyValueArray::json
 * @return json array object with the whole array
 */
JsonSerial  KeyValueArray::json()
{ JsonSerial j = "{ \"";
  if ( !dict.codesContainCode(m_key) )
    return "{<!-- unknown key -->}";
  j.append( dict.nameFromCode(m_key) );
  j.append( "\": [ " );
  bool wroteOne = false;
  foreach ( ValueBase *vp, m_values )
    { if ( vp != nullptr )
        { j.append( vp->json() + " , \n" );
          wroteOne = true;
        }
    }
  if ( wroteOne )
    j = j.mid( 0, j.size() - 3 );
  j.append( "] }\n" );
  return j;
}

bool  KeyValueArray::setJson( const JsonSerial &j )
{ QJsonDocument jd = QJsonDocument::fromJson(j);
  if ( !jd.isObject() )
    { qWarning( "document is not a JsonObject" ); return false; }
  QJsonObject jo = jd.object();
  if ( jo.keys().size() != 1 )
    { qWarning( "object has %d keys (should be 1)", jo.keys().size() ); return false; }
  QStringList keys = jo.keys();
  Utf8String obKey = keys.at(0).toUtf8();
  if ( !dict.codesContainName( obKey ) )
    { qWarning( "dictionary does not contain a %s element", obKey.data() ); return false; }
  setKey( dict.codeFromCodeName( obKey ) );
  QJsonValue jv = jo.value( obKey );
  if ( !jv.isArray() )
    { qWarning( "value is not an array" ); return false; }
  clear();
  QJsonArray ja = jv.toArray();
  if ( ja.size() < 1 )  // Empty?
    return true;        // We're done.
  QVariantList vl = ja.toVariantList();
  foreach ( QVariant v, vl )
    { switch ( type() )
        { case RDT_INT64_ARRAY:     append( (qint64)v.toLongLong() ); break;
          case RDT_INT32_ARRAY:     append( (qint32)v.toInt() ); break;
          case RDT_RCODE_ARRAY:     append( QByteArray::fromHex( v.toString().toUtf8() ) ); break;
          case RDT_STRING_ARRAY:    append( v.toString().toUtf8() ); break;
          case RDT_BYTEARRAY_ARRAY: append( QByteArray::fromHex( v.toString().toUtf8() ) ); break;
          case RDT_OBJECT_ARRAY:
          case RDT_MPZ_ARRAY:
          case RDT_MPQ_ARRAY: qWarning( "unhandled type" ); return false;
        }
    }
  return true;
}

/**
 * @brief BlockValueObject::json
 * @return object as a standard json string
 */
JsonSerial BlockValueObject::json()
{ JsonSerial j = " {";
  QList<RiceyInt> keys = m_obMap.keys();
  bool wroteOne = false;
  foreach ( RiceyInt key, keys )
    { if ( !dict.codesContainCode(key) )
        j.append( " \""+intToRice(key).toHex()+"\" <!-- unknown key --> : " ); // TODO: type extend key with _X
       else
        j.append( " \""+dict.nameFromCode(key)+"\": " );
      ValueBase *vp = m_obMap[key];
      if ( vp != nullptr )
        j.append( vp->json() + " ,\n" );
       else
        j.append( "NULL ,\n" );
      wroteOne = true;
    }
  if ( wroteOne )
    j = j.mid( 0, j.size() - 2 );
  j.append( "\n }\n" );
  return j;
}

/**
 * @brief BlockValueObject::setJson
 * @param j - json sub-string just containing the key-value pairs in the object
 * @return true if successful
 */
bool  BlockValueObject::setJson( const JsonSerial &j )
{ QJsonDocument jd = QJsonDocument::fromJson(j);
  if ( !jd.isObject() )
    { qWarning( "document is not a JsonObject" ); return false; }
  QJsonObject jo = jd.object();
  QStringList keys = jo.keys();
  clear();
  if ( keys.size() < 1 ) // Empty object?
    return true;         // yes, we're done.
  foreach ( QString kStr, keys )
    { if ( !dict.codesContainName( kStr.toUtf8() ) )
        { qWarning( "unknown key %s in object", kStr.toUtf8().data() ); }
       else
        { RiceyInt k = dict.codeFromCodeName( kStr.toUtf8() );
          ValueBase *vbo = jsonValueByKey( k, jo.value( kStr ), this );
          if ( vbo == nullptr )
            { qWarning( "problem reading json value" ); return false; }
          if ( !insert( k, vbo ) )
            { delete vbo; qWarning( "problem inserting value" ); return false; }
        }
    }
  return true;
}

#define JDT_OBJECT 1
#define JDT_DOUBLE 2
#define JDT_STRING 3
#define JDT_ARRAY  4
ValueBase *ValueBase::jsonValueByKey( RiceyInt k, const QJsonValue &jv, QObject *parent )
{ qint32 typ = k & RDT_OBTYPEMASK;
  qint32 jdt = 0;
  bool typeMatch = false;
  switch ( typ )
    { case RDT_OBJECT:    typeMatch = jv.isObject(); jdt = JDT_OBJECT; break;
      case RDT_INT64:
      case RDT_INT32:     typeMatch = jv.isDouble(); jdt = JDT_DOUBLE; break;
      case RDT_MPZ:
      case RDT_MPQ:
      case RDT_RCODE:
      case RDT_STRING:
      case RDT_BYTEARRAY: typeMatch = jv.isString(); jdt = JDT_STRING; break;
      case RDT_OBJECT_ARRAY:
      case RDT_INT64_ARRAY:
      case RDT_INT32_ARRAY:
      case RDT_MPZ_ARRAY:
      case RDT_MPQ_ARRAY:
      case RDT_RCODE_ARRAY:
      case RDT_STRING_ARRAY:
      case RDT_BYTEARRAY_ARRAY: typeMatch = jv.isArray(); jdt = JDT_ARRAY; break;
      default: qWarning( "typefault" ); return nullptr;
    }
  if ( !typeMatch )
    { qWarning( "%s value does not match type %d", dict.nameFromCode(k).data(), typ ); return nullptr; }
  ValueBase *vbo;
  QJsonDocument jd;
  switch ( jdt )
    { case JDT_OBJECT:
        vbo = newValue( k, parent );
        jd.setObject( jv.toObject() );
        vbo->setJson( jd.toJson() );
        return vbo;

      case JDT_DOUBLE: // Better to cast to string, or int?
      case JDT_STRING:
        vbo = newValue( k, parent );
        vbo->setJson( jv.toString().toUtf8() );
        return vbo;

      case JDT_ARRAY:
        vbo = newValue( k, parent );
        jd.setArray( jv.toArray() );
        return vbo;
    }
  return nullptr;
}

/**
 * @brief BlockValueObject::bsonish
 * @return The entire object as a list of key-values
 */
BsonSerial BlockValueObject::bsonish()
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
