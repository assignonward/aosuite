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
#include <QIODevice>
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
virtual            ~ValueBase() {}
static   ValueBase *newValue( RiceyInt k, QObject *parent = nullptr, ValueBase *vtc = nullptr );
static  JsonSerial  removeQuotes( const JsonSerial &j );
virtual       void  clear()         = 0;
virtual     quint8  type()    const = 0;
virtual BsonSerial  bsonish() const = 0;
virtual JsonSerial  json()    const = 0;
virtual  DotSerial  dot()     const { return json(); }
virtual     qint32  setBsonish( const BsonSerial & ) = 0;
         ValueBase *bsonishValueByKey( RiceyInt, const BsonSerial &, qint32 *l = nullptr, QObject *parent = nullptr );
         ValueBase *jsonValueByKey( RiceyInt, const QJsonValue &, QObject * );
virtual       bool  setJson( const JsonSerial & ) = 0;
virtual       bool  operator==( const ValueBase &v ) const { if ( v.type() != type() ) return false;  return v.bsonish() == bsonish(); }
        QByteArray  bsonishDefaultValue( qint8 ) const;
};

/**
 * @brief The BlockValueNull class - an empty value
 */
class BlockValueNull : public ValueBase
{ public:
    explicit  BlockValueNull( QObject *parent = nullptr ) : ValueBase( parent ) {}
             ~BlockValueNull() {}
        void  clear()          {}
      quint8  type()    const { return RDT_NULL; }
  BsonSerial  bsonish() const { return BsonSerial(); }
  JsonSerial  json()    const { QString s = "NULL"; return s.toUtf8(); }
      qint32  setBsonish( const BsonSerial &b ) { (void)b; return -1; }  // cannot set a value to a BlockValueNull
        bool  setJson   ( const JsonSerial &j ) { (void)j; return false; }
};

extern BlockValueNull  glob_null;   // Returned when BlockValueObject::valueAt() calls an invalid index
typedef QMap< RiceyInt, ValueBase *> BlockObjectMap;
typedef QVarLengthArray<ValueBase *> ValueArray;

/**
 * @brief The BlockValueArray class - a one dimensional array of zero or more ValueBase objects
 */
class BlockValueArray : public ValueBase
{ public:
       explicit  BlockValueArray( QObject *parent = nullptr ) : ValueBase( parent ) {}
                 BlockValueArray( const BlockValueArray &v, QObject *parent = nullptr ) : ValueBase( parent ) { m_values = v.m_values; }
                ~BlockValueArray() {}
BlockValueArray &operator= ( const BlockValueArray &v ) { clear(); m_values = v.m_values; return *this; }
           void  clear() { m_values.clear(); }
         qint32  size() const { return m_values.size(); }
 virtual quint8  type() const = 0;
     BsonSerial  bsonish() const;
     JsonSerial  json()    const;
      DotSerial  dot()     const;
         qint32  setBsonish( const BsonSerial & );
           bool  setJson( const JsonSerial & );
           bool  append( ValueBase * );
      ValueBase *at( qint32 n ) const { if (( n >= 0 ) && ( n < size() )) return m_values.at(n); else return nullptr; }
           bool  operator==( const BlockValueArray & ) const;

public:
     ValueArray  m_values; // Values in this array
};

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
     virtual void  clear()         {}
           qint32  setKey( const RiceyCode &r );
             bool  setKey( const RiceyInt &k ) { return( setKey( intToRice( k ) ) > 0 ); }
       Utf8String  keyHex()  const { return keyCode().toHex(); }
             char *keyHexd() const { return keyHex().data();   }
        RiceyCode  keyCode() const { return intToRice( m_key ); }
         RiceyInt  key()     const { return m_key; }
           quint8  type()    const { return (m_key & RDT_OBTYPEMASK); }
