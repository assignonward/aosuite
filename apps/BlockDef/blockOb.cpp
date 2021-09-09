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

QHash<QByteArray,QByteArray> keyNames;  // key

/**
 * @brief initKeyNames - easy code generator output, TODO: initialize from the json definition instead.
 */
void initKeyNames()
{ keyNames.insert( QByteArray::fromHex(     "00" ), "ObTerm_o"          );
  keyNames.insert( QByteArray::fromHex(     "01" ), "int64_i"           );
  keyNames.insert( QByteArray::fromHex(     "02" ), "int32_l"           );
  keyNames.insert( QByteArray::fromHex(     "03" ), "mpz_n"             );
  keyNames.insert( QByteArray::fromHex(     "04" ), "mpq_r"             );
  keyNames.insert( QByteArray::fromHex(     "05" ), "type_y"            );
  keyNames.insert( QByteArray::fromHex(     "06" ), "text_s"            );
  keyNames.insert( QByteArray::fromHex(     "07" ), "data_b"            );
  keyNames.insert( QByteArray::fromHex(     "10" ), "chainBlock_o"      );
  keyNames.insert( QByteArray::fromHex(     "20" ), "signedBlock_o"     );
  keyNames.insert( QByteArray::fromHex(     "30" ), "parentSignature_o" );
  keyNames.insert( QByteArray::fromHex(     "40" ), "signature_o"       );
  keyNames.insert( QByteArray::fromHex(     "11" ), "time_i"            );
  keyNames.insert( QByteArray::fromHex(     "13" ), "AOShares_n"        );
  keyNames.insert( QByteArray::fromHex(     "15" ), "SHA256_y"          );
  keyNames.insert( QByteArray::fromHex(     "25" ), "ECB256_y"          );
  keyNames.insert( QByteArray::fromHex(   "8025" ), "RSA3072_y"         );
  keyNames.insert( QByteArray::fromHex(   "8015" ), "SHA3b512_y"        );
  keyNames.insert( QByteArray::fromHex(   "9015" ), "jpg_y"             );
  keyNames.insert( QByteArray::fromHex(   "9025" ), "png_y"             );
  keyNames.insert( QByteArray::fromHex( "A0B000" ), "AOGenesisBlock_o"  );
  keyNames.insert( QByteArray::fromHex( "A0CD00" ), "AOChainDesc_o"     );
  keyNames.insert( QByteArray::fromHex( "A0CF00" ), "AOChainFunc_o"     );
  keyNames.insert( QByteArray::fromHex( "A0EC00" ), "AOEndChain_o"      );
  keyNames.insert( QByteArray::fromHex( "A0CD06" ), "Symbol_s"          );
  keyNames.insert( QByteArray::fromHex( "A0CD16" ), "CdName_s"          );
  keyNames.insert( QByteArray::fromHex( "A0CD26" ), "Tagline_s"         );
  keyNames.insert( QByteArray::fromHex( "A0CD36" ), "Description_s"     );
  keyNames.insert( QByteArray::fromHex( "A0CD10" ), "Icon_o"            );
  keyNames.insert( QByteArray::fromHex( "A0CD20" ), "Banner_o"          );
  keyNames.insert( QByteArray::fromHex( "A0CD30" ), "Image_o"           );
  keyNames.insert( QByteArray::fromHex( "A0CF03" ), "CfShares_n"        );
  keyNames.insert( QByteArray::fromHex( "A0CF13" ), "CfCoins_n"         );
  keyNames.insert( QByteArray::fromHex( "A0CF04" ), "CfRecFee_r"        );
}

/**
 * @brief BlockValueObject::~BlockValueObject - cleanup the data elements on destruction
 */
BlockValueObject::~BlockValueObject()
{ foreach( QPointer<KeyValuePair> blockOb, m_obMap )
    if ( blockOb )
      delete( blockOb );
}

/**
 * @brief BlockValueInt64::setBsonish
 * @param b - byte array which starts with 8 bytes of LittleEndian encoded integer
 * @return true if conversion was successful
 */
