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
static   ValueBase *newValue( RiceyInt k, QObject *parent = nullptr, ValueBase *vtc = nullptr );
virtual     quint8  type()    const = 0;
virtual BsonSerial  bsonish() const = 0;
virtual JsonSerial  json()    const = 0;
virtual     qint32  setBsonish( const BsonSerial & ) = 0;
         ValueBase *bsonishValueByKey( RiceyInt, const BsonSerial &, qint32 *l = nullptr, QObject *parent = nullptr );
         ValueBase *jsonValueByKey( RiceyInt, const QJsonValue &, QObject * );
virtual       bool  setJson( const JsonSerial & ) = 0;
virtual Utf8String  valueString() const = 0;
        QByteArray  bsonishNull( qint8 ) const;
};

/**
 * @brief The BlockValueNull class - an empty value with no defined type
 */
class BlockValueNull : public ValueBase
{ public:
    explicit  BlockValueNull( QObject *parent = nullptr ) : ValueBase( parent ) {}
             ~BlockValueNull() {}
      quint8  type()    const { return RDT_NULL; }
  BsonSerial  bsonish() const { return intToRice(RCD_ObTerm_o); }
  JsonSerial  json()    const { QString s = "NULL"; return s.toUtf8(); }
      qint32  setBsonish( const BsonSerial &b ) { (void)b; return -1; }  // cannot set a value to a BlockValueNull
        bool  setJson   ( const JsonSerial &j ) { (void)j; return false; }
  Utf8String  valueString() const { return "bvn"; }
};

extern BlockValueNull  glob_null;   // Returned when BlockValueObject::valueAt() calls an invalid index
typedef QMap< RiceyInt, ValueBase *> BlockObjectMap;
typedef QVarLengthArray<ValueBase *> ValueArray;

/**
 * @brief The KeyValueBase class - common base for objects that contain a key and one or more values
 */
class KeyValueBase : public ValueBase
{
    Q_OBJECT
public:
   explicit  KeyValueBase( const RiceyCode &r, QObject *parent = nullptr ) : ValueBase( parent )  { setKey( r ); }
   explicit  KeyValueBase( const RiceyInt  &k, QObject *parent = nullptr ) : ValueBase( parent )  { setKey( k ); }
            ~KeyValueBase() {}
     qint32  setKey( const RiceyCode &r );
       bool  setKey( const RiceyInt &k ) { return( setKey( intToRice( k ) ) > 0 ); }
 Utf8String  keyHex()  const { return keyCode().toHex(); }
       char *keyHexd() const { return keyHex().data();   }
  RiceyCode  keyCode() const { return intToRice( m_key ); }
   RiceyInt  key()     const { return m_key; }
     quint8  type()    const { return (m_key & RDT_OBTYPEMASK); }
 Utf8String  valueString() const { return "kvb"; }

   RiceyInt  m_key; // Ricey code bsonish key
};

/**
 * @brief The KeyValuePair class - a single key and a single value (of any type)
 */
class KeyValuePair : public KeyValueBase
{
    Q_OBJECT
public:
    explicit  KeyValuePair( const RiceyInt &k, QObject *parent = nullptr ) : KeyValueBase( k, parent ) {}
             ~KeyValuePair() { if ( m_value ) m_value->deleteLater(); }
        void  set( ValueBase &value ) { if ( value.type() != type() ) qWarning("kvp type mismatch"); else { if ( m_value ) m_value->deleteLater(); m_value = &value; } }
 ValueBase &  valueRef() const { return *m_value; }
  BsonSerial  bsonish()  const;
  JsonSerial  json()     const;
      qint32  setBsonish( const BsonSerial & );
        bool  setJson   ( const JsonSerial &j ) { (void)j; return true; } // TODO: fixme
  Utf8String  valueString() const { return "kvp"; }

public:
  QPointer<ValueBase> m_value; // Value of this KeyValuePair
};

/**
 * @brief The BlockValueInt64 class - single signed 64 bit integer
 *
 * Container's Keys are type i/I
 */
