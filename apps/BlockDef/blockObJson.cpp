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
#include <QVariant>

// Design Intent:
//
// The json() and setJson() functions of block object family members are intended as
//   a bi-directional serialization/deserialization pathway to/from a somewhat human
//   readable form for the block data.  Although json is not the primary form for
//   block data, it should be possible to serialize a block as json, edit it, then
//   deserialize the edited json data back into a block object.  Operations like
//   hash and digital signatures require the bsonish representation of the data, so
//   they cannot be calculated directly on json representations without first
//   translating to bsonish.
//
// State of the code:
//
// Although the Qt Json library functions are convenient for parsing of structures
//   like arrays, they are struggling with accuracy of larger int64 numbers due to
//   internal representation of such numbers as double floats.  I am beginning to
//   question the utility of including the int32 data type, and also the value of
//   representing qint64 as naked integers in json rather than ASCII decimal strings
//   which would not suffer the precision problems of the bare integers.
//

/**
 * @brief BlockValueInt64::setJson
 * @param j - byte array which should contain a UTF8 encoded integer
 * @return true if conversion was successful
 */
bool BlockValueInt64::setJson( const JsonSerial &j )
{ if ( j.size() < 1 )
    { qWarning( "empty json" ); return false; }
  bool ok;
  qint64 v = j.toLongLong(&ok);
  if ( ok )
    m_value = v;
   else
    qWarning( "problem converting '%s' to int64", j.data() );
  return ok;
}

/**
 * @brief BlockValueMPZ::setJson
 * @param j - json representation of the big integer
 * @return true if successful
 */
bool BlockValueMPZ::setJson( const JsonSerial &j )
{ JsonSerial t = j.trimmed();
  if ( t.size() < 3 )
    { qWarning( "empty array passed to BlockValueMPZ::setJson" ); return false; }
  if ( t.at(0) != '"' )
    { qWarning( "BlockValueMPZ::setJson expected to start with \"" ); return false; }
  t = t.mid(1);
  if ( t.at(t.size()-1) != '"' )
    { qWarning( "BlockValueMPZ::setJsonexpected to end with \"" ); return false; }
  t.chop(1);
  t.append( (quint8)0 );

  int ok = mpz_set_str( &m_value, t.data(), 10 );
  if ( ok != 0 )
    qWarning( "problem in converting json to MPZ MP_INT %s %s",j.data(),t.data() );
  return ( ok == 0 );
}

/**
 * @brief BlockValueMPQ::setJson
 * @param j - json representation of two big integers
 * @return true if successful
 */
bool BlockValueMPQ::setJson( const JsonSerial &j )
{ JsonSerial t = j.trimmed();
  if ( t.size() < 3 )
    { qWarning( "empty array passed to BlockValueMPQ::setJson" ); return false; }
  if ( t.at(0) != '"' )
    { qWarning( "BlockValueMPQ::setJson expected to start with \"" ); return false; }
  t = t.mid(1);
  if ( t.at(t.size()-1) != '"' )
    { qWarning( "BlockValueMPQ::setJsonexpected to end with \"" ); return false; }
  t.chop(1);
  t.append( (quint8)0 );
  if ( ! (t.count( '/' ) == 1) )
    { qWarning( "BlockValueMPQ::setJsonexpected to contain one and only /" ); return false; }
  qint32 si = t.indexOf( '/' );
  if (( si <= 0 ) || ( si >= (t.size() - 1) ))
    { qWarning( "BlockValueMPQ::setJsonexpected to include numbers on either side of the /" ); return false; }
  JsonSerial ns = t.mid( 0,si ); ns.append((quint8)0);
  JsonSerial ds = t.mid(si+1);   ds.append((quint8)0);
  // qWarning( "ns %s ds %s", ns.data(), ds.data() );
  MP_INT n;
  MP_INT d;
  int ok;
  ok = mpz_init_set_str( &n, ns.data(), 10 );
  if ( ok != 0 )
    { qWarning( "problem in converting numerator json to MPZ MP_INT %s",ns.data() ); return false; }
  ok = mpz_init_set_str( &d, ds.data(), 10 );
  if ( ok != 0 )
    { qWarning( "problem in converting denominator json to MPZ MP_INT %s",ds.data() ); return false; }
  mpq_set_num( &m_value, &n );
  mpq_set_den( &m_value, &d );
  mpz_clear( &n );
  mpz_clear( &d );
  return true;
}

/**
 * @brief BlockValueRiceyCode::setJson
 * @param j - ricey code in ASCII hexadecimal
 * @return true if conversion was successful
 */
bool  BlockValueRiceyCode::setJson( const JsonSerial &j )
{ RiceyCode r = j.fromHex( j );
  if ( !validRicey( r ) )
    { qWarning( "invalid ricey code in json" ); return false; }
  m_value = r;
  return true;
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
    { qWarning( "BlockValueString::document is not a JsonObject '%s'",d.data() ); return false; }
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
JsonSerial  BlockValueString::json() const
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
  // TODO: refactor to expose this encoding to the string array
}

/**
 * @brief KeyValuePair::json
 * @return the json representation of a single key-value pair
 */
JsonSerial  KeyValuePair::json() const
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
 * @brief KeyValueArray::json
 * @return json array object with the key and the whole array
 */
JsonSerial  KeyValueArray::json() const
{ JsonSerial j = " \"";
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
  j.append( "] \n" );
  return j;
}

