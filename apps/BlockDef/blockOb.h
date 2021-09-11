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
QByteArray bsonishNull( qint8 );

/**
 * @brief The ValueBase class - base class for all value types found in block objects
 */
class ValueBase : public QObject
{
    Q_OBJECT
public:
          explicit  ValueBase(QObject *parent = nullptr) : QObject( parent ) {}
                   ~ValueBase() {}
virtual      qint8  type()    = 0;
virtual BsonSerial  bsonish() = 0;
virtual JsonSerial  json()    = 0;
virtual       bool  setBsonish( const BsonSerial & ) = 0;
virtual       bool  setJson   ( const JsonSerial & ) = 0;
};

/**
 * @brief The BlockValueNull class - an empty value with no defined type
 */
class BlockValueNull : public ValueBase
{ public:
    explicit  BlockValueNull( QObject *parent = nullptr ) : ValueBase( parent ) {}
             ~BlockValueNull() {}
       qint8  type()    { return RDT_NULL; }
  BsonSerial  bsonish() { QByteArray b; b.append((qint8)0); return b; }
  JsonSerial  json()    { QString s = "\"00\""; return s.toUtf8(); }
        bool  setBsonish( const BsonSerial &b ) { (void)b; return true; }
        bool  setJson   ( const JsonSerial &j ) { (void)j; return true; }
};

/**
 * @brief The KeyValueBase class - common base for objects that contain a key and one or more values
 */
class KeyValueBase : public ValueBase
{
    Q_OBJECT
public:
   explicit  KeyValueBase( const RiceyCode &key, QObject *parent = nullptr ) : ValueBase( parent )  { setKey( key ); }
            ~KeyValueBase() {}
       bool  setKey( const RiceyCode &key );
  RiceyCode  key()  { return m_key; }
      qint8  type() { if ( m_key.size() > 0 ) return m_key.at(m_key.size()-1) & 0x0F; return 0x7F; }

  RiceyCode  m_key; // Ricey code bsonish key
};

/**
 * @brief The KeyValuePair class - a single key and a single value (of any type)
 */
class KeyValuePair : public KeyValueBase
{
    Q_OBJECT
public:
    explicit  KeyValuePair( const RiceyCode &key, QObject *parent = nullptr ) : KeyValueBase( key, parent ) {}
             ~KeyValuePair() { if ( m_value ) delete( m_value ); }
        void  setValue( ValueBase &value ) { if ( value.type() != type() ) qWarning("type mismatch"); else { if ( m_value ) delete( m_value ); m_value = &value; } }
 ValueBase &  value() { return *m_value; }
  BsonSerial  bsonish();
  JsonSerial  json();
        bool  setBsonish( const BsonSerial & );
        bool  setJson   ( const JsonSerial &j ) { (void)j; return true; } // TODO: fixme

public:
  QPointer<ValueBase> m_value; // Value of this KeyValuePair
};

/**
 * @brief The KeyValueArray class - zero or more values of the same type stored under a single key
 */
class KeyValueArray : public KeyValueBase
{
    Q_OBJECT
public:
    explicit  KeyValueArray( const QByteArray &key, QObject *parent = nullptr ) : KeyValueBase( key, parent ) {}
             ~KeyValueArray() { while ( m_values.size() > 0 ) { if ( m_values.last() != nullptr ) { delete( m_values.last() ); } m_values.removeLast(); } }
      qint32  size()          { return m_values.size(); }
        void  appendValue( ValueBase &value ) { m_values.append( &value ); } // TODO: type checking
 ValueBase &  valueAt( qint32 n ) { if (( n >= 0 ) && ( n < m_values.size() )) return *m_values.at(n); else return m_null; }
  BsonSerial  bsonish();
  JsonSerial  json();
        bool  setBsonish( const BsonSerial &b ) { (void)b; return true; } // TODO: fixme
        bool  setJson   ( const JsonSerial &j ) { (void)j; return true; } // TODO: fixme

public:
  QVarLengthArray<QPointer<ValueBase> > m_values; // Values in this array
                        BlockValueNull  m_null;   // Returned when valueAt calls an invalid index
};