class BlockValueInt64 : public ValueBase
{ public:
    explicit  BlockValueInt64( QObject *parent = nullptr ) : ValueBase( parent ) { set( 0 ); }
              BlockValueInt64( const qint64 &v, QObject *parent = nullptr ) : ValueBase( parent ) { set( v ); }
             ~BlockValueInt64() {}
      quint8  type()    const { return RDT_INT64; }
  BsonSerial  bsonish() const { BsonSerial b; QDataStream s(&b,QIODevice::WriteOnly); s.setByteOrder(QDataStream::LittleEndian); s << m_value; return b; }
  JsonSerial  json()    const { return QString::number( m_value ).toUtf8(); }
      qint32  setBsonish( const BsonSerial &b );
        bool  setJson   ( const JsonSerial &j );
      qint64  value()   const { return m_value; }
        void  set( const qint64 &v ) { m_value = v; }
        bool  operator==( const qint64 &v )           const { return v         == value(); }
        bool  operator==( const BlockValueInt64 &v  ) const { return v.value() == value(); }
  Utf8String  valueString() const { return QString::number( m_value ).toUtf8(); }

      qint64  m_value;
};

/**
 * @brief The BlockValueInt32 class - single signed 32 bit integer
 *
 * Container's Keys are type l/L
 */