virtual DotSerial  dot()     const = 0;

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
              KeyValuePair( const RiceyInt &k, ValueBase *vp, QObject *parent = nullptr ) : KeyValueBase( k, parent ) { set(vp); }
              KeyValuePair( const BsonSerial &b, QObject *parent = nullptr ) : KeyValueBase( riceToInt(b), parent ) { setBsonish(b); }
             ~KeyValuePair() { if ( m_value ) m_value->deleteLater(); }
        void  set( ValueBase *vp ) { if ( vp->type() != type() ) qWarning("kvp type mismatch"); else { if ( m_value ) m_value->deleteLater(); m_value = vp; } }
   ValueBase *value() const { return m_value; }
  BsonSerial  bsonish()  const;
  JsonSerial  json()     const;
      qint32  setBsonish( const BsonSerial & );
        bool  setJson   ( const JsonSerial &j ) { (void)j; return true; } // TODO: fixme
   DotSerial  dot()      const;

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
        void  clear()         { m_value = 0; }
      quint8  type()    const { return RDT_INT64; }
  BsonSerial  bsonish() const { BsonSerial b; QDataStream s(&b,QIODevice::WriteOnly); s.setByteOrder(QDataStream::LittleEndian); s << m_value; return b; }
  JsonSerial  json()    const { return QString::number( m_value ).toUtf8(); }
      qint32  setBsonish( const BsonSerial &b );
        bool  setJson   ( const JsonSerial &j );
      qint64  value()   const { return m_value; }
        void  set( const qint64 &v ) { m_value = v; }
        bool  operator==( const qint64 &v )           const { return v         == value(); }
        bool  operator==( const BlockValueInt64 &v  ) const { return v.value() == value(); }

      qint64  m_value;
};