extern BlockValueNull  glob_null;   // Returned when BlockValueObject::valueAt() calls an invalid index

/**
 * @brief The BlockValueObject class - zero or more KeyValuePairs
 *   all with unique keys, types may be mixed.
 *
 * Keys are type o/O
 */
class BlockValueObject : public ValueBase
{ public:
         explicit  BlockValueObject( QObject *parent = nullptr ) : ValueBase( parent ) {}
                  ~BlockValueObject();
            qint8  type()    { return RDT_OBJECT; }
       BsonSerial  bsonish() { return BsonSerial(); } // TODO: fixme
       JsonSerial  json()    { return JsonSerial(); } // TODO: fixme
             bool  setBsonish(  const BsonSerial &b ) { (void)b; return true; } // TODO: fixme
             bool  setJson   (  const JsonSerial &j ) { (void)j; return true; } // TODO: fixme
           qint32  size()    { return m_obMap.size(); }
  QList<RiceyCode> keys()    { return m_obMap.keys(); }
             bool  contains  (  const RiceyCode &k ) { return m_obMap.contains( k ); }
             void  insert    (  const RiceyCode &k, ValueBase *v ) { m_obMap.insert( k, v ); }
        ValueBase *value     (  const RiceyCode &k ) { if ( !contains(k) ) return &glob_null; return m_obMap.value(k); }

QMap<RiceyCode, QPointer<ValueBase> > m_obMap;
};

/**
 * @brief The BlockValueInt64 class - single signed 64 bit integer
 *
 * Keys are type i/I
 */
class BlockValueInt64 : public ValueBase
{ public:
    explicit  BlockValueInt64( QObject *parent = nullptr ) : ValueBase( parent ) { m_value = 0; }
             ~BlockValueInt64() {}
       qint8  type()    { return RDT_INT64; }
  BsonSerial  bsonish() { BsonSerial b; QDataStream s(b); s.setByteOrder(QDataStream::LittleEndian); s << m_value; return b; }
  JsonSerial  json()    { return QString::number( m_value ).toUtf8(); }
        bool  setBsonish( const BsonSerial &b );
        bool  setJson   ( const JsonSerial &j );
      qint64  value()   { return m_value; }
        void  setValue( qint64 v ) { m_value = v; }

      qint64  m_value;
};

/**
 * @brief The BlockValueInt32 class - single signed 32 bit integer
 *
 * Keys are type l/L
 */
class BlockValueInt32 : public ValueBase
{ public:
    explicit  BlockValueInt32( QObject *parent = nullptr ) : ValueBase( parent ) { m_value = 0; }
             ~BlockValueInt32() {}
       qint8  type()    { return RDT_INT32; }
  BsonSerial  bsonish() { BsonSerial b; QDataStream s(b); s.setByteOrder(QDataStream::LittleEndian); s << m_value; return b; }
  JsonSerial  json()    { return QString::number( m_value ).toUtf8(); }
        bool  setBsonish( const BsonSerial &b );
        bool  setJson   ( const JsonSerial &j );
      qint32  value()   { return m_value; }
        void  setValue( qint32 v ) { m_value = v; }

      qint32  m_value;
};

/**
 * @brief The BlockValueRiceyCode class - a single Ricey Code, 1 to 7 octets in length
 *
 * Keys are type y/Y
 */
class BlockValueRiceyCode : public ValueBase
{ public:
    explicit  BlockValueRiceyCode( QObject *parent = nullptr ) : ValueBase( parent ) {}
             ~BlockValueRiceyCode() {}
       qint8  type()    { return RDT_RCODE; }
  BsonSerial  bsonish() { return m_value; }
  JsonSerial  json()    { return "\""+m_value.toHex()+"\""; }
        bool  setBsonish( const BsonSerial & );
        bool  setJson   ( const JsonSerial & );
   RiceyCode  value()   { return m_value; }
        void  setValue( RiceyCode v ) { if ( !validRicey( v ) ) qWarning("invalid ricey code"); else m_value = v; }