class BlockValueInt32 : public ValueBase
{ public:
    explicit  BlockValueInt32( QObject *parent = nullptr ) : ValueBase( parent ) { set(0); }
              BlockValueInt32( const qint32 &v, QObject *parent = nullptr ) : ValueBase( parent ) { set(v); }
             ~BlockValueInt32() {}
      quint8  type()    const { return RDT_INT32; }
  BsonSerial  bsonish() const { BsonSerial b; QDataStream s(&b,QIODevice::WriteOnly); s.setByteOrder(QDataStream::LittleEndian); s << m_value; return b; }
  JsonSerial  json()    const { return QString::number( m_value ).toUtf8(); }
      qint32  setBsonish( const BsonSerial &b );
        bool  setJson   ( const JsonSerial &j );
      qint32  value()   const { return m_value; }
        void  set( const qint32 &v )  { m_value = v; }
        bool  operator==( const qint32 &v )           const { return v         == value(); }
        bool  operator==( const BlockValueInt32 &v  ) const { return v.value() == value(); }
  Utf8String  valueString() const { return QString::number( m_value ).toUtf8(); }

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
              BlockValueRiceyCode( const RiceyCode &v, QObject *parent = nullptr ) : ValueBase( parent ) { set(v); }
              BlockValueRiceyCode( const RiceyInt &r , QObject *parent = nullptr ) : ValueBase( parent ) { set(r); }
             ~BlockValueRiceyCode() {}
      quint8  type()    const { return RDT_RCODE; }
  BsonSerial  bsonish() const { return m_value; }
  JsonSerial  json()    const { return "\""+m_value.toHex()+"\""; }
      qint32  setBsonish ( const BsonSerial & );
        bool  setJson    ( const JsonSerial & );
   RiceyCode  value()    const { return m_value; }
    RiceyInt  valueInt() const { return riceToInt( m_value ); }
        void  set( const RiceyCode &v ) { if ( !validRicey( v ) ) qWarning("invalid ricey code"); else m_value = v; } // to detach or not to detach...
        void  set( const RiceyInt &r )  { set( intToRice( r ) ); }
        bool  operator==( const RiceyInt  &v ) const { return v == valueInt(); }
        bool  operator==( const RiceyCode &v ) const { return v == value(); }
        bool  operator==( const BlockValueRiceyCode &v  ) const { return v.value() == value(); }
  Utf8String  valueString() const { return m_value.toHex(); }

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
              BlockValueString( const Utf8String &v, QObject *parent = nullptr ) : ValueBase( parent ) { set(v); }
             ~BlockValueString() {}
      quint8  type()    const { return RDT_STRING; }
  BsonSerial  bsonish() const;
  JsonSerial  json()    const;
      qint32  setBsonish( const BsonSerial &b );
        bool  setJson   ( const BsonSerial &j );
        void  set( const Utf8String &v ) { if ( m_value.size() > MAX_LENGTH ) qWarning("MAX_LENGTH exceeded"); else m_value = v; }
  Utf8String  value() const { return m_value; }
        bool  operator==( const Utf8String &v ) const { return v == value(); }
        bool  operator==( const BlockValueString &v  ) const { return v.value() == value(); }
  Utf8String  valueString() const { return m_value; }

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
              BlockValueByteArray( const QByteArray &v, QObject *parent = nullptr ) : ValueBase( parent ) { set(v); }
             ~BlockValueByteArray() {}
      quint8  type()    const { return RDT_BYTEARRAY; }
  BsonSerial  bsonish() const { BsonSerial b; QDataStream s(&b,QIODevice::WriteOnly); s.setByteOrder(QDataStream::LittleEndian); s << (qint32)m_value.size(); b.append( m_value ); return b; }
  JsonSerial  json()    const { return "\""+m_value.toHex()+"\""; }
      qint32  setBsonish( const BsonSerial & );
        bool  setJson   ( const JsonSerial & );
  QByteArray  value()   const { return m_value; }
        void  set( const QByteArray &v ) { if ( m_value.size() > MAX_LENGTH ) qWarning("MAX_LENGTH exceeded"); else m_value = v; }
        bool  operator==( const QByteArray &v ) const { return v == value(); }
        bool  operator==( const BlockValueByteArray &v  ) const { return v.value() == value(); }
  Utf8String  valueString() const { return m_value.toHex(); }

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
              BlockValueMPZ( const MP_INT &v, QObject *parent = nullptr ) : ValueBase( parent ) { set(v); }
             ~BlockValueMPZ() {}
      quint8  type()    const { return RDT_MPZ; }
  BsonSerial  bsonish() const { return QByteArray(); } // TODO: fixme
  JsonSerial  json()    const { return QByteArray(); } // TODO: fixme
      qint32  setBsonish( const BsonSerial &b ) { (void)b; return -1; } // TODO: fixme
        bool  setJson   ( const JsonSerial &j ) { (void)j; return false; } // TODO: fixme
      MP_INT  value()   const { return m_value; }
        void  set( const MP_INT &v ) { m_value = v; }
        bool  operator==( const MP_INT &v ) const { (void)v; return false; } // v == value(); TODO: fixme
        bool  operator==( const BlockValueMPZ &v  ) const { (void)v;  return false; } // v.  value() == value(); }
  Utf8String  valueString() const { return "MPZ"; }

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
              BlockValueMPQ( const MP_RAT &v, QObject *parent = nullptr ) : ValueBase( parent ) { set(v); }
             ~BlockValueMPQ() {}
      quint8  type()    const { return RDT_MPQ; }
  BsonSerial  bsonish() const { return QByteArray(); } // TODO: fixme
  JsonSerial  json()    const { return QByteArray(); } // TODO: fixme
      qint32  setBsonish( const BsonSerial &b ) { (void)b; return -1; } // TODO: fixme
        bool  setJson   ( const JsonSerial &j ) { (void)j; return false; } // TODO: fixme
      MP_RAT  value()   const { return m_value; }
        void  set( const MP_RAT &v ) { m_value = v; }
        bool  operator==( const MP_RAT &v ) const { (void)v; return false; } // v == value(); }  TODO: fixme
        bool  operator==( const BlockValueMPQ &v  ) const { (void)v;  return false; } // v.  value() == value(); }
  Utf8String  valueString() const { return "MPQ"; }

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
    explicit  KeyValueArray( RiceyInt  k, QObject *parent = nullptr ) : KeyValueBase(            k, parent ) {}
    explicit  KeyValueArray( RiceyCode r, QObject *parent = nullptr ) : KeyValueBase( riceToInt(r), parent ) {}
             ~KeyValueArray() { clear(); }
        void  clear() { while ( m_values.size() > 0 ) { if ( m_values.last() != nullptr ) { m_values.last()->deleteLater(); } m_values.removeLast(); } }
      qint32  size() const { return m_values.size(); }
        bool  append( ValueBase *value ) { if ( value == nullptr )                           { qWarning( "will not append null" );                               return false; }
                                           if ( value->type() != ( type() & RDT_TYPEMASK ) ) { qWarning( "type mismatch %d %d",(int)value->type(),(int)type() ); return false; }
                                           m_values.append( value ); return true; }
        bool  append( qint64 v )   { if ( type() == RDT_INT64_ARRAY     ) { return append( new BlockValueInt64    (v, this) ); } qWarning( "type mismatch I %d",(int)type() ); return false; }
        bool  append( qint32 v )   { if ( type() == RDT_INT32_ARRAY     ) { return append( new BlockValueInt32    (v, this) ); } qWarning( "type mismatch L %d",(int)type() ); return false; }
        bool  append( MP_INT v )   { if ( type() == RDT_MPZ_ARRAY       ) { return append( new BlockValueMPZ      (v, this) ); } qWarning( "type mismatch N %d",(int)type() ); return false; }
        bool  append( MP_RAT v )   { if ( type() == RDT_MPQ_ARRAY       ) { return append( new BlockValueMPQ      (v, this) ); } qWarning( "type mismatch R %d",(int)type() ); return false; }
        bool  append( RiceyInt v ) { if ( type() == RDT_RCODE_ARRAY     ) { return append( new BlockValueRiceyCode(v, this) ); } qWarning( "type mismatch Y %d",(int)type() ); return false; }
        bool  append( const QByteArray &v )
                                   { if ( type() == RDT_RCODE_ARRAY     ) { return append( new BlockValueRiceyCode(v, this) ); }
                                     if ( type() == RDT_STRING_ARRAY    ) { return append( new BlockValueString   (v, this) ); }
                                     if ( type() == RDT_BYTEARRAY_ARRAY ) { return append( new BlockValueByteArray(v, this) ); } qWarning( "type mismatch BA %d",(int)type() ); return false; }
        bool  append( const BlockObjectMap &v );
        void  append( const ValueArray &v ) { foreach ( ValueBase *vbp, v ) m_values.append( newValue( key() & RDT_TYPEMASK, this, vbp ) ); };
   ValueBase *at( qint32 n ) const { if (( n >= 0 ) && ( n < size() )) return m_values.at(n); else return nullptr; }
  ValueArray  value() { return m_values; }
        void  set( const ValueArray &v ) { clear(); append( v ); };
  BsonSerial  bsonish() const;
  JsonSerial  json()    const;
      qint32  setBsonish( const BsonSerial &b );
        bool  setJson   ( const JsonSerial &j );
  Utf8String  valueString() const { return "array"; }

public:
  ValueArray  m_values; // Values in this array
};

