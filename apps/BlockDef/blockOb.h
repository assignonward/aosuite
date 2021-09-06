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
#include <QPointer>
#include <QDataStream>
#include "riceyCodes.h"

void initKeyNames();

/**
 * @brief The BlockValue class - base class for all value types found in block objects
 */
class BlockValue : public QObject
{
    Q_OBJECT
public:
          explicit  BlockValue(QObject *parent = nullptr) : QObject( parent ) {}
                   ~BlockValue() {}
              void  setKey( QByteArray key ) { bKey = key; }
virtual      qint8  type() = 0;
virtual QByteArray  bsonish() = 0;
virtual QByteArray  json() = 0;

   QByteArray  bKey;   // Ricey code bsonish key, used in sort arbitration
};

/**
 * @brief The BlockOb class - a block has a key and a value
 */
class BlockOb : public QObject
{
    Q_OBJECT
public:
    explicit  BlockOb( const QByteArray &key, QObject *parent = nullptr ) : QObject( parent ) { setKey( key ); }
             ~BlockOb() { if ( bValue ) delete( bValue ); }
        void  setKey( const QByteArray &key ) { bKey = key; if ( bKey.size() > 0 ) type = bKey.at(bKey.size()-1) & 0x0F; else type = 0x7F; } // TODO: type checking
  QByteArray  key() { return bKey; }
        void  setValue( BlockValue &value ) { if ( bValue ) delete( bValue ); bValue = &value; if ( bValue ) bValue->setKey( bKey ); } // TODO: type checking
BlockValue &  value() { return *bValue; }
  QByteArray  bsonish() { QByteArray b = bKey; if ( bValue ) b.append( bValue->bsonish() ); return b; }
  QByteArray  json()    { return QByteArray(); }

public:
           QByteArray  bKey;   // Ricey code bsonish key
                qint8  type;
  QPointer<BlockValue> bValue; // Value of this BlockObject
};

class BlockValueObjectList : public BlockValue
{ public:
    explicit  BlockValueObjectList( QObject *parent = nullptr ) : BlockValue( parent ) {}
             ~BlockValueObjectList();
       qint8  type() { return RDT_OBJECT; }
  QByteArray  bsonish() { return QByteArray(); }
  QByteArray  json()    { return QByteArray(); }

  QList<QPointer<BlockOb> > obList;
};

class BlockValueInt64 : public BlockValue
{ public:
    explicit  BlockValueInt64( QObject *parent = nullptr ) : BlockValue( parent ) { value = 0; }
    ~BlockValueInt64() {}
       qint8  type() { return RDT_INT64; }
  QByteArray  bsonish() { QByteArray b; QDataStream s(b); s.setByteOrder(QDataStream::LittleEndian); s << value; return b; }
  QByteArray  json()    { return QString::number( value ).toUtf8(); }

      qint64  value;
};

class BlockValueInt32 : public BlockValue
{ public:
    explicit  BlockValueInt32( QObject *parent = nullptr ) : BlockValue( parent ) { value = 0; }
             ~BlockValueInt32() {}
       qint8  type() { return RDT_INT32; }
  QByteArray  bsonish() { QByteArray b; QDataStream s(b); s.setByteOrder(QDataStream::LittleEndian); s << value; return b; }
  QByteArray  json()    { return QString::number( value ).toUtf8(); }

      qint32  value;
};

class BlockValueRiceyCode : public BlockValue
{ public:
    explicit  BlockValueRiceyCode( QObject *parent = nullptr ) : BlockValue( parent ) {}
             ~BlockValueRiceyCode() {}
       qint8  type() { return RDT_RCODE; }
  QByteArray  bsonish() { return value; }
  QByteArray  json()    { return "\"0x"+value.toHex()+"\""; }

  QByteArray  value;
};

class BlockValueString : public BlockValue
{ public:
    explicit  BlockValueString( QObject *parent = nullptr ) : BlockValue( parent ) {}
             ~BlockValueString() {}
       qint8  type() { return RDT_STRING; }
  QByteArray  bsonish() { QByteArray b; QDataStream s(b); s.setByteOrder(QDataStream::LittleEndian); s << (qint32)value.size(); b.append( value ); return b; }
  QByteArray  json()    { return "\""+value+"\""; }

  QByteArray  value;  // Strings are encoded as UTF8
};

class BlockValueByteArray : public BlockValue
{ public:
    explicit  BlockValueByteArray( QObject *parent = nullptr ) : BlockValue( parent ) {}
             ~BlockValueByteArray() {}
       qint8  type() { return RDT_BYTEARRAY; }
  QByteArray  bsonish() { QByteArray b; QDataStream s(b); s.setByteOrder(QDataStream::LittleEndian); s << (qint32)value.size(); b.append( value ); return b; }
  QByteArray  json()    { return "\"0x"+value.toHex()+"\""; }

  QByteArray  value;
};

/*
RDT_MPZ       0x3
RDT_MPQ       0x4
*/

#endif // BLOCKOB_H