bool BlockValueInt64::setBsonish( const QByteArray &b )
{ if ( b.size() < 8 )
    return false;
  QDataStream s(b);
  s.setByteOrder(QDataStream::LittleEndian);
  s >> m_value;
  return true;
}

/**
 * @brief BlockValueInt64::setJson
 * @param j - byte array which should contain a UTF8 encoded integer
 * @return true if conversion was successful
 */
bool BlockValueInt64::setJson( const QByteArray &j )
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
 * @return true if conversion was successful
 */
bool BlockValueInt32::setBsonish( const QByteArray &b )
{ if ( b.size() < 4 )
    return false;
  QDataStream s(b);
  s.setByteOrder(QDataStream::LittleEndian);
  s >> m_value;
  return true;
}

/**
 * @brief BlockValueInt32::setJson
 * @param j - byte array which should contain a UTF8 encoded integer
 * @return true if conversion was successful
 */
bool BlockValueInt32::setJson( const QByteArray &j )
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
 * @return true if conversion was successful
 */
bool  BlockValueRiceyCode::setBsonish( const QByteArray &b )
{ if ( !validRicey( b ) )
    return false;
  m_value = b;
  return true;
}

/**
 * @brief BlockValueRiceyCode::setJson
 * @param j - ricey code in ASCII hexadecimal
 * @return true if conversion was successful
 */
bool  BlockValueRiceyCode::setJson( const QByteArray &j )
{ QByteArray b = j.fromHex( j );
  if ( !validRicey( b ) )
    return false;
  m_value = b;
  return true;
}

/**
 * @brief BlockValueRiceyCode::validRicey
 * @param b - byte array to evaluate
 * @return true if b contains a valid ricey code
 */
bool  BlockValueRiceyCode::validRicey( const QByteArray &b )
{ if ( b.size() < 1 )
    return false;
  if ( b.size() > 7 )
    return false;

  for ( int i = 0; i < b.size(); i++ )
    { if ( i == b.size() - 1 )
        { if (( b.at(i) & 0x80 ) != 0)
            return false;
        }
       else
        { if (( b.at(i) & 0x80 ) == 0)
            return false;
        }
    }
  return true;
}

/**
 * @brief BlockValueByteArray::setBsonish
 * @param b - 4 byte LittleEndian length plus byte array data
 * @return true if conversion was successful (zero length is still valid)
 */
bool  BlockValueByteArray::setBsonish( const QByteArray &b )
{ if ( b.size() < 4 )
    return false;
  qint32 sz;
  QDataStream s(b);
  s.setByteOrder(QDataStream::LittleEndian);
  s >> sz;
  if ( sz < 0 )
    return false;
  if ( b.size() < 4+sz )
    return false;
  if ( sz == 0 )
    m_value = QByteArray();
   else
    m_value = b.mid(4,sz);
  return true;
}

/**
 * @brief BlockValueByteArray::setJson
 * @param j - ASCII hexadecimal encoded list of octets
 * @return - true always, conversion can't fail, invalid hex is skipped
 */
bool  BlockValueByteArray::setJson( const QByteArray &j )
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
QByteArray  BlockValueString::bsonish()
{ QByteArray b = intToRice( m_value.size() );
  b.append( m_value );
  return b;
}

/**
 * @brief BlockValueString::setBsonish
 * @param b - rice coded length in bytes followed by UTF-8 encoded string
 * @return true if conversion was successful (zero length is still valid)
 */
bool  BlockValueString::setBsonish( const QByteArray &b )
{ if ( b.size() < 1 )
    return false;
  qint32 sz;
  bool ok;
  qint64 length = (qint64)riceToInt( b, &sz, &ok );
  if ( !ok || ( b.size() < (sz + length) ) )  // TODO: add a protocol context that includes properties like max string length
    return false;
  if ( sz == 0 )  // empty string is a valid construct
    { m_value = QByteArray();
      return true;
    }
  QByteArray string = b.mid(sz, length);
  QTextCodec::ConverterState state;
  QTextCodec *codec = QTextCodec::codecForName("UTF-8");
  codec->toUnicode( string.constData(), string.size(), &state );
  if (state.invalidChars > 0) // Checking if string is valid UTF-8?
    return false;
  m_value = string;
  return true;
}