/**
 * @brief The BlockValueObject class - zero or more KeyValue
 *   all with unique keys, types may be mixed.
 *
 * Container's Keys are type o/O
 */
class BlockValueObject : public ValueBase
{ public:
         explicit  BlockValueObject( QObject *parent = nullptr ) : ValueBase( parent ) {}
                   BlockValueObject( const BlockObjectMap &v, QObject *parent = nullptr ) : ValueBase( parent ) { set(v); }
                   BlockValueObject( const BsonSerial &b, QObject *parent = nullptr ) : ValueBase( parent ) { setBsonish(b); }
                  ~BlockValueObject() { clear(); }
           quint8  type()    const { return RDT_OBJECT; }
       BsonSerial  bsonish() const;
       JsonSerial  json()    const;
           qint32  setBsonish(  const BsonSerial &b );
             bool  setJson   (  const JsonSerial &j );
             void  clear()   { QList<RiceyInt> keys = m_obMap.keys(); foreach( RiceyInt k, keys ) { if ( m_obMap[k] != nullptr ) m_obMap[k]->deleteLater(); m_obMap.remove(k); } }
           qint32  size() const { return m_obMap.size(); }
   QList<RiceyInt> keys() const { return m_obMap.keys(); }
             bool  contains( RiceyInt k ) const { return m_obMap.contains( k ); }
             bool  insert    ( RiceyInt k, ValueBase *v );
           qint32  insert    ( const BlockObjectMap &vl );
             bool  insert    ( RiceyInt k, const BlockObjectMap &v ) { BlockValueObject    *vp = new BlockValueObject   ( v, this ); bool ok = insert( k, vp ); if ( !ok ) vp->deleteLater(); return ok; }             bool  insert    ( RiceyInt k,       qint64  v ) { BlockValueInt64     *vp = new BlockValueInt64    ( v, this ); bool ok = insert( k, vp ); if ( !ok ) vp->deleteLater(); return ok; }
             bool  insert    ( RiceyInt k,               qint32  v ) { BlockValueInt32     *vp = new BlockValueInt32    ( v, this ); bool ok = insert( k, vp ); if ( !ok ) vp->deleteLater(); return ok; }
             bool  insert    ( RiceyInt k,         const MP_INT &v ) { BlockValueMPZ       *vp = new BlockValueMPZ      ( v, this ); bool ok = insert( k, vp ); if ( !ok ) vp->deleteLater(); return ok; }
             bool  insert    ( RiceyInt k,         const MP_RAT &v ) { BlockValueMPQ       *vp = new BlockValueMPQ      ( v, this ); bool ok = insert( k, vp ); if ( !ok ) vp->deleteLater(); return ok; }
             bool  insert    ( RiceyInt k,             RiceyInt  v ) { BlockValueRiceyCode *vp = new BlockValueRiceyCode( v, this ); bool ok = insert( k, vp ); if ( !ok ) vp->deleteLater(); return ok; }
             bool  insert    ( RiceyInt k, const QByteArray &v ) { BlockValueString    *vs;
                                                                   BlockValueByteArray *vb;
                                                                   BlockValueRiceyCode *vr; bool ok;
                                                                   switch( k & RDT_TYPEMASK )
                                                                     { case RDT_STRING:    vs = new BlockValueString   ( v, this ); ok = insert( k, vs ); if ( !ok ) vs->deleteLater(); return ok;
                                                                       case RDT_BYTEARRAY: vb = new BlockValueByteArray( v, this ); ok = insert( k, vb ); if ( !ok ) vb->deleteLater(); return ok;
                                                                       case RDT_RCODE:     vr = new BlockValueRiceyCode( v, this ); ok = insert( k, vr ); if ( !ok ) vr->deleteLater(); return ok;
                                                                       default: return false;
                                                                 }   }
             // TODO: insert array types
        ValueBase *value( const RiceyInt  &k ) const { if ( !contains(k) ) return &glob_null; return m_obMap.value(k); }
        ValueBase *value( const RiceyCode &r ) const { return value( riceToInt( r ) ); }
   BlockObjectMap  value() const { return m_obMap; }
           qint32  set       ( const BlockObjectMap  &vl ) { clear(); return insert( vl ); }
             bool  operator==( const BlockObjectMap   &  ) const;
             bool  operator==( const BlockValueObject &v ) const { return ( v == m_obMap ); }
       Utf8String  valueString() const { return "obj"; }