class BlockValueInt64Array : public BlockValueArray
{ public:
    explicit  BlockValueInt64Array( QObject *parent = nullptr ) : BlockValueArray( parent ) {}
              BlockValueInt64Array( const QList<qint64> &v, QObject *parent = nullptr ) : BlockValueArray( parent ) { set(v); }
             ~BlockValueInt64Array() {}
      quint8  type() const { return RDT_INT64_ARRAY; }
        bool  append( qint64 v ) { return this->BlockValueArray::append( new BlockValueInt64(v, this) ); }
        bool  append( ValueBase *v ) { return BlockValueArray::append( v ); }
      qint64  at( qint32 n ) const { if (( n >= 0 ) && ( n < size() )) return ((BlockValueInt64 *)m_values[n])->value(); qWarning( "array index %d out of bounds %d",n,size() ); return 0; }
QList<qint64> value() const { QList<qint64> vl; qint32 n = 0; while ( n < size() ) vl.append(at(n++)); return vl; }
        void  set( const QList<qint64> &vl ) { clear(); foreach( qint64 v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
        bool  operator==(const QList<qint64>& l) const { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=at(i)) return false; return true; }
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
        void  clear()         { m_value = 0; }
      quint8  type()    const { return RDT_RCODE; }
  BsonSerial  bsonish() const { return m_value; }
  JsonSerial  json()    const { return "\""+dict.nameFromCode(m_value)+"\""; }
      qint32  setBsonish ( const BsonSerial & );
        bool  setJson    ( const JsonSerial & );
   RiceyCode  value()    const { return m_value; }
    RiceyInt  valueInt() const { return riceToInt( m_value ); }
        void  set( const RiceyCode &v ) { if ( !validRicey( v ) ) qWarning("invalid ricey code"); else m_value = v; } // to detach or not to detach...
        void  set( const RiceyInt &r )  { set( intToRice( r ) ); }
        bool  operator==( const RiceyInt  &v ) const { return v == valueInt(); }
        bool  operator==( const RiceyCode &v ) const { return v == value(); }
        bool  operator==( const BlockValueRiceyCode &v  ) const { return v.value() == value(); }

   RiceyCode  m_value;
};

class BlockValueRiceyCodeArray : public BlockValueArray
{ public:
         explicit  BlockValueRiceyCodeArray( QObject *parent = nullptr ) : BlockValueArray( parent ) {}
                   BlockValueRiceyCodeArray( QList<RiceyInt>  v, QObject *parent = nullptr ) : BlockValueArray( parent ) { set(v); }
                   BlockValueRiceyCodeArray( QList<RiceyCode> v, QObject *parent = nullptr ) : BlockValueArray( parent ) { set(v); }
                  ~BlockValueRiceyCodeArray() {}
           quint8  type() const { return RDT_RCODE_ARRAY; }
             bool  append( RiceyInt  v ) { return this->BlockValueArray::append( new BlockValueRiceyCode(v, this) ); }
             bool  append( RiceyCode v ) { return this->BlockValueArray::append( new BlockValueRiceyCode(v, this) ); }
             bool  append( ValueBase *v ) { return BlockValueArray::append( v ); }
        RiceyCode  at( qint32 n )    const { if (( n >= 0 ) && ( n < size() )) return ((BlockValueRiceyCode *)m_values[n])->value(); qWarning( "array index %d out of bounds %d",n,size() ); return RiceyCode(); }
         RiceyInt  intAt( qint32 n ) const { if (( n >= 0 ) && ( n < size() )) return riceToInt( at(n) );                            qWarning( "array index %d out of bounds %d",n,size() ); return -1; }
  QList<RiceyCode> value()    const { QList<RiceyCode> vl; qint32 n = 0; while ( n < size() ) vl.append(   at(n++)); return vl; }
  QList<RiceyInt > valueInt() const { QList<RiceyInt > vl; qint32 n = 0; while ( n < size() ) vl.append(intAt(n++)); return vl; }
             void  set( const QList<RiceyCode> &vl ) { clear(); foreach( RiceyCode v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
             void  set( const QList<RiceyInt>  &vl ) { clear(); foreach( RiceyInt  v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
             bool  operator==(const QList<RiceyCode>& l) const { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=   at(i)) return false; return true; }
             bool  operator==(const QList<RiceyInt> & l) const { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=intAt(i)) return false; return true; }
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
static qint32  utf8CharSize( const char *, qint32 );
  static bool  validUtf8( const Utf8String & );
         void  clear()         { m_value.clear(); }
       quint8  type()    const { return RDT_STRING; }
   BsonSerial  bsonish() const;
   JsonSerial  json()    const;
       qint32  setBsonish( const BsonSerial &b );
         bool  setJson   ( const BsonSerial &j );
         void  set( const Utf8String &v ) { if ( m_value.size() > MAX_LENGTH ) qWarning("MAX_LENGTH exceeded"); else m_value = v; }
   Utf8String  value() const { return m_value; }
         bool  operator==( const Utf8String &v ) const { return v == value(); }
         bool  operator==( const BlockValueString &v  ) const { return v.value() == value(); }

   Utf8String  m_value;  // Strings are encoded as UTF8 in a QByteArray
};

class BlockValueStringArray : public BlockValueArray
{ public:
          explicit  BlockValueStringArray( QObject *parent = nullptr ) : BlockValueArray( parent ) {}
                    BlockValueStringArray( const QList<Utf8String> &v, QObject *parent = nullptr ) : BlockValueArray( parent ) { set(v); }
                   ~BlockValueStringArray() {}
            quint8  type() const { return RDT_STRING_ARRAY; }
              bool  append( Utf8String v ) { return this->BlockValueArray::append( new BlockValueString(v, this) ); }
              bool  append( ValueBase *v ) { return BlockValueArray::append( v ); }
        Utf8String  at( qint32 n ) const { if (( n >= 0 ) && ( n < size() )) return ((BlockValueString *)m_values[n])->value(); qWarning( "array index %d out of bounds %d",n,size() ); return Utf8String(); }
  QList<Utf8String> value() const { QList<Utf8String> vl; qint32 n = 0; while ( n < size() ) vl.append(at(n++)); return vl; }
              void  set( const QList<Utf8String> &vl ) { clear(); foreach( Utf8String v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
              bool  operator==(const QList<Utf8String>& l) const { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=at(i)) return false; return true; }
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
        void  clear()         { m_value.clear(); }
      quint8  type()    const { return RDT_BYTEARRAY; }
  BsonSerial  bsonish() const;
  JsonSerial  json()    const { return "\""+m_value.toHex()+"\""; }
      qint32  setBsonish( const BsonSerial & );
        bool  setJson   ( const JsonSerial & );
  QByteArray  value()   const { return m_value; }
        void  set( const QByteArray &v ) { if ( m_value.size() > MAX_LENGTH ) qWarning("MAX_LENGTH exceeded"); else m_value = v; }
        bool  operator==( const QByteArray &v ) const { return v == value(); }
        bool  operator==( const BlockValueByteArray &v  ) const { return v.value() == value(); }

  QByteArray  m_value;
};

class BlockValueByteArrayArray : public BlockValueArray
{ public:
          explicit  BlockValueByteArrayArray( QObject *parent = nullptr ) : BlockValueArray( parent ) {}
                    BlockValueByteArrayArray( const QList<QByteArray> &v, QObject *parent = nullptr ) : BlockValueArray( parent ) { set(v); }
                   ~BlockValueByteArrayArray() {}
            quint8  type() const { return RDT_BYTEARRAY_ARRAY; }
              bool  append( QByteArray v ) { return this->BlockValueArray::append( new BlockValueByteArray(v, this) ); }
              bool  append( ValueBase *v ) { return BlockValueArray::append( v ); }
        QByteArray  at( qint32 n ) const { if (( n >= 0 ) && ( n < size() )) return ((BlockValueByteArray *)m_values[n])->value(); qWarning( "array index %d out of bounds %d",n,size() ); return QByteArray(); }
  QList<QByteArray> value() const { QList<QByteArray> vl; qint32 n = 0; while ( n < size() ) vl.append(at(n++)); return vl; }
              void  set( const QList<QByteArray> &vl ) { clear(); foreach( QByteArray v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
              bool  operator==(const QList<QByteArray>& l) const { if (l.size() != size()) return false; for (qint32 i=0;i<size();i++) if (l.at(i)!=at(i)) return false; return true; }
};

/**
 * @brief The BlockValueMPZ class - an arbitrary precision integer
 *
 * Container's Keys are type n/N
 */
class BlockValueMPZ : public ValueBase
{ public:
          explicit  BlockValueMPZ( QObject *parent = nullptr ) : ValueBase( parent ) { mpz_init( &m_value ); }
                    BlockValueMPZ( const MP_INT &v, QObject *parent = nullptr ) : ValueBase( parent ) { mpz_init( &m_value ); set(v); }
                    BlockValueMPZ( const Utf8String &s, QObject *parent = nullptr ) : ValueBase( parent ) { mpz_init( &m_value ); setJson( "\""+s+"\"" ); }
                   ~BlockValueMPZ() { clear(); }
              void  clear()         { mpz_clear( &m_value ); }
            quint8  type()    const { return RDT_MPZ; }
 static Utf8String  toStr( const MP_INT & );
 static BsonSerial  toBCD( const MP_INT & );
 static     qint32  fromBCD( const BsonSerial &, mpz_t );
        BsonSerial  bsonish() const { return toBCD( value() ); }
        JsonSerial  json()    const { return "\""+toStr( value() )+"\""; }
            qint32  setBsonish( const BsonSerial &b ) { return fromBCD( b, &m_value ); }
              bool  setJson   ( const JsonSerial &j );
            MP_INT  value()   const { return m_value; }
              void  set( const MP_INT &v ) { mpz_set( &m_value, &v ); }
              bool  operator==( const MP_INT &v ) const { return ( mpz_cmp( &m_value, &v ) == 0 ); }
              bool  operator==( const BlockValueMPZ &v  ) const { MP_INT v1 = v.value(); return ( mpz_cmp( &m_value, &v1 ) == 0 ); }

            MP_INT  m_value;
};

class BlockValueMPZArray : public BlockValueArray
{ public:
      explicit  BlockValueMPZArray( QObject *parent = nullptr ) : BlockValueArray( parent ) {}
               ~BlockValueMPZArray() {}
        quint8  type() const { return RDT_MPZ_ARRAY; }
          bool  append( MP_INT v ) { return this->BlockValueArray::append( new BlockValueMPZ(v, this) ); }
          bool  append( ValueBase *v ) { return BlockValueArray::append( v ); }
        MP_INT  at( qint32 n ) const { if (( n >= 0 ) && ( n < size() )) return ((BlockValueMPZ *)m_values[n])->value(); MP_INT tv = temp; qWarning( "array index %d out of bounds %d",n,size() ); mpz_set_si( &tv, 0 ); return tv; }
  QList<MP_INT> value() const { QList<MP_INT> vl; qint32 n = 0; while ( n < size() ) vl.append(at(n++)); return vl; }
          void  set( const QList<MP_INT> &vl ) { clear(); foreach( MP_INT v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
          bool  operator==(const QList<MP_INT>& l) const { if (l.size() != size()) return false; MP_INT tv = temp; for (qint32 i=0;i<size();i++) { tv = at(i); if ( mpz_cmp( &l.at(i), &tv ) != 0 ) return false; } return true; }
        MP_INT  temp;
};

/**
 * @brief The BlockValueMPQ class - a fraction of two arbitrary precision integers
 *
 * Container's Keys are type r/R
 */
class BlockValueMPQ : public ValueBase
{ public:
    explicit  BlockValueMPQ( QObject *parent = nullptr ) : ValueBase( parent ) { mpq_init( &m_value ); }
              BlockValueMPQ( const MP_RAT &v, QObject *parent = nullptr ) : ValueBase( parent ) { mpq_init( &m_value ); set(v); }
              BlockValueMPQ( const Utf8String &s, QObject *parent = nullptr ) : ValueBase( parent ) { mpq_init( &m_value ); setJson( "\""+s+"\"" ); }
             ~BlockValueMPQ() { clear(); }
        void  clear()         { mpq_clear( &m_value ); }
      quint8  type()    const { return RDT_MPQ; }
static  Utf8String  toStr( const MP_RAT & );
  BsonSerial  bsonish() const;
  JsonSerial  json()    const { return "\""+toStr( value() )+"\""; }
      qint32  setBsonish( const BsonSerial &b );
        bool  setJson   ( const JsonSerial &j );
      MP_RAT  value()   const { return m_value; }
        void  set( const MP_RAT &v ) { mpq_set( &m_value, &v ); }
        bool  operator==( const MP_RAT &v ) const { return ( mpq_cmp( &m_value, &v ) == 0 ); }
        bool  operator==( const BlockValueMPQ &v  ) const { MP_RAT v1 = v.value(); return ( mpq_cmp( &m_value, &v1 ) == 0 ); }

      MP_RAT  m_value;
};

class BlockValueMPQArray : public BlockValueArray
{ public:
      explicit  BlockValueMPQArray( QObject *parent = nullptr ) : BlockValueArray( parent ) {}
               ~BlockValueMPQArray() {}
        quint8  type() const { return RDT_MPQ_ARRAY; }
          bool  append( MP_RAT v ) { return this->BlockValueArray::append( new BlockValueMPQ(v, this) ); }
          bool  append( ValueBase *v ) { return BlockValueArray::append( v ); }
        MP_RAT  at( qint32 n ) const { if (( n >= 0 ) && ( n < size() )) return ((BlockValueMPQ *)m_values[n])->value(); MP_RAT tv = temp; qWarning( "array index %d out of bounds %d",n,size() ); mpq_set_si( &tv, 1, 1 ); return tv; }
  QList<MP_RAT> value() const { QList<MP_RAT> vl; qint32 n = 0; while ( n < size() ) vl.append(at(n++)); return vl; }
          void  set( const QList<MP_RAT> &vl ) { clear(); foreach( MP_RAT v, vl ) { if ( !append( v ) ) qWarning( "append Failed" ); } }
          bool  operator==(const QList<MP_RAT>& l) const { if (l.size() != size()) return false;
                                                           MP_RAT tv;
                                                           for (qint32 i=0;i<size();i++)
                                                             { tv = at(i);
                                                               if ( mpq_cmp( &l.at(i), &tv ) != 0 ) return false;
                                                             }
                                                           return true;
                                                         }
        MP_RAT  temp;

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
         explicit  KeyValueArray( RiceyInt  k, QObject *parent = nullptr ) : KeyValueBase(            k, parent ) { m_val = nullptr; }
         explicit  KeyValueArray( RiceyInt  k, BlockValueArray *bva, QObject *parent = nullptr ) : KeyValueBase( k, parent ) { m_val = bva; }
         explicit  KeyValueArray( RiceyCode r, QObject *parent = nullptr ) : KeyValueBase( riceToInt(r), parent ) { m_val = nullptr; }
                  ~KeyValueArray() { clear(); }
             void  clear() { if ( m_val ) { m_val->clear(); m_val->deleteLater(); } m_val = nullptr; }
           qint32  size() const { if ( m_val ) return m_val->size(); return 0; }
             bool  append( ValueBase *value );
        ValueBase *at( qint32 n ) const { if ( m_val ) return m_val->at(n); return nullptr; }
             bool  typeMatch( quint8 );
       BsonSerial  bsonish() const;
       JsonSerial  json()    const;
           qint32  setBsonish( const BsonSerial &b );
             bool  setJson   ( const JsonSerial &j );
        DotSerial  dot()      const;

public:
  BlockValueArray *m_val;
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
                   BlockValueObject( QObject *parent, const JsonSerial &j ) : ValueBase( parent ) { setJson(j); }
                  ~BlockValueObject() { clear(); }
           quint8  type()    const { return RDT_OBJECT; }
       BsonSerial  bsonish() const;
       JsonSerial  json()    const;
           qint32  setBsonish(  const BsonSerial &b );
             bool  setJson   (  const JsonSerial &j );
        DotSerial  dot()      const;
             void  clear()   { QList<RiceyInt> keys = m_obMap.keys(); foreach( RiceyInt k, keys ) { if ( m_obMap[k] != nullptr ) m_obMap[k]->deleteLater(); m_obMap.remove(k); } }
           qint32  size() const { return m_obMap.size(); }
   QList<RiceyInt> keys() const { return m_obMap.keys(); }
             bool  contains( RiceyInt k ) const { return m_obMap.contains( k ); }
             bool  insert    ( RiceyInt, ValueBase * );
             bool  insert    ( KeyValueArray * );
           qint32  insert    ( const BlockObjectMap & );
             bool  insert    ( RiceyInt k, const BlockObjectMap &v ) { BlockValueObject    *vp = new BlockValueObject   ( v, this ); bool ok = insert( k, vp ); if ( !ok ) vp->deleteLater(); return ok; }             bool  insert    ( RiceyInt k,       qint64  v ) { BlockValueInt64     *vp = new BlockValueInt64    ( v, this ); bool ok = insert( k, vp ); if ( !ok ) vp->deleteLater(); return ok; }
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

   BlockObjectMap  m_obMap;
};

class BlockValueObjectArray : public BlockValueArray
{ public:
             explicit  BlockValueObjectArray( QObject *parent = nullptr ) : BlockValueArray( parent ) {}
                      ~BlockValueObjectArray() {}
//BlockValueObjectArray &operator= ( const QList<BlockObjectMap> &v ) { set(v); return *this; }
//BlockValueObjectArray &operator+= ( const BlockObjectMap &v )    { append(v); return *this; }
               quint8  type() const { return RDT_OBJECT_ARRAY; }
                 bool  append( const BlockObjectMap &v ) { return BlockValueArray::append( new BlockValueObject(v, this) ); }
                 bool  append( ValueBase *v ) { return BlockValueArray::append( v ); }
       BlockObjectMap  at( qint32 n ) const { if (( n >= 0 ) && ( n < size() )) return ((BlockValueObject *)m_values.at(n))->value(); qWarning( "array index %d out of bounds %d",n,size() ); return BlockObjectMap(); }
 QList<BlockObjectMap> value() const { QList<BlockObjectMap> vl; qint32 n = 0; while ( n < size() ) vl.append(at(n++)); return vl; }
                 void  set( const QList<BlockObjectMap> &v ) { clear(); for ( qint32 i = 0; i < v.size(); i++ ) { BlockObjectMap m = v.at(i); append( new BlockValueObject(m,this) ); } }
                 bool  operator==(const QList<BlockObjectMap>& l) const;
                 bool  operator==(const BlockValueObjectArray& o) const { return *this == o.value(); }
};

#endif // BLOCKOB_H
