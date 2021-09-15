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
#ifndef BLOCKOB_H
#define BLOCKOB_H

#include <QObject>
#include <QMap>
#include <QPointer>
#include <QDataStream>
#include "dictionary.h"

#include <stdio.h>
#include <stdarg.h>
#include <obstack.h>
#include <gmp.h>

#define MAX_LENGTH 1073741824  // Nothing bigger than 1GB

/**
 * @brief The ValueBase class - base class for all value types found in block objects
 */
class ValueBase : public QObject
{
    Q_OBJECT
public:
          explicit  ValueBase(QObject *parent = nullptr) : QObject( parent ) {}
                   ~ValueBase() {}
static   ValueBase *newValue( RiceyInt k, QObject *parent = nullptr );
virtual     quint8  type()    = 0;
virtual BsonSerial  bsonish() = 0;
virtual JsonSerial  json()    = 0;
virtual     qint32  setBsonish( const BsonSerial & ) = 0;
         ValueBase *bsonishValueByKey( RiceyInt, const BsonSerial &, qint32 *l = nullptr, QObject *parent = nullptr );
         ValueBase *jsonValueByKey( RiceyInt, const QJsonValue &, QObject * );
virtual       bool  setJson( const JsonSerial & ) = 0;
        QByteArray  bsonishNull( qint8 );
};

/**
 * @brief The BlockValueNull class - an empty value with no defined type
 */
class BlockValueNull : public ValueBase
{ public:
    explicit  BlockValueNull( QObject *parent = nullptr ) : ValueBase( parent ) {}
             ~BlockValueNull() {}
      quint8  type()    { return RDT_NULL; }
  BsonSerial  bsonish() { return intToRice(RCD_ObTerm_o); }
  JsonSerial  json()    { QString s = "NULL"; return s.toUtf8(); }
      qint32  setBsonish( const BsonSerial &b ) { (void)b; return -1; }  // cannot set a value to a BlockValueNull
        bool  setJson   ( const JsonSerial &j ) { (void)j; return false; }
};

/**
 * @brief The KeyValueBase class - common base for objects that contain a key and one or more values
 */
class KeyValueBase : public ValueBase
{
    Q_OBJECT
public:
   explicit  KeyValueBase( RiceyInt k, QObject *parent = nullptr ) : ValueBase( parent )  { setKey( k ); }
            ~KeyValueBase() {}
     qint32  setKey( const RiceyCode &key );
       bool  setKey( RiceyInt k ) { return( setKey( intToRice( k ) ) > 0); }
 Utf8String  keyHex()  { return keyCode().toHex(); }
       char *keyHexd() { return keyHex().data(); }
  RiceyCode  keyCode() { return intToRice( m_key ); }
   RiceyInt  key()     { return m_key; }
     quint8  type()    { return (m_key & RDT_OBTYPEMASK); }

   RiceyInt  m_key; // Ricey code bsonish key
};

/**
 * @brief The KeyValuePair class - a single key and a single value (of any type)
 */
class KeyValuePair : public KeyValueBase
{
    Q_OBJECT
public:
    explicit  KeyValuePair( RiceyInt k, QObject *parent = nullptr ) : KeyValueBase( k, parent ) {}
             ~KeyValuePair() { if ( m_value ) delete( m_value ); }
        void  setValue( ValueBase &value ) { if ( value.type() != type() ) qWarning("kvp type mismatch"); else { if ( m_value ) delete( m_value ); m_value = &value; } }
 ValueBase &  valueRef() { return *m_value; }
  BsonSerial  bsonish();
  JsonSerial  json();
      qint32  setBsonish( const BsonSerial & );
        bool  setJson   ( const JsonSerial &j ) { (void)j; return true; } // TODO: fixme

public:
  QPointer<ValueBase> m_value; // Value of this KeyValuePair
};


extern BlockValueNull  glob_null;   // Returned when BlockValueObject::valueAt() calls an invalid index

/**
 * @brief The BlockValueObject class - zero or more KeyValue
 *   all with unique keys, types may be mixed.
 *
 * Container's Keys are type o/O
 */
