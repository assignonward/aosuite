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
#include <QHash>
#include <QMap>
#include <QPointer>
#include <QDataStream>
#include "riceyCodes.h"

#include <stdio.h>
#include <stdarg.h>
#include <obstack.h>
#include <gmp.h>

      void initKeyNames();
QByteArray bsonishNull( qint8 );
/**
 * @brief The BlockValue class - base class for all value types found in block objects
 */
class BlockValue : public QObject
{
    Q_OBJECT
public:
          explicit  BlockValue(QObject *parent = nullptr) : QObject( parent ) {}
                   ~BlockValue() {}
              void  setKey( const QByteArray &key ) { m_key = key; }
virtual      qint8  type()    = 0;
virtual QByteArray  bsonish() = 0;
virtual QByteArray  json()    = 0;
virtual       bool  setBsonish( const QByteArray & ) = 0;
virtual       bool  setJson   ( const QByteArray & ) = 0;

   QByteArray  m_key;   // Ricey code bsonish key, used in sort arbitration, type checking
};

/**
 * @brief The BlockValueNull class - an empty value
 */
class BlockValueNull : public BlockValue
{ public:
    explicit  BlockValueNull( QObject *parent = nullptr ) : BlockValue( parent ) {}
             ~BlockValueNull() {}
       qint8  type()        { return RDT_NULL; }
  QByteArray  bsonish()     { QByteArray b; b.append((qint8)0); return b; }
  QByteArray  bsonishNull() { QByteArray b; b.append((qint8)0); return b; }
  QByteArray  json()        { QString s = "\"\""; return s.toUtf8(); }
        bool  setBsonish( const QByteArray &b ) { (void)b; return true; }
        bool  setJson   ( const QByteArray &j ) { (void)j; return true; }
};

/**
 * @brief The BlockObject class - a block has a single key and a value
 */
class BlockObject : public QObject
{
    Q_OBJECT
public:
    explicit  BlockObject( const QByteArray &key, QObject *parent = nullptr ) : QObject( parent ) { setKey( key ); }
             ~BlockObject() { if ( m_value ) delete( m_value ); }
        void  calcType( const QByteArray &key ) { if ( key.size() > 0 ) m_type = key.at(key.size()-1) & 0x0F; else m_type = 0x7F; }
        void  setKey( const QByteArray &key )   { m_key = key; calcType(key); }
       qint8  type()    { return m_type; }
  QByteArray  key()     { return m_key; }
        void  setValue( BlockValue &value )     { if ( m_value ) delete( m_value ); m_value = &value; if ( m_value ) m_value->setKey( m_key ); } // TODO: type checking
BlockValue &  value()   { return *m_value; }
  QByteArray  bsonish();
  QByteArray  json();

public:
                qint8  m_type;  // Type, extracted from bKey
           QByteArray  m_key;   // Ricey code bsonish key
  QPointer<BlockValue> m_value; // Value of this BlockObject
};

/**
 * @brief The BlockArray class - zero or more block objects of the same type
 */
class BlockArray : public QObject
{
    Q_OBJECT
public:
    explicit  BlockArray( const QByteArray &key, QObject *parent = nullptr ) : QObject( parent ) { setKey( key ); }
             ~BlockArray() { while ( m_values.size() > 0 ) { if ( m_values.last() != nullptr ) { delete( m_values.last() ); } m_values.removeLast(); } }
        void  calcType( const QByteArray &key ) { if ( key.size() > 0 ) m_type = key.at(key.size()-1) & 0x0F; else m_type = 0x7F; }
        void  setKey( const QByteArray &key )   { m_key = key; calcType(key); }
       qint8  type()    { return m_type; }
  QByteArray  key()     { return m_key; }
      qint32  size()    { return m_values.size(); }
        void  appendValue( BlockValue &value ) { m_values.append( &value ); value.setKey( m_key ); } // TODO: type checking
BlockValue &  valueAt( qint32 n ) { if (( n >= 0 ) && ( n < m_values.size() )) return *m_values.at(n); else return m_null; }
  QByteArray  bsonish() { return QByteArray(); }
  QByteArray  json()    { return QByteArray(); } // TODO: actual json() encoding including ricey code to string key conversion

public:
                        qint8  m_type;   // Type, extracted from bKey
                   QByteArray  m_key;    // Ricey code bsonish key
  QList<QPointer<BlockValue> > m_values; // Values in this BlockArray
               BlockValueNull  m_null;   // Returned when valueAt calls an invalid index
};

/**
 * @brief The BlockValueObject class - zero or more BlockObjects all with unique keys
 */
class BlockValueObject : public BlockValue
{ public:
         explicit  BlockValueObject( QObject *parent = nullptr ) : BlockValue( parent ) {}
                  ~BlockValueObject();
            qint8  type()    { return RDT_OBJECT; }
       QByteArray  bsonish() { return QByteArray(); }
       QByteArray  json()    { return QByteArray(); }
             bool  setBsonish( const QByteArray &b ) { (void)b; return true; }
             bool  setJson   ( const QByteArray &j ) { (void)j; return true; }
           qint32  size()    { return m_obMap.size(); }
             bool  contains( const QByteArray &k )   { return m_obMap.contains( k ); }
     BlockValue &  value( const QByteArray &k ) { if ( !contains(k) ) return m_null; return m_obMap.value(k)->value(); }

QMap<QByteArray, QPointer<BlockObject> > m_obMap;
                         BlockValueNull  m_null;   // Returned when valueAt calls an invalid index
};