bool  KeyValueArray::setJson( const JsonSerial &j )
{ QJsonDocument jd = QJsonDocument::fromJson( "{"+j+"}" );
  if ( !jd.isObject() )
    { qWarning( "KeyValueArray::document is not a JsonObject '%s'",j.data() ); return false; }
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
  if ( type() == RDT_OBJECT_ARRAY )
    { foreach ( QJsonValue jv, ja )
        { if ( !jv.isObject() )
            qWarning( "array element is not object in KeyValueArray::setJson() type OBJECT_ARRAY" );
           else
            { QJsonObject jo = jv.toObject();
              QJsonDocument ed( jo );
              BlockValueObject *bvo = new BlockValueObject( this, ed.toJson() );
              append( bvo->value() );
              delete bvo;
            }
        }
    }
   else
    { QVariantList vl = ja.toVariantList();
      foreach ( QVariant v, vl )
        { switch ( type() )
            { case RDT_INT64_ARRAY:     append( (qint64)v.toLongLong() ); break;
              case RDT_RCODE_ARRAY:     append( QByteArray::fromHex( v.toString().toUtf8() ) ); break;
              case RDT_STRING_ARRAY:    append( v.toString().toUtf8() ); break; // TODO: encode using the same escapes as in the BlockString.json() function
              case RDT_BYTEARRAY_ARRAY: append( QByteArray::fromHex( v.toString().toUtf8() ) ); break;
              case RDT_MPZ_ARRAY:
              case RDT_MPQ_ARRAY:       append( v.toString().toUtf8() ); break;
              default: qWarning( "unhandled type in KeyValueArray::setJson()" ); return false;
    }   }   }
  return true;
}

/**
 * @brief BlockValueObject::json
 * @return object as a standard json string
 */
JsonSerial BlockValueObject::json() const
{ JsonSerial j = " {";
  QList<RiceyInt> keys = m_obMap.keys();
  bool wroteOne = false;
  foreach ( RiceyInt key, keys )
    { ValueBase *vp = m_obMap[key];
      if ( vp != nullptr )
        { if ( key & RDT_ARRAY )
            j.append( vp->json() + " ,\n" );
           else
            { if ( !dict.codesContainCode(key) )
                { j.append( " \""+intToRice(key).toHex()+"\" <!-- unknown key --> : " ); qWarning( "json Object unknown key" ); } // TODO: type extend key with _X
               else
                j.append( " \""+dict.nameFromCode(key)+"\": " );
              if ( vp != nullptr )
                j.append( vp->json() + " ,\n" );
               else
                { j.append( "NULL ,\n" ); qWarning( "json Object conversion encountered NULL" ); }
            }
          wroteOne = true;
        }
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
    { if ( !jd.isArray() )
        { qWarning( "BlockValueObject::document is not a JsonObject or JsonArray '%s'",j.data() ); return false; }
      qWarning( "TODO: process array '%s'",j.data() );
      return false;
    }
  QJsonObject jo   = jd.object();
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
            { qWarning( "problem reading json value %s from %s", kStr.toUtf8().data(), j.data() ); return false; }
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
/**
 * @brief ValueBase::jsonValueByKey
 * @param k - key to help determine/verify value type
 * @param jv - single json value read from larger document
 * @param parent - object parent for the ValueBase object created
 * @return pointer to a ValueBase object with the value of the JsonValue, nullptr if there is a problem
 */
ValueBase *ValueBase::jsonValueByKey( RiceyInt k, const QJsonValue &jv, QObject *parent )
{ qint32 typ = k & RDT_OBTYPEMASK;
  qint32 jdt = 0;
  bool typeMatch = false;
  switch ( typ )
    { case RDT_OBJECT:    typeMatch = jv.isObject(); jdt = JDT_OBJECT; break;
      case RDT_INT64:     typeMatch = jv.isDouble(); jdt = JDT_DOUBLE; break;
      case RDT_MPZ:
      case RDT_MPQ:
      case RDT_RCODE:
      case RDT_STRING:
      case RDT_BYTEARRAY: typeMatch = jv.isString(); jdt = JDT_STRING; break;
      case RDT_OBJECT_ARRAY:
      case RDT_INT64_ARRAY:
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
  QString str;
  switch ( jdt )
    { case JDT_OBJECT:
        vbo = newValue( k, parent );
        jd.setObject( jv.toObject() );
        vbo->setJson( jd.toJson() );
        return vbo;

      case JDT_DOUBLE:
        if ( typ == RDT_INT64 ) { return new BlockValueInt64( jv.toInt(), this ); }
        return nullptr;

      case JDT_STRING:
        vbo = newValue( k, parent );
        if (( typ == RDT_STRING ) || ( typ == RDT_MPZ ) || ( typ == RDT_MPQ ))
          vbo->setJson( "\""+jv.toString().toUtf8()+"\"" );
         else
          vbo->setJson(      jv.toString().toUtf8()      );
        return vbo;

      case JDT_ARRAY:
        vbo = newValue( k, parent );
        jd.setArray( jv.toArray() );
        if ( !vbo->setJson( " \""+dict.nameFromCode(k)+"\": "+jd.toJson() ) )
          { vbo->deleteLater();
            qWarning( "problem setting array from json" );
            return nullptr;
          }
        return vbo;
    }
  qWarning( "unrecognized json value type" );
  return nullptr;
}