class BlockValueObject : public ValueBase
{ public:
         explicit  BlockValueObject( QObject *parent = nullptr ) : ValueBase( parent ) {}
                  ~BlockValueObject() { clear(); }
           quint8  type()    { return RDT_OBJECT; }
       BsonSerial  bsonish();
       JsonSerial  json();
           qint32  setBsonish(  const BsonSerial &b );
             bool  setJson   (  const JsonSerial &j );
             void  clear()   { QList<RiceyInt> keys = m_obMap.keys(); foreach( RiceyInt k, keys ) { if ( m_obMap[k] != nullptr ) delete( m_obMap[k] ); m_obMap.remove(k); } }
           qint32  size()    { return m_obMap.size(); }
   QList<RiceyInt> keys()    { return m_obMap.keys(); }
             bool  contains  ( RiceyInt k ) { return m_obMap.contains( k ); }
             bool  insert    ( RiceyInt k, ValueBase *v );
        ValueBase *value     ( RiceyInt k ) { if ( !contains(k) ) return &glob_null; return m_obMap.value(k); }

QMap<RiceyInt, QPointer<ValueBase> > m_obMap;
};

/**
 * @brief The BlockValueInt64 class - single signed 64 bit integer
 *
 * Container's Keys are type i/I
 */
class BlockValueInt64 : public ValueBase
{ public:
    explicit  BlockValueInt64( QObject *parent = nullptr ) : ValueBase( parent ) { setValue( 0 ); }
    explicit  BlockValueInt64( qint64 v, QObject *parent = nullptr ) : ValueBase( parent ) { setValue( v ); }
             ~BlockValueInt64() {}
      quint8  type()    { return RDT_INT64; }
  BsonSerial  bsonish() { BsonSerial b; QDataStream s(&b,QIODevice::WriteOnly); s.setByteOrder(QDataStream::LittleEndian); s << m_value; return b; }
  JsonSerial  json()    { return QString::number( m_value ).toUtf8(); }
      qint32  setBsonish( const BsonSerial &b );
        bool  setJson   ( const JsonSerial &j );
      qint64  value()   { return m_value; }
        void  setValue( qint64 v ) { m_value = v; }

      qint64  m_value;
};

/**
 * @brief The BlockValueInt32 class - single signed 32 bit integer
 *
 * Container's Keys are type l/L
 */
class BlockValueInt32 : public ValueBase
{ public:
    explicit  BlockValueInt32( QObject *parent = nullptr ) : ValueBase( parent ) { setValue(0); }
    explicit  BlockValueInt32( qint32 v, QObject *parent = nullptr ) : ValueBase( parent ) { setValue(v); }
             ~BlockValueInt32() {}
      quint8  type()    { return RDT_INT32; }
  BsonSerial  bsonish() { BsonSerial b; QDataStream s(&b,QIODevice::WriteOnly); s.setByteOrder(QDataStream::LittleEndian); s << m_value; return b; }
  JsonSerial  json()    { return QString::number( m_value ).toUtf8(); }
      qint32  setBsonish( const BsonSerial &b );
        bool  setJson   ( const JsonSerial &j );
      qint32  value()   { return m_value; }
        void  setValue( qint32 v ) { m_value = v; }

      qint32  m_value;
};

/**
 * @brief The BlockValueRiceyCode class - a single Ricey Code, 1 to 7 octets in length
 *
 * Container's Keys are type y/Y
 */
class BlockValueRiceyCode : public ValueBase
{ public:
    explicit  BlockValueRiceyCode( QObject *parent = nullptr ) : ValueBase( parent ) {}
    explicit  BlockValueRiceyCode( RiceyCode v, QObject *parent = nullptr ) : ValueBase( parent ) { setValue(v); }
    explicit  BlockValueRiceyCode( RiceyInt r , QObject *parent = nullptr ) : ValueBase( parent ) { setValue(r); }
             ~BlockValueRiceyCode() {}
      quint8  type()     { return RDT_RCODE; }
  BsonSerial  bsonish()  { return m_value; }
  JsonSerial  json()     { return "\""+m_value.toHex()+"\""; }
      qint32  setBsonish ( const BsonSerial & );
        bool  setJson    ( const JsonSerial & );
   RiceyCode  value()    { return m_value; }
    RiceyInt  valueInt() { return riceToInt( m_value ); }
        void  setValue( RiceyCode v ) { if ( !validRicey( v ) ) qWarning("invalid ricey code"); else m_value = v; }
        void  setValue( RiceyInt r ) { setValue( intToRice( r ) ); }