class BlockValueInt64 : public BlockValue
{ public:
    explicit  BlockValueInt64( QObject *parent = nullptr ) : BlockValue( parent ) { m_value = 0; }
             ~BlockValueInt64() {}
       qint8  type()    { return RDT_INT64; }
  QByteArray  bsonish() { QByteArray b; QDataStream s(b); s.setByteOrder(QDataStream::LittleEndian); s << m_value; return b; }
  QByteArray  json()    { return QString::number( m_value ).toUtf8(); }
        bool  setBsonish( const QByteArray &b );
        bool  setJson   ( const QByteArray &j );
      qint64  value()   { return m_value; }
        void  setValue( qint64 v ) { m_value = v; }

      qint64  m_value;
};

class BlockValueInt32 : public BlockValue
{ public:
    explicit  BlockValueInt32( QObject *parent = nullptr ) : BlockValue( parent ) { m_value = 0; }
             ~BlockValueInt32() {}
       qint8  type()    { return RDT_INT32; }
  QByteArray  bsonish() { QByteArray b; QDataStream s(b); s.setByteOrder(QDataStream::LittleEndian); s << m_value; return b; }
  QByteArray  json()    { return QString::number( m_value ).toUtf8(); }
        bool  setBsonish( const QByteArray &b );
        bool  setJson   ( const QByteArray &j );
      qint32  value()   { return m_value; }
        void  setValue( qint32 v ) { m_value = v; }

      qint32  m_value;
};

class BlockValueRiceyCode : public BlockValue
{ public:
    explicit  BlockValueRiceyCode( QObject *parent = nullptr ) : BlockValue( parent ) {}
             ~BlockValueRiceyCode() {}
       qint8  type()    { return RDT_RCODE; }
  QByteArray  bsonish() { return m_value; }
  QByteArray  json()    { return "\""+m_value.toHex()+"\""; }
        bool  setBsonish( const QByteArray &b );
        bool  setJson   ( const QByteArray &j );
 static bool  validRicey( const QByteArray &b );
  QByteArray  value()   { return m_value; }
        void  setValue( QByteArray v ) { m_value = v; }

  QByteArray  m_value;
};

class BlockValueString : public BlockValue
{ public:
    explicit  BlockValueString( QObject *parent = nullptr ) : BlockValue( parent ) {}
             ~BlockValueString() {}
       qint8  type()    { return RDT_STRING; }
  QByteArray  bsonish() { QByteArray b; QDataStream s(b); s.setByteOrder(QDataStream::LittleEndian); s << (qint32)m_value.size(); b.append( m_value ); return b; }
  QByteArray  json();
        bool  setBsonish( const QByteArray &b );
        bool  setJson   ( const QByteArray &j );
  QByteArray  value()   { return m_value; }
        void  setValue( QByteArray v ) { m_value = v; }

  QByteArray  m_value;  // Strings are encoded as UTF8 in a QByteArray
};

class BlockValueByteArray : public BlockValue
{ public:
    explicit  BlockValueByteArray( QObject *parent = nullptr ) : BlockValue( parent ) {}
             ~BlockValueByteArray() {}
       qint8  type()    { return RDT_BYTEARRAY; }
  QByteArray  bsonish() { QByteArray b; QDataStream s(b); s.setByteOrder(QDataStream::LittleEndian); s << (qint32)m_value.size(); b.append( m_value ); return b; }
  QByteArray  json()    { return "\""+m_value.toHex()+"\""; }
        bool  setBsonish( const QByteArray &b );
        bool  setJson   ( const QByteArray &j );
  QByteArray  value()   { return m_value; }
        void  setValue( QByteArray v ) { m_value = v; }

  QByteArray  m_value;
};

class BlockValueMPZ : public BlockValue
{ public:
    explicit  BlockValueMPZ( QObject *parent = nullptr ) : BlockValue( parent ) {}
             ~BlockValueMPZ() {}
       qint8  type()    { return RDT_MPZ; }
  QByteArray  bsonish() { return QByteArray(); }
  QByteArray  json()    { return QByteArray(); }
        bool  setBsonish( const QByteArray &b ) { (void)b; return true; }
        bool  setJson   ( const QByteArray &j ) { (void)j; return true; }
      MP_INT  value()   { return m_value; }
        void  setValue( MP_INT v ) { m_value = v; }

      MP_INT  m_value;
};

class BlockValueMPQ : public BlockValue
{ public:
    explicit  BlockValueMPQ( QObject *parent = nullptr ) : BlockValue( parent ) {}
             ~BlockValueMPQ() {}
       qint8  type()    { return RDT_MPQ; }
  QByteArray  bsonish() { return QByteArray(); }
  QByteArray  json()    { return QByteArray(); }
        bool  setBsonish( const QByteArray &b ) { (void)b; return true; }
        bool  setJson   ( const QByteArray &j ) { (void)j; return true; }
      MP_RAT  value()   { return m_value; }
        void  setValue( MP_RAT v ) { m_value = v; }

      MP_RAT  m_value;
};

#endif // BLOCKOB_H