   BlockObjectMap  m_obMap;
};

class BlockArrayInt64 : public KeyValueArray
{ public:
    explicit  BlockArrayInt64( QObject *parent = nullptr ) : KeyValueArray( RCD_int64Array_I, parent ) {}
    explicit  BlockArrayInt64( RiceyInt k, QObject *parent = nullptr ) : KeyValueArray( k, parent ) {} // TODO: key type checking
              BlockArrayInt64( RiceyInt k, const QList<qint64> &v, QObject *parent = nullptr ) : KeyValueArray( k, parent ) { set(v); } // TODO: key type checking
             ~BlockArrayInt64() {}
      qint64  at( qint32 n ) const { if (( n >= 0 ) && ( n < size() )) return ((BlockValueInt64 *)m_values[n])->value(); qWarning( "array index %d out of bounds %d",n,size() ); return 0; }
QList<qint64> value() const { QList<qint64> vl; qint32 n = 0; while ( n < size() ) vl.append(at(n++)); return vl; }
        void  set( const QList<qint64> &vl ) { clear(); foreach( qint64 v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
        bool  operator==(const QList<qint64>& l) const { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=at(i)) return false; return true; }
};

class BlockArrayInt32 : public KeyValueArray
{ public:
    explicit  BlockArrayInt32( QObject *parent = nullptr ) : KeyValueArray( RCD_int32Array_L, parent ) {}
    explicit  BlockArrayInt32( RiceyInt k, QObject *parent = nullptr ) : KeyValueArray( k, parent ) {} // TODO: key type checking
              BlockArrayInt32( RiceyInt k, const QList<qint32> &v, QObject *parent = nullptr ) : KeyValueArray( k, parent ) { set(v); } // TODO: key type checking
             ~BlockArrayInt32() {}
      qint64  at( qint32 n ) const { if (( n >= 0 ) && ( n < size() )) return ((BlockValueInt32 *)m_values[n])->value(); qWarning( "array index %d out of bounds %d",n,size() ); return 0; }
QList<qint32> value() const { QList<qint32> vl; qint32 n = 0; while ( n < size() ) vl.append(at(n++)); return vl; }
        void  set( const QList<qint32> &vl ) { clear(); foreach( qint32 v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
        bool  operator==(const QList<qint32>& l) const { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=at(i)) return false; return true; }
};

class BlockArrayRicey : public KeyValueArray
{ public:
       explicit  BlockArrayRicey( QObject *parent = nullptr ) : KeyValueArray( RCD_riceyArray_Y, parent ) {}
       explicit  BlockArrayRicey( RiceyInt k, QObject *parent = nullptr ) : KeyValueArray( k, parent ) {} // TODO: key type checking
                 BlockArrayRicey( RiceyInt k, const QList<RiceyInt > &v, QObject *parent = nullptr ) : KeyValueArray( k, parent ) { set(v); } // TODO: key type checking
                 BlockArrayRicey( RiceyInt k, const QList<RiceyCode> &v, QObject *parent = nullptr ) : KeyValueArray( k, parent ) { set(v); } // TODO: key type checking
                ~BlockArrayRicey() {}
      RiceyCode  at( qint32 n )    const { if (( n >= 0 ) && ( n < size() )) return ((BlockValueRiceyCode *)m_values[n])->value(); qWarning( "array index %d out of bounds %d",n,size() ); return RiceyCode(); }
       RiceyInt  intAt( qint32 n ) const { if (( n >= 0 ) && ( n < size() )) return riceToInt( at(n) );                            qWarning( "array index %d out of bounds %d",n,size() ); return -1; }
QList<RiceyCode> value()    const { QList<RiceyCode> vl; qint32 n = 0; while ( n < size() ) vl.append(   at(n++)); return vl; }
QList<RiceyInt > valueInt() const { QList<RiceyInt > vl; qint32 n = 0; while ( n < size() ) vl.append(intAt(n++)); return vl; }
           void  set( const QList<RiceyCode> &vl ) { clear(); foreach( RiceyCode v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
           void  set( const QList<RiceyInt>  &vl ) { clear(); foreach( RiceyInt  v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
           bool  operator==(const QList<RiceyCode>& l) const { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=   at(i)) return false; return true; }
           bool  operator==(const QList<RiceyInt> & l) const { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=intAt(i)) return false; return true; }
};

class BlockArrayString : public KeyValueArray
{ public:
        explicit  BlockArrayString( QObject *parent = nullptr ) : KeyValueArray( RCD_stringArray_S, parent ) {}
        explicit  BlockArrayString( RiceyInt k, QObject *parent = nullptr ) : KeyValueArray( k, parent ) {} // TODO: key type checking
                  BlockArrayString( RiceyInt k, const QList<Utf8String> &v, QObject *parent = nullptr ) : KeyValueArray( k, parent ) { set(v); } // TODO: key type checking
                 ~BlockArrayString() {}
      Utf8String  at( qint32 n ) const { if (( n >= 0 ) && ( n < size() )) return ((BlockValueString *)m_values[n])->value(); qWarning( "array index %d out of bounds %d",n,size() ); return Utf8String(); }
QList<Utf8String> value() const { QList<Utf8String> vl; qint32 n = 0; while ( n < size() ) vl.append(at(n++)); return vl; }
            void  set( const QList<Utf8String> &vl ) { clear(); foreach( Utf8String v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
            bool  operator==(const QList<Utf8String>& l) const { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=at(i)) return false; return true; }
};

class BlockArrayByteArray : public KeyValueArray
{ public:
        explicit  BlockArrayByteArray( QObject *parent = nullptr ) : KeyValueArray( RCD_byteArrayArray_B, parent ) {}
        explicit  BlockArrayByteArray( RiceyInt k, QObject *parent = nullptr ) : KeyValueArray( k, parent ) {} // TODO: key type checking
                  BlockArrayByteArray( RiceyInt k, const QList<QByteArray> &v, QObject *parent = nullptr ) : KeyValueArray( k, parent ) { set(v); } // TODO: key type checking
                 ~BlockArrayByteArray() {}
      QByteArray  at( qint32 n ) const { if (( n >= 0 ) && ( n < size() )) return ((BlockValueByteArray *)m_values[n])->value(); qWarning( "array index %d out of bounds %d",n,size() ); return QByteArray(); }
QList<QByteArray> value() const { QList<QByteArray> vl; qint32 n = 0; while ( n < size() ) vl.append(at(n++)); return vl; }
            void  set( const QList<QByteArray> &vl ) { clear(); foreach( QByteArray v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
            bool  operator==(const QList<QByteArray>& l) const { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=at(i)) return false; return true; }
};

#endif // BLOCKOB_H