/**
 * @brief BlockValueString::setJson
 * @param j - json escaped UTF-8 string in quotes
 * @return - true if conversion succeeds
 */
bool  BlockValueString::setJson( const QByteArray &j )
{ QByteArray d = "{ 'v':";
  d.append( j );
  d.append( " }" );
  QJsonDocument jd = QJsonDocument::fromJson(d);
  if ( !jd.isObject() )
    return false;
  QJsonObject o = jd.object();
  if ( !o.contains("v") )
    return false;
  QJsonValue v = o.value("v");
  if ( !v.isString() )
    return false;
  m_value = v.toString().toUtf8();
  return true;
}

/**
 * @brief BlockValueString::json - m_value is assumed to be valid UTF-8
 * @return m_value UTF-8 string encoded as a json value field (escaped, in quotes)
 */
QByteArray  BlockValueString::json()
{ QJsonValue v( QString::fromUtf8(m_value) );
  QJsonObject o;
  o.insert( "k", v );
  QJsonDocument d(o);
  QByteArray ba = d.toJson(QJsonDocument::Compact);
  if (( !ba.contains( ':' ) ) || ( !ba.contains( '"' ) ))
    return QByteArray( "\"\"" );
  qint32 ci = ba.indexOf( ':' );
  qint32 fqi = ba.indexOf( '"', ci );
  qint32 lqi = ba.lastIndexOf( '"' );
  if (( fqi >= lqi ) || ( fqi < 0 ) || ( lqi < 0 ))
    return QByteArray( "\"\"" );
  return ba.mid( fqi, lqi-fqi+1 );
}

/**
 * @brief bsonishNull - used to prevent bson parse errors when a value is missing
 * @param keyType - key type to make the value for
 * @return a null bson value for the type
 */
QByteArray bsonishNull( qint8 keyType )
{ QByteArray b;
  QDataStream s(b);
  s.setByteOrder(QDataStream::LittleEndian);
  switch ( keyType )
    { case RDT_OBJECT:      return b;
      case RDT_INT64:     { s << (qint64)0; return b; }
      case RDT_INT32:     { s << (qint32)0; return b; }
      case RDT_MPZ:         return b;
      case RDT_MPQ:         return b;
      case RDT_RCODE:     { return QByteArray( 0 ); }
      case RDT_STRING:    { s << (qint32)0; return b; }
      case RDT_BYTEARRAY: { s << (qint32)0; return b; }
    }
  return b;
}

/**
 * @brief BlockObject::bsonish
 * @return the bsonish representation of a single key-value pair
 */
QByteArray  KeyValuePair::bsonish()
{ QByteArray b = m_key;
  if ( b.size() < 1 )                                // empty key, empty bson
    return QByteArray();
  if ( !BlockValueRiceyCode::validRicey( m_key ) )  // invalid key, empty bson
    return QByteArray();
  if ( m_value )
    b.append( m_value->bsonish() );
   else
    b.append( bsonishNull( m_key.at( m_key.size()-1 ) & 0x07 ) );
  return b;
}

/**
 * @brief BlockObject::json
 * @return the json representation of a single key-value pair
 */
QByteArray  KeyValuePair::json()
{ QByteArray j = "{ \"";
  if ( !keyNames.contains( m_key ) )
    return "{<!-- unknown key -->}";
  j.append(keyNames[m_key]);
  j.append( "\": " );
  if ( m_value == nullptr )
    j.append( "0 <!-- null value -->" );
   else
    j.append( m_value->json() );
  j.append( " }" );
  return j;
}

QByteArray  KeyValueArray::bsonish()
{ QByteArray b = m_key; // TODO: type checking
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

QByteArray  KeyValueArray::json()
{ QByteArray j = "{ \"";
  if ( !keyNames.contains( m_key ) )
    return "{<!-- unknown key -->}";
  j.append(keyNames[m_key]);
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
  j.append( "] }" );
  return j;
}