   RiceyCode  m_value;
};

/**
 * @brief The BlockValueString class - a UTF-8 encoded string
 *
 * Container's Keys are type s/S
 */
class BlockValueString : public ValueBase
{ public:
    explicit  BlockValueString( QObject *parent = nullptr ) : ValueBase( parent ) {}
    explicit  BlockValueString( Utf8String v, QObject *parent = nullptr ) : ValueBase( parent ) { setValue(v); }
             ~BlockValueString() {}
      quint8  type()    { return RDT_STRING; }
  BsonSerial  bsonish();
  JsonSerial  json();
      qint32  setBsonish( const BsonSerial &b );
        bool  setJson   ( const BsonSerial &j );
  Utf8String  value()   { return m_value; }
        void  setValue( Utf8String v ) { if ( m_value.size() > MAX_LENGTH ) qWarning("MAX_LENGTH exceeded"); else m_value = v; }

  Utf8String  m_value;  // Strings are encoded as UTF8 in a QByteArray
};

/**
 * @brief The BlockValueByteArray class - an array of octets
 *
 * Container's Keys are type b/B
 */
class BlockValueByteArray : public ValueBase
{ public:
    explicit  BlockValueByteArray( QObject *parent = nullptr ) : ValueBase( parent ) {}
    explicit  BlockValueByteArray( QByteArray v, QObject *parent = nullptr ) : ValueBase( parent ) { setValue(v); }
             ~BlockValueByteArray() {}
      quint8  type()    { return RDT_BYTEARRAY; }
  BsonSerial  bsonish() { BsonSerial b; QDataStream s(&b,QIODevice::WriteOnly); s.setByteOrder(QDataStream::LittleEndian); s << (qint32)m_value.size(); b.append( m_value ); return b; }
  JsonSerial  json()    { return "\""+m_value.toHex()+"\""; }
      qint32  setBsonish( const BsonSerial & );
        bool  setJson   ( const JsonSerial & );
  QByteArray  value()   { return m_value; }
        void  setValue( QByteArray v ) { if ( m_value.size() > MAX_LENGTH ) qWarning("MAX_LENGTH exceeded"); else m_value = v; }

  QByteArray  m_value;
};

/**
 * @brief The BlockValueMPZ class - an arbitrary precision integer
 *
 * Container's Keys are type n/N
 */
class BlockValueMPZ : public ValueBase
{ public:
    explicit  BlockValueMPZ( QObject *parent = nullptr ) : ValueBase( parent ) {}
    explicit  BlockValueMPZ( MP_INT v, QObject *parent = nullptr ) : ValueBase( parent ) { setValue(v); }
             ~BlockValueMPZ() {}
      quint8  type()    { return RDT_MPZ; }
  BsonSerial  bsonish() { return QByteArray(); } // TODO: fixme
  JsonSerial  json()    { return QByteArray(); } // TODO: fixme
      qint32  setBsonish( const BsonSerial &b ) { (void)b; return -1; } // TODO: fixme
        bool  setJson   ( const JsonSerial &j ) { (void)j; return false; } // TODO: fixme
      MP_INT  value()   { return m_value; }
        void  setValue( MP_INT v ) { m_value = v; }

      MP_INT  m_value;
};

/**
 * @brief The BlockValueMPQ class - a fraction of two arbitrary precision integers
 *
 * Container's Keys are type r/R
 */