   RiceyCode  m_value;
};

/**
 * @brief The BlockValueString class - a UTF-8 encoded string
 *
 * Keys are type s/S
 */
class BlockValueString : public ValueBase
{ public:
    explicit  BlockValueString( QObject *parent = nullptr ) : ValueBase( parent ) {}
             ~BlockValueString() {}
       qint8  type()    { return RDT_STRING; }
  BsonSerial  bsonish();
  JsonSerial  json();
        bool  setBsonish( const BsonSerial &b );
        bool  setJson   ( const BsonSerial &j );
  Utf8String  value()   { return m_value; }
        void  setValue( Utf8String v ) { if ( m_value.size() > MAX_LENGTH ) qWarning("MAX_LENGTH exceeded"); else m_value = v; }

  Utf8String  m_value;  // Strings are encoded as UTF8 in a QByteArray
};

/**
 * @brief The BlockValueByteArray class - an array of octets
 *
 * Keys are type b/B
 */
class BlockValueByteArray : public ValueBase
{ public:
    explicit  BlockValueByteArray( QObject *parent = nullptr ) : ValueBase( parent ) {}
             ~BlockValueByteArray() {}
       qint8  type()    { return RDT_BYTEARRAY; }
  BsonSerial  bsonish() { BsonSerial b; QDataStream s(b); s.setByteOrder(QDataStream::LittleEndian); s << (qint32)m_value.size(); b.append( m_value ); return b; }
  JsonSerial  json()    { return "\""+m_value.toHex()+"\""; }
        bool  setBsonish( const BsonSerial & );
        bool  setJson   ( const JsonSerial & );
  QByteArray  value()   { return m_value; }
        void  setValue( QByteArray v ) { if ( m_value.size() > MAX_LENGTH ) qWarning("MAX_LENGTH exceeded"); else m_value = v; }

  QByteArray  m_value;
};

/**
 * @brief The BlockValueMPZ class - an arbitrary precision integer
 *
 * Keys are type n/N
 */
class BlockValueMPZ : public ValueBase
{ public:
    explicit  BlockValueMPZ( QObject *parent = nullptr ) : ValueBase( parent ) {}
             ~BlockValueMPZ() {}
       qint8  type()    { return RDT_MPZ; }
  BsonSerial  bsonish() { return QByteArray(); } // TODO: fixme
  JsonSerial  json()    { return QByteArray(); } // TODO: fixme
        bool  setBsonish( const BsonSerial &b ) { (void)b; return true; } // TODO: fixme
        bool  setJson   ( const JsonSerial &j ) { (void)j; return true; } // TODO: fixme
      MP_INT  value()   { return m_value; }
        void  setValue( MP_INT v ) { m_value = v; }

      MP_INT  m_value;
};

/**
 * @brief The BlockValueMPQ class - a fraction of two arbitrary precision integers
 *
 * Keys are type r/R
 */
class BlockValueMPQ : public ValueBase
{ public:
    explicit  BlockValueMPQ( QObject *parent = nullptr ) : ValueBase( parent ) {}
             ~BlockValueMPQ() {}
       qint8  type()    { return RDT_MPQ; }
  BsonSerial  bsonish() { return QByteArray(); } // TODO: fixme
  JsonSerial  json()    { return QByteArray(); } // TODO: fixme
        bool  setBsonish( const BsonSerial &b ) { (void)b; return true; } // TODO: fixme
        bool  setJson   ( const JsonSerial &j ) { (void)j; return true; } // TODO: fixme
      MP_RAT  value()   { return m_value; }
        void  setValue( MP_RAT v ) { m_value = v; }

      MP_RAT  m_value;
};

#endif // BLOCKOB_H