class BlockValueMPQ : public ValueBase
{ public:
    explicit  BlockValueMPQ( QObject *parent = nullptr ) : ValueBase( parent ) {}
    explicit  BlockValueMPQ( MP_RAT v, QObject *parent = nullptr ) : ValueBase( parent ) { setValue(v); }
             ~BlockValueMPQ() {}
      quint8  type()    { return RDT_MPQ; }
  BsonSerial  bsonish() { return QByteArray(); } // TODO: fixme
  JsonSerial  json()    { return QByteArray(); } // TODO: fixme
      qint32  setBsonish( const BsonSerial &b ) { (void)b; return -1; } // TODO: fixme
        bool  setJson   ( const JsonSerial &j ) { (void)j; return false; } // TODO: fixme
      MP_RAT  value()   { return m_value; }
        void  setValue( MP_RAT v ) { m_value = v; }

      MP_RAT  m_value;
};

/**
 * @brief The KeyValueArray class - zero or more values of the same type stored under a single key
 *   order of items in the array is important, conserved, and determined by the creator of the array.
 *   Equivalent to a JsonObject with a single key: array value inside.
 */
class KeyValueArray : public KeyValueBase
{
    Q_OBJECT
public:
    explicit  KeyValueArray( RiceyInt k, QObject *parent = nullptr ) : KeyValueBase( k, parent ) {}
             ~KeyValueArray() { clear(); }
        void  clear() { while ( m_values.size() > 0 ) { if ( m_values.last() != nullptr ) { delete( m_values.last() ); } m_values.removeLast(); } }
      qint32  size()  { return m_values.size(); }
        bool  append( ValueBase *value ) { if ( value == nullptr )                           { qWarning( "will not append null" );                               return false; }
                                           if ( value->type() != ( type() & RDT_TYPEMASK ) ) { qWarning( "type mismatch %d %d",(int)value->type(),(int)type() ); return false; }
                                           m_values.append( value ); return true; }
        bool  append( qint64 v )     { if ( type() == RDT_INT64_ARRAY     ) { return append( new BlockValueInt64    (v, this) ); } qWarning( "type mismatch I %d",(int)type() ); return false; }
        bool  append( qint32 v )     { if ( type() == RDT_INT32_ARRAY     ) { return append( new BlockValueInt32    (v, this) ); } qWarning( "type mismatch L %d",(int)type() ); return false; }
        bool  append( MP_INT v )     { if ( type() == RDT_MPZ_ARRAY       ) { return append( new BlockValueMPZ      (v, this) ); } qWarning( "type mismatch N %d",(int)type() ); return false; }
        bool  append( MP_RAT v )     { if ( type() == RDT_MPQ_ARRAY       ) { return append( new BlockValueMPQ      (v, this) ); } qWarning( "type mismatch R %d",(int)type() ); return false; }
        bool  append( RiceyInt v )   { if ( type() == RDT_RCODE_ARRAY     ) { return append( new BlockValueRiceyCode(v, this) ); } qWarning( "type mismatch Y %d",(int)type() ); return false; }
        bool  append( QByteArray v ) { if ( type() == RDT_RCODE_ARRAY     ) { return append( new BlockValueRiceyCode(v, this) ); }
                                       if ( type() == RDT_STRING_ARRAY    ) { return append( new BlockValueString   (v, this) ); }
                                       if ( type() == RDT_BYTEARRAY_ARRAY ) { return append( new BlockValueByteArray(v, this) ); } qWarning( "type mismatch BA %d",(int)type() ); return false; }
        // TODO: append an object...
   ValueBase *at( qint32 n ) { if (( n >= 0 ) && ( n < size() )) return m_values.at(n); else return nullptr; }
  BsonSerial  bsonish();
  JsonSerial  json();
      qint32  setBsonish( const BsonSerial &b );
        bool  setJson   ( const JsonSerial &j );

public:
  QVarLengthArray<ValueBase *> m_values; // Values in this array
};

class BlockArrayInt64 : public KeyValueArray
{ public:
    explicit  BlockArrayInt64( QObject *parent = nullptr ) : KeyValueArray( RCD_int64Array_I, parent ) {}
    explicit  BlockArrayInt64( RiceyInt k, QObject *parent = nullptr ) : KeyValueArray( k, parent ) {} // TODO: key type checking
             ~BlockArrayInt64() {}
      qint64  at( qint32 n ) { if (( n >= 0 ) && ( n < size() )) return ((BlockValueInt64 *)m_values[n])->value(); qWarning( "array index %d out of bounds %d",n,size() ); return 0; }
        void  set( const QList<qint64> &vl ) { clear(); foreach( qint64 v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
        bool  operator==(const QList<qint64>& l) { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=at(i)) return false; return true; }
};

class BlockArrayInt32 : public KeyValueArray
{ public:
    explicit  BlockArrayInt32( QObject *parent = nullptr ) : KeyValueArray( RCD_int32Array_L, parent ) {}
    explicit  BlockArrayInt32( RiceyInt k, QObject *parent = nullptr ) : KeyValueArray( k, parent ) {} // TODO: key type checking
             ~BlockArrayInt32() {}
      qint64  at( qint32 n ) { if (( n >= 0 ) && ( n < size() )) return ((BlockValueInt32 *)m_values[n])->value(); qWarning( "array index %d out of bounds %d",n,size() ); return 0; }
        void  set( const QList<qint32> &vl ) { clear(); foreach( qint32 v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
        bool  operator==(const QList<qint32>& l) { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=at(i)) return false; return true; }
};

class BlockArrayRicey : public KeyValueArray
{ public:
    explicit  BlockArrayRicey( QObject *parent = nullptr ) : KeyValueArray( RCD_riceyArray_Y, parent ) {}
    explicit  BlockArrayRicey( RiceyInt k, QObject *parent = nullptr ) : KeyValueArray( k, parent ) {} // TODO: key type checking
             ~BlockArrayRicey() {}
   RiceyCode  at( qint32 n ) { if (( n >= 0 ) && ( n < size() )) return ((BlockValueRiceyCode *)m_values[n])->value(); qWarning( "array index %d out of bounds %d",n,size() ); return RiceyCode(); }
    RiceyInt  intAt( qint32 n ) { if (( n >= 0 ) && ( n < size() )) return riceToInt( at(n) );                         qWarning( "array index %d out of bounds %d",n,size() ); return -1; }
        void  set( const QList<RiceyCode> &vl ) { clear(); foreach( RiceyCode v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
        void  set( const QList<RiceyInt>  &vl ) { clear(); foreach( RiceyInt  v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
        bool  operator==(const QList<RiceyCode>& l) { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=   at(i)) return false; return true; }
        bool  operator==(const QList<RiceyInt> & l) { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=intAt(i)) return false; return true; }
};

class BlockArrayString : public KeyValueArray
{ public:
    explicit  BlockArrayString( QObject *parent = nullptr ) : KeyValueArray( RCD_stringArray_S, parent ) {}
    explicit  BlockArrayString( RiceyInt k, QObject *parent = nullptr ) : KeyValueArray( k, parent ) {} // TODO: key type checking
             ~BlockArrayString() {}
  Utf8String  at( qint32 n ) { if (( n >= 0 ) && ( n < size() )) return ((BlockValueString *)m_values[n])->value(); qWarning( "array index %d out of bounds %d",n,size() ); return Utf8String(); }
        void  set( const QList<Utf8String> &vl ) { clear(); foreach( Utf8String v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
        bool  operator==(const QList<Utf8String>& l) { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=at(i)) return false; return true; }
};

class BlockArrayByteArray : public KeyValueArray
{ public:
    explicit  BlockArrayByteArray( QObject *parent = nullptr ) : KeyValueArray( RCD_byteArrayArray_B, parent ) {}
    explicit  BlockArrayByteArray( RiceyInt k, QObject *parent = nullptr ) : KeyValueArray( k, parent ) {} // TODO: key type checking
             ~BlockArrayByteArray() {}
  Utf8String  at( qint32 n ) { if (( n >= 0 ) && ( n < size() )) return ((BlockValueByteArray *)m_values[n])->value(); qWarning( "array index %d out of bounds %d",n,size() ); return QByteArray(); }
        void  set( const QList<QByteArray> &vl ) { clear(); foreach( QByteArray v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
        bool  operator==(const QList<QByteArray>& l) { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=at(i)) return false; return true; }
};

#endif // BLOCKOB_H
